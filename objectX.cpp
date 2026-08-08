//==================================================================================
// 
// オブジェクトXクラスのソースファイル [objectX.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectX.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "input.h"
#include "camera.h"
#include "vec3math.h"
#include "matrix.h"
#include "playerCamera.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MODEL_SPD			(5.0f)		// モデルの移動スピード
#define MODEL_ROTSPD		(0.1f)		// モデルの回転スピード

//==================================================================================
// --- オブジェクト3Dの生成処理 ---
//==================================================================================
CObjectX *CObjectX::Create(const char *pXFileName,const Vector3& pos, const Vector3& rot)
{
	CObjectX *pObject3D = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pObject3D = new CObjectX;
	if (pObject3D == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pObject3D->Init(pXFileName, pos, rot);

	return pObject3D;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObjectX::CObjectX(const int nPriority) : CObject(nPriority)
{
	// メンバ変数をクリア
	m_pMesh = nullptr;
	m_pBuffMat = nullptr;
	m_pIdx = nullptr;
	m_dwNumMat = 0;
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_vtxMin = VECTOR3_NULL;
	m_vtxMax = VECTOR3_NULL;
	strcpy(m_aFileName, "");

	// タイプの設定
	SetType(TYPE_XMODEL);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObjectX::~CObjectX()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CObjectX::Init(const char *pXFileName, const Vector3 &pos, const Vector3 &rot)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// Xファイル読み込み
	LoadXFile(pXFileName);

	// 引数の値を保存
	m_pos = pos;
	m_rot = rot;

	// ファイル名保存
	strcpy_s(m_aFileName, pXFileName);
	
	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CObjectX::Uninit(void)
{
	// メッシュを解放
	SafeRelease(m_pMesh);

	// マテリアルを解放
	SafeRelease(m_pBuffMat);

	if (m_pIdx != nullptr)
	{ // インデックスを解放
		delete[] m_pIdx;
		m_pIdx = nullptr;
	}

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CObjectX::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CObjectX::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL *pMat = nullptr;		// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの設定
	Mtx::CalcWorld(&m_mtxWorld, m_pos, m_rot);

	//  ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//  現在のマテリアルを保存
	pDevice->GetMaterial(&matDef);

	// マテリアルを取得
	pMat = static_cast<D3DXMATERIAL*>(m_pBuffMat->GetBufferPointer());

	// 各マテリアルを描画
	for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_pIdx[nCntMat]));

		// モデル(パーツ)の描画
		m_pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==================================================================================
// --- Xファイルの読み込み処理 ---
//==================================================================================
HRESULT	CObjectX::LoadXFile(const char *pXFileName)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL *pMat = NULL;		// マテリアルへのポインタ
	HRESULT hr = S_OK;				// モデル読み込み結果
	int nNumVtx = 0;				// 頂点数
	DWORD dwSizeFVF = 0;			// 頂点フォーマットのサイズ
	BYTE *pVtxBuff = NULL;			// 頂点バッファへのポインタ

	// Xファイルの読み込み
	hr = D3DXLoadMeshFromX(pXFileName,			// 読み込むXファイル名
		D3DXMESH_SYSTEMMEM,
		pDevice,						// デバイスポインタ
		NULL,
		&m_pBuffMat,		// マテリアルへのポインタ
		NULL,
		&m_dwNumMat,		// マテリアルの数
		&m_pMesh);		// メッシュへのポインタ
	if (FAILED(hr))
	{ // 読み込み失敗
		return E_FAIL;
	}

	// マテリアル数分だけ、インデックス用バッファを確保
	m_pIdx = new int[static_cast<int>(m_dwNumMat)];
	memset(m_pIdx, -1, sizeof(int) * m_dwNumMat);

	// マテリアルデータへのポインタを取得
	pMat = static_cast<D3DXMATERIAL*>(m_pBuffMat->GetBufferPointer());

	for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
	{ // マテリアル数分だけテクスチャチェック
		if (pMat[nCntMat].pTextureFilename != NULL)
		{ // テクスチャの読み込み
			m_pIdx[nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
		}
	}

	// 頂点数を取得
	nNumVtx = m_pMesh->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	DWORD dwFvf = m_pMesh->GetFVF();
	dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// 頂点バッファをロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	// 頂点の最大、最小値を取得
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		Vector3 vtx = *(Vector3*)pVtxBuff;	// 頂点座標の代入

		// 最小値を取得
		m_vtxMin.x = (m_vtxMin.x > vtx.x) ? vtx.x : m_vtxMin.x;
		m_vtxMin.y = (m_vtxMin.y > vtx.y) ? vtx.y : m_vtxMin.y;
		m_vtxMin.z = (m_vtxMin.z > vtx.z) ? vtx.z : m_vtxMin.z;

		// 最大値を取得
		m_vtxMax.x = (m_vtxMax.x < vtx.x) ? vtx.x : m_vtxMax.x;
		m_vtxMax.y = (m_vtxMax.y < vtx.y) ? vtx.y : m_vtxMax.y;
		m_vtxMax.z = (m_vtxMax.z < vtx.z) ? vtx.z : m_vtxMax.z;

		pVtxBuff += dwSizeFVF;		// 頂点フォーマットのサイズ分ポインタを進める
	}

	/*** 頂点バッファをアンロック ***/
	m_pMesh->UnlockVertexBuffer();

	// 読み込み結果を返す
	return S_OK;
}