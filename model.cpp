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
#include "model.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "matrix.h"

//==================================================================================
// --- オブジェクト3Dの生成処理 ---
//==================================================================================
CModel *CModel::Create(const char *pXFileName,
	const D3DXVECTOR3 &pos, 
	const D3DXVECTOR3& rot)
{
	CModel *pObject3D = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pObject3D = new CModel;
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
CModel::CModel()
{
	// メンバ変数をクリア
	m_pMesh = nullptr;
	m_pBuffMat = nullptr;
	m_pIdx = nullptr;
	m_dwNumMat = 0;
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_pParent = nullptr;
	ZeroMemory(&m_mtxWorld, sizeof(D3DXMATRIX));
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CModel::~CModel()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CModel::Init(const char *pXFileName, const D3DXVECTOR3 &pos, const D3DXVECTOR3 &rot)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL *pMat = NULL;		// マテリアルへのポインタ
	HRESULT hr = S_OK;				// モデル読み込み結果

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
		return -1;
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

	// 引数の値を保存
	m_pos = pos;
	m_rot = rot;
	m_posLocal = pos;
	m_rotLocal = rot;

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CModel::Uninit(void)
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
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CModel::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CModel::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL *pMat = nullptr;		// マテリアルへのポインタ
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATRIX mtxParent;				// 親マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの設定
	Mtx::CalcWorld(&m_mtxWorld, m_pos, m_rot);

	if (m_pParent != nullptr)
	{ // 親モデルが存在するなら、親モデルのマトリックス取得
		mtxParent = m_pParent->GetMtxWorld();
	}
	else
	{ // 存在しないなら、プレイヤ―のマトリックス取得
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// マトリックスを掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	// ワールドマトリックスの設定
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