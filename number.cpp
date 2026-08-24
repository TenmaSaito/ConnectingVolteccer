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
#include <string_view>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define INIT_WIDTH		(100)		// 初期の幅
#define INIT_HEIGHT		(100)		// 初期の高さ
#define TEX_PATH		"data/TEXTURE/Anim/voltNumber000.png"		// 数値のテクスチャ

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asNumberPath[CNumber::TYPE_MAX] =	// 数字テクスチャのパス
	{
		"data/TEXTURE/Anim/voltNumber000.png",		// 白文字に黄色のインライン
		"data/TEXTURE/Anim/voltNumber001.png",		// 黒文字に黄色のインライン
		"data/TEXTURE/Anim/voltNumber002.png",		// 黒文字に黄色のインライン (非立体)
		"data/TEXTURE/Anim/voltNumber003.png",		// 黒文字に黄色のインライン + 白のアウトライン
	};
}

//==================================================================================
// --- 数値オブジェクトの生成処理 (位置、サイズ、値指定) ---
//==================================================================================
CNumber *CNumber::Create(const TYPE type, const Vector3 &pos, const Vector2 &size, const int nValue)
{
	CNumber *pNumber = nullptr;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pNumber = new CNumber;
	if (pNumber == nullptr)
	{ // 生成に失敗した場合、NULLを返す
		return nullptr;
	}

	// 初期化処理
	pNumber->Init(type, pos, size, nValue);

	return pNumber;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CNumber::CNumber()
{
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
HRESULT CNumber::Init(const TYPE type, const Vector3 &pos, const Vector2 &size, const int nNumber)
{
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 各引数を保存
	m_pos = pos;
	m_size = size;
	m_nNumber = nNumber;
	m_col = COLOR_ONE;

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

	// TEXTUREをロード
	m_nIdxTexture = CTexture::GetInstance()->Register(c_asNumberPath[type]);

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
	pVtx[0].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = Color(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標設定
	int nUV = m_nNumber % 10;
	pVtx[0].tex = Vector2(0.1f * nUV, 0.0f);
	pVtx[1].tex = Vector2(0.1f * nUV + 0.1f, 0.0f);
	pVtx[2].tex = Vector2(0.1f * nUV, 1.0f);
	pVtx[3].tex = Vector2(0.1f * nUV + 0.1f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CNumber::Uninit(void)
{ // 頂点バッファの破棄
	SafeRelease(m_pVtxBuff);
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
{ // 描画しない場合はスキップ
	if (m_bDisp == false) return;

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
void CNumber::SetPosition(const Vector3 &position)
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
void CNumber::SetRotation(const Vector3 &rotation)
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
void CNumber::SetPositionAndRotation(const Vector3 &position, const Vector3 &rotation)
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
void CNumber::SetSize(const Vector2 &size)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ

	// サイズを保存
	m_size = size;

	// 対角線の長さと角度を求める
	m_fLength = sqrtf(powf(m_size.x, 2) + powf(m_size.y, 2)) * 0.5f;
	m_fAngle = atan2f(m_size.x, m_size.y);

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
// --- 色設定処理 ---
//==================================================================================
void CNumber::SetColor(const Color &col)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点情報へのポインタ

	m_col = col;		// 色を保存

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void **)&pVtx, 0);

	// 色を設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

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
	pVtx[0].tex = Vector2(0.1f * nUV, 0.0f);
	pVtx[1].tex = Vector2(0.1f * nUV + 0.1f, 0.0f);
	pVtx[2].tex = Vector2(0.1f * nUV, 1.0f);
	pVtx[3].tex = Vector2(0.1f * nUV + 0.1f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}