//==================================================================================
// 
// オブジェクト2Dクラスのソースファイル [object2D.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/11
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object2D.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "texture.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define INIT_WIDTH		(100)		// 初期の幅
#define INIT_HEIGHT		(100)		// 初期の高さ
#define INIT_LIFE		(10000000)	// 初期体力
#define ACCEL_RESIST	(0.003f)		// 加速度の減少係数

//==================================================================================
// --- オブジェクト2Dの生成処理 ---
//==================================================================================
CObject2D *CObject2D::Create(const Vector3 &pos)
{
	CObject2D *pObject2D = NULL;		// 生成したオブジェクトへのポインタ

	if (CObject::GetNumAll() >= MAX_OBJECT)
	{ // オブジェクトの総数が配列の最大数だった場合
		// 生成せずにNULLを返す
		return NULL;
	}

	// オブジェクトの生成
	pObject2D = new CObject2D;
	if (pObject2D == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pObject2D->Init();

	// 位置を設定
	pObject2D->SetPosition(pos);

	return pObject2D;
}

//==================================================================================
// --- オブジェクト2Dの生成処理 (位置と角度指定) ---
//==================================================================================
CObject2D *CObject2D::Create(const Vector3 &pos, const Vector3 &rot)
{
	CObject2D* pObject2D = NULL;		// 生成したオブジェクトへのポインタ

	if (CObject::GetNumAll() >= MAX_OBJECT)
	{ // オブジェクトの総数が配列の最大数だった場合
		// 生成せずにNULLを返す
		return NULL;
	}

	// オブジェクトの生成
	pObject2D = new CObject2D;
	if (pObject2D == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pObject2D->Init();

	// 位置と角度を設定
	pObject2D->SetPositionAndRotation(pos, rot);

	return pObject2D;
}

//==================================================================================
// --- オブジェクト2Dの生成処理 (サイズ指定) ---
//==================================================================================
CObject2D *CObject2D::Create(const Vector2 &size)
{
	CObject2D *pObject2D = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{ // オブジェクトの総数が配列の最大数だった場合
		// 生成せずにNULLを返す
		return NULL;
	}

	// オブジェクトの生成
	pObject2D = new CObject2D;
	if (pObject2D == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pObject2D->Init(size);

	return pObject2D;
}

//==================================================================================
// --- オブジェクト2Dの生成処理 (位置、サイズ指定) ---
//==================================================================================
CObject2D *CObject2D::Create(const Vector3 &pos, const Vector2 &size)
{
	CObject2D *pObject2D = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{ // オブジェクトの総数が配列の最大数だった場合
		// 生成せずにNULLを返す
		return NULL;
	}

	// オブジェクトの生成
	pObject2D = new CObject2D;
	if (pObject2D == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pObject2D->Init(pos, size);

	return pObject2D;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObject2D::CObject2D(const int nPriority) : CObject(nPriority)
{
	// メンバ変数をクリア
	m_pVtxBuff = NULL;
	m_nIdxTexture = -1;
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_offset = VECTOR3_NULL;
	m_size = VECTOR2_NULL;
	m_color = Constant::ZeroCol;
	m_fLength = 0.0f;
	m_fAngle = 0.0f;
	m_bDisp = false;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObject2D::~CObject2D()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CObject2D::Init(void)
{
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_2D *pVtx = NULL;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

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

	// 変数の初期化
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_size = Vector2(INIT_WIDTH, INIT_HEIGHT);
	m_color = Constant::White;
	m_bDisp = true;

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
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 初期化処理 (位置指定) ---
//==================================================================================
HRESULT CObject2D::Init(const Vector3 &pos)
{
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_2D *pVtx = NULL;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

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

	// 変数の初期化
	m_rot = VECTOR3_NULL;
	m_size = Vector2(INIT_WIDTH, INIT_HEIGHT);
	m_color = Constant::White;
	m_bDisp = true;

	// 位置を保存
	m_pos = pos;

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
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 初期化処理 (位置と角度指定) ---
//==================================================================================
HRESULT CObject2D::Init(const Vector3 &pos, const Vector3 &rot)
{
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_2D* pVtx = NULL;		// 頂点情報へのポインタ
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

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

	// 変数の初期化
	m_size = Vector2(INIT_WIDTH, INIT_HEIGHT);
	m_color = Constant::White;
	m_bDisp = true;

	// 位置と角度を保存
	m_pos = pos;
	m_rot = rot;

	// 対角線の長さと角度を求める
	m_fLength = sqrtf(powf(INIT_WIDTH, 2) + powf(INIT_HEIGHT, 2)) * 0.5f;
	m_fAngle = atan2f(INIT_WIDTH, INIT_HEIGHT);

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
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 初期化処理 (サイズ指定) ---
//==================================================================================
HRESULT CObject2D::Init(const Vector2 &size)
{
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_2D *pVtx = NULL;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();	// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスへのポインタ

	// 頂点バッファ作成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr) || m_pVtxBuff == NULL)
	{ // 頂点バッファの生成に失敗した場合、エラーを返す
		return hr;
	}

	// 変数の初期化
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_color = Constant::White;
	m_bDisp = true;

	// サイズを保存
	m_size = size;

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
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 初期化処理 (位置、サイズ指定) ---
//==================================================================================
HRESULT CObject2D::Init(const Vector3 &pos, const Vector2 &size)
{
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_2D *pVtx = NULL;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();	// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスへのポインタ

	// 頂点バッファ作成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr) || m_pVtxBuff == NULL)
	{ // 頂点バッファの生成に失敗した場合、エラーを返す
		return hr;
	}

	// 変数の初期化
	m_rot = VECTOR3_NULL;
	m_color = Constant::White;
	m_bDisp = true;

	// 位置、サイズを保存
	m_pos = pos;
	m_size = size;

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
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 初期化処理 (位置、角度、サイズ指定) ---
//==================================================================================
HRESULT CObject2D::Init(const Vector3 &pos, const Vector3 &rot, const Vector2 &size)
{
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_2D* pVtx = NULL;		// 頂点情報へのポインタ
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();	// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスへのポインタ

	// 頂点バッファ作成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr) || m_pVtxBuff == NULL)
	{ // 頂点バッファの生成に失敗した場合、エラーを返す
		return hr;
	}

	// 変数の初期化
	m_color = Constant::White;
	m_bDisp = true;

	// 位置、角度、サイズを保存
	m_pos = pos;
	m_rot = rot;
	m_size = size;

	// 対角線の長さと角度を求める
	m_fLength = sqrtf(powf(size.x, 2) + powf(size.y, 2)) * 0.5f;
	m_fAngle = atan2f(size.x, size.y);

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(-m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(-m_fAngle) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_fAngle) * m_fLength;
	pVtx[2].pos.z = 0.0f;

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
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CObject2D::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff)
	{ // 確保されていれば解放する
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CObject2D::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CObject2D::Draw(void)
{
	if (m_bDisp == true)
	{ // 描画する場合のみ処理実行
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
}

//==================================================================================
// --- 位置設定処理 ---
//==================================================================================
void CObject2D::SetPosition(const Vector3 &position)
{
	VERTEX_2D *pVtx = NULL;		// 頂点情報へのポインタ

	// 位置を保存
	m_pos = position;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + m_offset.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + m_offset.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + m_offset.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + m_offset.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- オフセット設定処理 ---
//==================================================================================
void CObject2D::SetOffset(const Vector3 &offset)
{
	VERTEX_2D* pVtx = NULL;		// 頂点情報へのポインタ

	// 位置を保存
	m_offset = offset;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + m_offset.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + m_offset.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + m_offset.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + m_offset.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 角度設定処理 ---
//==================================================================================
void CObject2D::SetRotation(const Vector3 &rotation)
{
	VERTEX_2D *pVtx = NULL;		// 頂点情報へのポインタ

	// 角度を保存
	m_rot = rotation;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + m_offset.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + m_offset.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + m_offset.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + m_offset.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 位置+角度の設定処理 ---
//==================================================================================
void CObject2D::SetPositionAndRotation(const Vector3 &position, const Vector3 &rotation)
{
	VERTEX_2D *pVtx = NULL;		// 頂点情報へのポインタ

	// 位置+角度を保存
	m_pos = position;
	m_rot = rotation;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + m_offset.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + m_offset.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + m_offset.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + m_offset.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- サイズ設定処理 ---
//==================================================================================
void CObject2D::SetSize(const Vector2 &size)
{
	VERTEX_2D *pVtx = NULL;		// 頂点情報へのポインタ

	// サイズを保存
	m_size = size;

	// 対角線の長さと角度を求める
	m_fLength = sqrtf(powf(size.x, 2) + powf(size.y, 2)) * 0.5f;
	m_fAngle = atan2f(size.x, size.y);

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + m_offset.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + m_offset.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + m_offset.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + m_offset.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 対角線の長さ設定処理 ---
//==================================================================================
void CObject2D::SetLength(const float fLength)
{
	VERTEX_2D* pVtx = NULL;		// 頂点情報へのポインタ

	// サイズを調整
	m_size *= fLength / m_fLength;

	// 対角線の長さと角度を求める
	m_fLength = fLength * 0.5f;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI + m_fAngle + m_rot.y) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + m_offset.x + sinf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.y = m_pos.y + m_offset.y + cosf(D3DX_PI - m_fAngle + m_rot.y) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + m_offset.x + sinf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.y = m_pos.y + m_offset.y + cosf(-m_fAngle + m_rot.y) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + m_offset.x + sinf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.y = m_pos.y + m_offset.y + cosf(m_fAngle + m_rot.y) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 色の設定処理 ---
//==================================================================================
void CObject2D::SetColor(const D3DXCOLOR &color)
{
	VERTEX_2D *pVtx = NULL;		// 頂点情報へのポインタ

	// 色を保存
	m_color = color;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラー設定
	pVtx[0].col = m_color;
	pVtx[1].col = m_color;
	pVtx[2].col = m_color;
	pVtx[3].col = m_color;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}