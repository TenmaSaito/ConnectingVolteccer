//==================================================================================
// 
// メッシュフィールドクラスのソースファイル [meshField.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/7
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "meshSphere.h"
#include "manager.h"
#include "renderer.h"
#include "util.h"
#include "effect.h"

//==================================================================================
// --- メッシュスフィアの作成 ---
//==================================================================================
CMeshSphere *CMeshSphere::Create(const Vector3 &pos,
	const float fRadius,
	const int nNumXBlock,
	const int nNumZBlock,
	const Vector3 &vecQua,
	const float fAngle)
{
	CMeshSphere *pMeshSphere = nullptr;		// 生成したメッシュスフィアへのポインタ

	// メッシュスフィアの生成
	pMeshSphere = new CMeshSphere;
	if (pMeshSphere == nullptr)
	{ // 生成失敗時、nullを返す
		return nullptr;
	}

	// メッシュスフィアの初期化
	pMeshSphere->Init(pos, fRadius, nNumXBlock, nNumZBlock, vecQua, fAngle);

	// 生成したメッシュスフィアを返す
	return pMeshSphere;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMeshSphere::CMeshSphere(const int nPriority) : CObject(nPriority)
{ // メンバ変数のクリア
	m_pVtxBuff = nullptr;
	m_pIdxBuff = nullptr;
	m_nNumXBlock = 0;
	m_nNumZBlock = 0;
	m_nNumVtx = 0;
	m_nNumIdx = 0;
	m_pos = VECTOR3_NULL;
	m_vecQua = VECTOR3_NULL;
	m_fAngle = 0.0f;
	m_fRadius = 0.0f;

	// タイプ指定
	SetType(TYPE_MESHSPHERE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMeshSphere::~CMeshSphere()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CMeshSphere::Init(const Vector3 &pos,
	const float fRadius,
	const int nNumXBlock,
	const int nNumZBlock,
	const Vector3 &vecQua,
	const float fAngle)
{ // メンバ変数への代入
	m_pos = pos;
	m_nNumXBlock = nNumXBlock;
	m_nNumZBlock = nNumZBlock;
	m_fRadius = fRadius;
	m_vecQua = vecQua;
	m_fAngle = fAngle;

	LPDIRECT3DDEVICE9 pDevice = CManager::GetDeviceByInstance();		// デバイスへのポインタ
	VERTEX_3D *pVtx = nullptr;	// 頂点バッファへのポインタ
	WORD *pIdx = nullptr;		// インデックスへのポインタ

	// 頂点数を計算
	m_nNumVtx = (m_nNumXBlock * (m_nNumZBlock - 1)) + 2;

	// インデックス数を計算
	m_nNumIdx = (2 * m_nNumXBlock) + ((m_nNumZBlock - 2) * (4 + (2 * m_nNumXBlock))) - (m_nNumZBlock - 2);
	
	// 頂点バッファ生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * m_nNumVtx,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// インデックスバッファ生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// 頂点及びインデックス設定
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	SetTop(pVtx, pIdx);

	m_pIdxBuff->Unlock();
	m_pVtxBuff->Unlock();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMeshSphere::Uninit(void)
{ // バッファ解放
	SafeRelease(m_pVtxBuff);
	SafeRelease(m_pIdxBuff);

	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMeshSphere::Update(void)
{

}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CMeshSphere::Draw(void)
{
}

//==================================================================================
// --- 上の円の設定処理 ---
//==================================================================================
void CMeshSphere::SetTop(VERTEX_3D *pVtx, WORD *pIdx)
{
	Vector3 pos = VECTOR3_NULL;		// 頂点座標
	float fAngle = 0.0f;				// 円弧上の角度
	float fIncreaseAngle = DOUBLE_PI / m_nNumXBlock;			// 増加する角度
	float fDecreaseHeight = (m_fRadius * 2.0f) / m_nNumZBlock;	// 減少する高さ

	// 一番上の頂点を求める
	pos = m_pos;
	pos.y += m_fRadius;

	// 一番上の頂点とインデックスを設定
	pVtx[0].pos = pos;
	pIdx[0] = 0;

	// 上から二列目の頂点を設定
	for (int nCntCircle = 0; nCntCircle < m_nNumXBlock; nCntCircle++)
	{ // 座標を求める
		pos.x = sinf(fAngle) * fDecreaseHeight;
		pos.y = m_fRadius - fDecreaseHeight;
		pos.z = cosf(fAngle) * fDecreaseHeight;

		// インデックス設定
		pIdx[nCntCircle + 1] = nCntCircle + 1;

		// 角度を増加
		fAngle += fIncreaseAngle;
		fAngle = Util::FixedRotation(fAngle);

		CEffect::Create(pos, 1000, 5);
	}

	// インデックス設定
	pIdx[m_nNumXBlock + 1] = 1;
}

//==================================================================================
// --- 中間の設定処理 ---
//==================================================================================
void CMeshSphere::SetMiddle(VERTEX_3D *pVtx, WORD *pIdx)
{

}

//==================================================================================
// --- 下の円の設定処理 ---
//==================================================================================
void CMeshSphere::SetUnder(VERTEX_3D *pVtx, WORD *pIdx)
{
	Vector3 pos = VECTOR3_NULL;		// 頂点座標
	int nIdxOffset = m_nNumIdx - (m_nNumXBlock + 2);			// インデックスのオフセット
	float fAngle = 0.0f;				// 円弧上の角度
	float fIncreaseAngle = DOUBLE_PI / m_nNumXBlock;			// 増加する角度
	float fDecreaseHeight = (m_fRadius * 2.0f) / m_nNumZBlock;	// 減少する高さ

	// 一番上の頂点を求める
	pos = m_pos;
	pos.y += m_fRadius;

	// 一番上の頂点とインデックスを設定
	pVtx[nIdxOffset].pos = pos;
	pIdx[nIdxOffset] = 0;

	// 上から二列目の頂点を設定
	for (int nCntCircle = 0; nCntCircle < m_nNumXBlock; nCntCircle++)
	{ // 座標を求める
		pos.x = sinf(fAngle) * fDecreaseHeight;
		pos.y = -m_fRadius + fDecreaseHeight;
		pos.z = cosf(fAngle) * fDecreaseHeight;

		// インデックス設定
		pIdx[nIdxOffset + nCntCircle + 1] = nCntCircle + 1;

		// 角度を増加
		fAngle += fIncreaseAngle;
		fAngle = Util::FixedRotation(fAngle);

		CEffect::Create(pos, 1000, 5);
	}

	// インデックス設定
	pIdx[nIdxOffset + m_nNumXBlock + 1] = m_nNumIdx - 1;
}