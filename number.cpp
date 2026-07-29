//==================================================================================
// 
// 数値表示クラスのソースファイル [number.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/11
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "number.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define INIT_WIDTH		(100)		// 初期の幅
#define INIT_HEIGHT		(100)		// 初期の高さ
#define TEX_PATH		"data/TEXTURE/Anim/number000.png"		// 数値のテクスチャ

//==================================================================================
// --- 数値オブジェクトの生成処理 (位置、サイズ、値指定) ---
//==================================================================================
CNumber *CNumber::Create(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nValue)
{
	CNumber *pNumber = nullptr;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pNumber = new CNumber;
	if (pNumber == nullptr)
	{ // 生成に失敗した場合、NULLを返す
		return nullptr;
	}

	// 初期化処理
	pNumber->Init(pos, size, nValue);

	return pNumber;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CNumber::CNumber()
{
	// メンバ変数をクリア
	m_pVtxBuff = nullptr;
	m_pos = D3DXVECTOR3(0, 0, 0);
	m_rot = D3DXVECTOR3(0, 0, 0);
	m_size = D3DXVECTOR2(0, 0);
	m_fLength = 0.0f;
	m_fAngle = 0.0f;
	m_nNumber = 0;
	m_nIdxTexture = -1;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CNumber::~CNumber()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CNumber::Init(void)
{
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// サイズを初期化
	m_size = D3DXVECTOR2(INIT_WIDTH, INIT_HEIGHT);
	m_nNumber = 0;

	// 頂点バッファ作成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr))
	{ // 頂点バッファの生成に失敗した場合、エラーを返す
		return hr;
	}

	// texload
	m_nIdxTexture = CTexture::GetInstance()->Register(TEX_PATH);

	// 対角線の長さと角度を求める
	m_fLength = sqrtf(powf(INIT_WIDTH, 2) + powf(INIT_HEIGHT, 2)) * 0.5f;
	m_fAngle = atan2f(INIT_WIDTH, INIT_HEIGHT);

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_fAngle) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 座標変換用変数設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラー設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CNumber::Init(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumber)
{
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 各引数を保存
	m_pos = pos;
	m_size = size;
	m_nNumber = nNumber;

	// 頂点バッファ作成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr))
	{ // 頂点バッファの生成に失敗した場合、エラーを返す
		return hr;
	}

	// texload
	m_nIdxTexture = CTexture::GetInstance()->Register(TEX_PATH);

	// 対角線の長さと角度を求める
	m_fLength = sqrtf(powf(size.x, 2) + powf(size.y, 2)) * 0.5f;
	m_fAngle = atan2f(size.x, size.y);

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_fAngle) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 座標変換用変数設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラー設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標設定
	int nUV = m_nNumber % 10;
	pVtx[0].tex = D3DXVECTOR2(0.1f * nUV, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(0.1f * nUV + 0.1f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.1f * nUV, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(0.1f * nUV + 0.1f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CNumber::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{ // 確保されていれば解放する
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CNumber::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CNumber::Draw(void)
{
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// テクスチャ設定
	pDevice->SetTexture(0, CTexture::GetInstance()->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);
}

//==================================================================================
// --- 位置設定処理 ---
//==================================================================================
void CNumber::SetPosition(const D3DXVECTOR3 &position)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 位置を保存
	m_pos = position;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 角度設定処理 ---
//==================================================================================
void CNumber::SetRotation(const D3DXVECTOR3 &rotation)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 角度を保存
	m_rot = rotation;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 位置+角度の設定処理 ---
//==================================================================================
void CNumber::SetPositionAndRotation(const D3DXVECTOR3 &position, const D3DXVECTOR3 &rotation)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 位置+角度を保存
	m_pos = position;
	m_rot = rotation;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- サイズ設定処理 ---
//==================================================================================
void CNumber::SetSize(const D3DXVECTOR2 &size)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ

	// サイズを保存
	m_size = size;

	// 対角線の長さと角度を求める
	m_fLength = sqrtf(powf(size.x, 2) + powf(size.y, 2)) * 0.5f;
	m_fAngle = atan2f(size.x, size.y);

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 値設定処理 ---
//==================================================================================
void CNumber::SetNumber(const int nValue)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 値を保存
	m_nNumber = nValue;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標設定
	int nUV = m_nNumber % 10;
	pVtx[0].tex = D3DXVECTOR2(0.1f * nUV, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(0.1f * nUV + 0.1f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.1f * nUV, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(0.1f * nUV + 0.1f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}