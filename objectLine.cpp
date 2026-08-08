//==================================================================================
// 
// オブジェクトラインクラスのソースファイル [objectLine.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/27
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectLine.h"
#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "vec3math.h"
#include "color.h"

//==================================================================================
// --- 生成処理 (始点と終点指定) ---
//==================================================================================
CObjectLine *CObjectLine::Create(const Vector3 &start, const Vector3 &end)
{
	CObjectLine *pLine = new CObjectLine;		// 生成したラインへのポインタ
	if (pLine != nullptr)
	{ // 生成出来ていれば初期化
		pLine->Init(start, end);
	}

	return pLine;
}

//==================================================================================
// --- 生成処理 (方向ベクトルと長さ指定) ---
//==================================================================================
CObjectLine *CObjectLine::Create(const Vector3 &origin, const Vector3 &vec, const float fLength)
{
	CObjectLine *pLine = new CObjectLine;		// 生成したラインへのポインタ
	if (pLine != nullptr)
	{ // 生成出来ていれば初期化
		pLine->Init(origin, vec, fLength);
	}

	return pLine;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObjectLine::CObjectLine(const int nPriority) : CObject(nPriority)
{ // メンバ変数のクリア
	m_pVtxBuff = nullptr;
	m_start = VECTOR3_NULL;
	m_end = VECTOR3_NULL;
	m_vec = VECTOR3_NULL;
	m_fLength = 0.0f;
	m_color = Color::GetColor(Color::COLOR_WHITE);
	m_nLife = 0;
	m_bDisp = true;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObjectLine::~CObjectLine()
{
}

//==================================================================================
// --- 初期化処理 (始点と終点指定) ---
//==================================================================================
HRESULT CObjectLine::Init(const Vector3 &start, const Vector3 &end)
{ // 引数を保存
	SetPoint(start, end);

	// 寿命を無限に設定
	m_nLife = INT_MAX;

	// 頂点作成
	return CreateVertex();
}

//==================================================================================
// --- 初期化処理 (方向ベクトルと長さ指定) ---
//==================================================================================
HRESULT CObjectLine::Init(const Vector3 &origin, const Vector3 &vec, const float fLength)
{ // 引数を保存
	SetVector(origin, vec, fLength);
	
	// 寿命を無限に設定
	m_nLife = INT_MAX;

	// 頂点作成
	return CreateVertex();
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CObjectLine::Uninit(void)
{ // 頂点破棄
	SafeRelease(m_pVtxBuff);

	// 親クラスの破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CObjectLine::Update(void)
{ // 頂点座標更新
	UpdateVertex();

	// 寿命減少
	m_nLife--;
	if (m_nLife <= 0)
	{ // 寿命が尽きたら破棄
		Uninit();
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CObjectLine::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetDeviceByInstance();		// デバイスの取得
	Matrix mtxWorld;		// ワールドマトリックス

	if (m_bDisp)
	{
		// 単位マトリックスを設定
		D3DXMatrixIdentity(&mtxWorld);
		pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

		// テクスチャ設定
		pDevice->SetTexture(0, nullptr);

		// 頂点フォーマット設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// Zテストを無効にする
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// ライティングを無効に設定
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

		// ラインの描画
		pDevice->DrawPrimitive(D3DPT_LINESTRIP,
			0,
			1);

		// ライティングを有効に設定
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

		// Zテストを有効にする
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}

//==================================================================================
// --- 頂点作成処理 ---
//==================================================================================
HRESULT CObjectLine::CreateVertex(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetDeviceByInstance();		// デバイスの取得
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ
	HRESULT hr = S_OK;				// 結果

	// 頂点作成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 2,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);

	// 頂点ロック
	m_pVtxBuff->Lock(0, 0, (void **)&pVtx, 0);

	// 頂点座標確認
	pVtx[0].pos = m_start;
	pVtx[1].pos = m_end;

	// 法線設定
	pVtx[0].nor = Vector3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = Vector3(0.0f, 1.0f, 0.0f);

	// 色
	pVtx[0].col = m_color;
	pVtx[1].col = m_color;

	// テクスチャ座標設定
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(0.0f, 0.0f);

	// ロック解除
	m_pVtxBuff->Unlock();

	return hr;
}

//==================================================================================
// --- 頂点更新処理 ---
//==================================================================================
void CObjectLine::UpdateVertex(void)
{
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 頂点ロック
	m_pVtxBuff->Lock(0, 0, (void **)&pVtx, 0);

	// 頂点座標確認
	pVtx[0].pos = m_start;
	pVtx[1].pos = m_end;

	// 法線設定
	pVtx[0].nor = Vector3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = Vector3(0.0f, 1.0f, 0.0f);

	// 色
	pVtx[0].col = m_color;
	pVtx[1].col = m_color;

	// ロック解除
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- ライン指定処理 (始点と終点指定) ---
//==================================================================================
void CObjectLine::SetPoint(const Vector3 &start, const Vector3 &end)
{ // 引数保存
	m_start = start;
	m_end = end;

	// 方向ベクトルと長さを計算
	m_vec = Vec3::Direction(end, start);
	m_fLength = Vec3::Length(end, start);
}

//==================================================================================
// --- ライン指定処理 (始点と方向ベクトルと長さ指定) ---
//==================================================================================
void CObjectLine::SetVector(const Vector3 &origin, const Vector3 &vec, const float fLength)
{ // 引数保存
	m_start = origin;
	m_vec = vec;
	m_fLength = fLength;

	// 終点を計算
	m_end = origin + (vec * fLength);
}