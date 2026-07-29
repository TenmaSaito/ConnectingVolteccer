//==================================================================================
// 
// メッシュオービット3Dクラスのソースファイル [meshOrbit3D.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/8
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "meshOrbit3D.h"
#include "manager.h"
#include "renderer.h"
#include "vec3math.h"

//==================================================================================
// --- メッシュオービット3Dの作成 ---
//==================================================================================
CMeshOrbit3D *CMeshOrbit3D::Create(const D3DXMATRIX *pMtxParent,
	const D3DXVECTOR3 &offset1,
	const D3DXVECTOR3 &offset2,
	const int nNumOrbit)
{
	CMeshOrbit3D* pMeshOrbit3D = nullptr;		// 生成したメッシュオービット3Dへのポインタ
	HRESULT hr = S_OK;							// 初期化の判定

	// メッシュオービットの生成
	pMeshOrbit3D = new CMeshOrbit3D;
	if (pMeshOrbit3D == nullptr)
	{ // 生成失敗時、nullを返す
		return nullptr;
	}

	// メッシュオービットの初期化
	hr = pMeshOrbit3D->Init(offset1, offset2, nNumOrbit);
	if (FAILED(hr))
	{ // 初期化失敗時、解放してnullを返す
		delete pMeshOrbit3D;
		return nullptr;
	}

	// 親マトリックスの登録
	pMeshOrbit3D->SetMtxParent(pMtxParent);

	// 生成したメッシュオービットを返す
	return pMeshOrbit3D;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMeshOrbit3D::CMeshOrbit3D(const int nPriority) : CMesh3D(nPriority)
{ // 各メンバ変数をクリア
	ZeroMemory(m_aOffset, sizeof(m_aOffset));
	m_pPosition = nullptr;

	// タイプの指定
	CObject::SetType(TYPE_ORBIT3D);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMeshOrbit3D::~CMeshOrbit3D()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CMeshOrbit3D::Init(const D3DXVECTOR3 &offset1,
	const D3DXVECTOR3 &offset2, 
	const int nNumOrbit)
{
	HRESULT hr = S_OK;		// 処理判定
	int nNumVtx = 4 + (2 * (nNumOrbit - 1));		// 頂点数
	int nNumIdx = 4 + (2 * (nNumOrbit - 1));		// インデックス数
	VERTEX_3D *pVtx = nullptr;		// 頂点バッファへのポインタ
	WORD *pIdx = nullptr;			// インデックスへのポインタ

	// 引数の保存
	m_aOffset[0] = offset1;
	m_aOffset[1] = offset2;

	// メッシュの初期化
	hr = CMesh3D::Init(nNumVtx, nNumIdx, nNumIdx - 2);
	if (FAILED(hr))
	{ // メッシュの初期化
		return E_FAIL;
	}

	// 生成したメッシュの頂点バッファを取得
	if (SUCCEEDED(CMesh::LockVertex(&pVtx)))
	{ // ロック成功
		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{ // 頂点バッファを更新
			pVtx[nCntVtx].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			pVtx[nCntVtx].col = Constant::White;
			pVtx[nCntVtx].tex = D3DXVECTOR2(static_cast<float>((nCntVtx % 4) / 2), static_cast<float>(nCntVtx % 2));
		}

		// ロック解除
		CMesh::UnlockVertex();
	}

	// 生成したメッシュのインデックスを設定
	if (SUCCEEDED(CMesh::LockIndex(&pIdx)))
	{ // ロック成功
		for (int nCntIdx = 0; nCntIdx < nNumIdx; nCntIdx++)
		{ // インデックス設定
			pIdx[nCntIdx] = nCntIdx;
		}

		// ロック解除
		CMesh::UnlockIndex();
	}

	// 頂点分の座標のバッファを確保
	m_pPosition = new D3DXVECTOR3[nNumVtx];
	if (m_pPosition == nullptr) return E_FAIL;

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMeshOrbit3D::Uninit(void)
{
	if (m_pPosition != nullptr)
	{ // バッファが確保されていれば
		delete[] m_pPosition;
		m_pPosition = nullptr;
	}

	// メッシュの終了
	CMesh3D::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMeshOrbit3D::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CMeshOrbit3D::Draw(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイスへのポインタ
	VERTEX_3D *pVtx = nullptr;				// 頂点バッファへのポインタ
	int nNumVtx = CMesh::GetVertexNum();	// 頂点数
	D3DXMATRIX mtxWorld;		// ワールドマトリックス

	// 頂点をずらす
	memmove(&m_pPosition[2], &m_pPosition[0], sizeof(D3DXVECTOR3) * (nNumVtx - 2));

	// 親マトリックスから頂点座標を求める
	D3DXVec3TransformCoord(&m_pPosition[0], &m_aOffset[0], m_pMtxParent);
	D3DXVec3TransformCoord(&m_pPosition[1], &m_aOffset[1], m_pMtxParent);

	// 生成したメッシュの頂点バッファを取得
	if (SUCCEEDED(CMesh::LockVertex(&pVtx)))
	{ // ロック成功
		for (int nCntVtx = 0; nCntVtx < CMesh::GetVertexNum(); nCntVtx++)
		{ // 頂点バッファを更新
			pVtx[nCntVtx].pos = m_pPosition[nCntVtx];
		}

		// ロック解除
		CMesh::UnlockVertex();
	}

	// 初期マトリックスを設定
	D3DXMatrixIdentity(&mtxWorld);
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	// カリングとライティングを無効に設定
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// メッシュの描画
	CMesh3D::Draw();

	// カリングとライティングを有効に設定
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//==================================================================================
// --- オフセットの設定処理 ---
//==================================================================================
void CMeshOrbit3D::SetOffset(const D3DXVECTOR3 &offset1,
	const D3DXVECTOR3 &offset2)
{// オフセットの保存
	m_aOffset[0] = offset1;
	m_aOffset[1] = offset2;
}

//==================================================================================
// --- 親マトリックスの登録処理 ---
//==================================================================================
void CMeshOrbit3D::SetMtxParent(const D3DXMATRIX *pMtxParent)
{ // マトリックスを登録し、位置を設定
	VERTEX_3D *pVtx = nullptr;				// 頂点バッファへのポインタ

	m_pMtxParent = pMtxParent;

	// 親マトリックスから頂点座標を求める
	D3DXVec3TransformCoord(&m_pPosition[0], &m_aOffset[0], m_pMtxParent);
	D3DXVec3TransformCoord(&m_pPosition[1], &m_aOffset[1], m_pMtxParent);

	// 生成したメッシュの頂点バッファを取得
	if (SUCCEEDED(CMesh::LockVertex(&pVtx)))
	{ // ロック成功
		for (int nCntVtx = 0; nCntVtx < CMesh::GetVertexNum(); nCntVtx++)
		{ // 頂点バッファを更新
			pVtx[nCntVtx].pos = m_pPosition[nCntVtx % 2];

			// 座標を保存
			m_pPosition[nCntVtx] = m_pPosition[nCntVtx % 2];
		}

		// ロック解除
		CMesh::UnlockVertex();
	}
}

//==================================================================================
// --- 先端と終端がほぼ同じ位置にいるかの判定処理 ---
//==================================================================================
bool CMeshOrbit3D::IsFinish(const float fEpsilon) const
{
	int nNumVtx = CMesh::GetVertexNum();		// 頂点数
	D3DXVECTOR3 midHead;		// 先端の中点
	D3DXVECTOR3 midTerminal;	// 終端の中点
	float fLength;				// 各中点間の距離

	// 各端の中点を求める
	midHead = Vec3::Middle(m_pPosition[0], m_pPosition[1]);
	midTerminal = Vec3::Middle(m_pPosition[nNumVtx - 2], m_pPosition[nNumVtx - 1]);

	// 各端の二点間の中点の距離で判定
	fLength = Vec3::Length(midTerminal, midHead);

	// 引数の値以下の場合同じ位置判定を返す
	return (fLength <= fEpsilon);
}