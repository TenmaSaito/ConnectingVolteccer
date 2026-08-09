//==================================================================================
// 
// Xファイルクラスのソースファイル [xfile.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/22
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "xfile.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_CAPACITY		(64)		// インスタンス生成時確保するXDATAの初期サイズ

//==================================================================================
// --- インスタンス取得処理 ---
//==================================================================================
CXFile *CXFile::GetInstance(void)
{
	static CXFile instance;		// インスタンス

	return &instance;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CXFile::CXFile()
{ // メンバ変数のクリア
	m_vXData.reserve(DEFAULT_CAPACITY);
	m_nNumAll = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CXFile::~CXFile()
{
}

//==================================================================================
// --- Xファイルの登録処理 ---
//==================================================================================
int CXFile::Resister(const char *pXFileName, const bool bCopy)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL *pMat = NULL;		// マテリアルへのポインタ
	HRESULT hr = S_OK;				// モデル読み込み結果
	int nNumVtx = 0;				// 頂点数
	DWORD dwSizeFVF = 0;			// 頂点フォーマットのサイズ
	BYTE *pVtxBuff = NULL;			// 頂点バッファへのポインタ
	int nIdxXFile = m_vXData.size();		// インデックス

	for (const auto &data : m_vXData)
	{ // 既に読み込んでいないか確認
		if (data.sXFileName == pXFileName)
		{ // 読み込み済みの場合、そのインデックスを返す
			return data.nId;
		}
	}

	XDATA newData = {};		// 新規で読み込んだデータ

	// Xファイルの読み込み
	hr = D3DXLoadMeshFromX(pXFileName,			// 読み込むXファイル名
		D3DXMESH_SYSTEMMEM,
		pDevice,				// デバイスポインタ
		NULL,
		&newData.pBuffMat,		// マテリアルへのポインタ
		NULL,
		&newData.dwNumMat,		// マテリアルの数
		&newData.pMesh);		// メッシュへのポインタ
	if (FAILED(hr))
	{ // 読み込み失敗
		return -1;
	}

	// マテリアルデータへのポインタを取得
	pMat = static_cast<D3DXMATERIAL*>(newData.pBuffMat->GetBufferPointer());

	// サイズを確保
	newData.vIdx.reserve(newData.dwNumMat);

	for (int nCntMat = 0; nCntMat < static_cast<int>(newData.dwNumMat); nCntMat++)
	{ // マテリアル数分だけテクスチャチェック
		// テクスチャの読み込み
		newData.vIdx.push_back(pTexture->Register(pMat[nCntMat].pTextureFilename));
	}

	// 頂点数を取得
	nNumVtx = newData.pMesh->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	DWORD dwFvf = newData.pMesh->GetFVF();
	dwSizeFVF = D3DXGetFVFVertexSize(newData.pMesh->GetFVF());

	// 頂点バッファをロック
	newData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	// 頂点の最大、最小値を取得
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		Vector3 vtx = *(Vector3*)pVtxBuff;	// 頂点座標の代入

		// 最小値を取得
		newData.vtxMin.x = (newData.vtxMin.x > vtx.x) ? vtx.x : newData.vtxMin.x;
		newData.vtxMin.y = (newData.vtxMin.y > vtx.y) ? vtx.y : newData.vtxMin.y;
		newData.vtxMin.z = (newData.vtxMin.z > vtx.z) ? vtx.z : newData.vtxMin.z;

		// 最大値を取得
		newData.vtxMax.x = (newData.vtxMax.x < vtx.x) ? vtx.x : newData.vtxMax.x;
		newData.vtxMax.y = (newData.vtxMax.y < vtx.y) ? vtx.y : newData.vtxMax.y;
		newData.vtxMax.z = (newData.vtxMax.z < vtx.z) ? vtx.z : newData.vtxMax.z;

		pVtxBuff += dwSizeFVF;		// 頂点フォーマットのサイズ分ポインタを進める
	}

	// 頂点バッファをアンロック
	newData.pMesh->UnlockVertexBuffer();

	// 文字列を保存
	newData.sXFileName.append(pXFileName);

	// データを移す
	m_vXData.push_back(std::move(newData));

	return nIdxXFile;
}

//==================================================================================
// --- Xファイルの破棄処理 ---
//==================================================================================
void CXFile::Unload(void)
{
	for (auto &rData : m_vXData)
	{ // 総数分チェック
		if (rData.pMesh != nullptr)
		{ // メッシュを解放
			rData.pMesh->Release();
			rData.pMesh = nullptr;
		}

		if (rData.pBuffMat != nullptr)
		{ // マテリアルを解放
			rData.pBuffMat->Release();
			rData.pBuffMat = nullptr;
		}

		// インデックスを破棄
		rData.vIdx.clear();

		// 文字列を破棄
		rData.sXFileName.clear();
	}
}

//==================================================================================
// --- Xデータへのポインタ取得処理 ---
//==================================================================================
bool CXFile::GetAddress(const int nIdxXFile, XDATA **ppOut)
{
	if (nIdxXFile < 0 || nIdxXFile >= m_vXData.size())
	{ // インデックス外の場合失敗
		return false;
	}

	if (ppOut != nullptr)
	{ // アドレス先が有効なら
		*ppOut = &m_vXData[nIdxXFile];
	}

	// 書き出し成功
	return true;
}