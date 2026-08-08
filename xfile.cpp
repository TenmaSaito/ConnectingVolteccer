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
	ZeroMemory(m_aXData, sizeof(m_aXData));
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
	int nIdxXFile = m_nNumAll;		// インデックス
	XDATA *pData = &m_aXData[nIdxXFile];		// データへのポインタ

	if (m_nNumAll >= MAX_XFILE)
	{ // 最大数をオーバーした場合読み込みスキップ
		return -1;
	}

	for (int nCntXFile = 0; nCntXFile < MAX_XFILE; nCntXFile++)
	{ // 既に読み込んでいないか確認
		if (m_aXData[nCntXFile].pXFileName == nullptr)
		{ // nullの場合スキップ
			continue;
		}

		if (strcmp(m_aXData[nCntXFile].pXFileName, pXFileName) == 0)
		{ // 読み込み済みの場合、そのインデックスを返す
			return nCntXFile;
		}
	}

	// Xファイルの読み込み
	hr = D3DXLoadMeshFromX(pXFileName,			// 読み込むXファイル名
		D3DXMESH_SYSTEMMEM,
		pDevice,				// デバイスポインタ
		NULL,
		&pData->pBuffMat,		// マテリアルへのポインタ
		NULL,
		&pData->dwNumMat,		// マテリアルの数
		&pData->pMesh);			// メッシュへのポインタ
	if (FAILED(hr))
	{ // 読み込み失敗
		return -1;
	}

	// マテリアル数分だけ、インデックス用バッファを確保
	pData->pIdx = new int[static_cast<int>(pData->dwNumMat)];
	memset(pData->pIdx, -1, sizeof(int) * pData->dwNumMat);

	// マテリアルデータへのポインタを取得
	pMat = static_cast<D3DXMATERIAL*>(pData->pBuffMat->GetBufferPointer());

	for (int nCntMat = 0; nCntMat < static_cast<int>(pData->dwNumMat); nCntMat++)
	{ // マテリアル数分だけテクスチャチェック
		if (pMat[nCntMat].pTextureFilename != NULL)
		{ // テクスチャの読み込み
			pData->pIdx[nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
		}
	}

	// 頂点数を取得
	nNumVtx = pData->pMesh->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	DWORD dwFvf = pData->pMesh->GetFVF();
	dwSizeFVF = D3DXGetFVFVertexSize(pData->pMesh->GetFVF());

	// 頂点バッファをロック
	pData->pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	// 頂点の最大、最小値を取得
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		Vector3 vtx = *(Vector3*)pVtxBuff;	// 頂点座標の代入

		// 最小値を取得
		pData->vtxMin.x = (pData->vtxMin.x > vtx.x) ? vtx.x : pData->vtxMin.x;
		pData->vtxMin.y = (pData->vtxMin.y > vtx.y) ? vtx.y : pData->vtxMin.y;
		pData->vtxMin.z = (pData->vtxMin.z > vtx.z) ? vtx.z : pData->vtxMin.z;

		// 最大値を取得
		pData->vtxMax.x = (pData->vtxMax.x < vtx.x) ? vtx.x : pData->vtxMax.x;
		pData->vtxMax.y = (pData->vtxMax.y < vtx.y) ? vtx.y : pData->vtxMax.y;
		pData->vtxMax.z = (pData->vtxMax.z < vtx.z) ? vtx.z : pData->vtxMax.z;

		pVtxBuff += dwSizeFVF;		// 頂点フォーマットのサイズ分ポインタを進める
	}

	// 頂点バッファをアンロック
	pData->pMesh->UnlockVertexBuffer();

	// 文字列の長さを取得
	int nLenString = static_cast<int>(strlen(pXFileName));

	// 文字列分メモリ確保
	pData->pXFileName = new char[nLenString + 1];

	// 文字列を保存
	strcpy(pData->pXFileName, pXFileName);

	return nIdxXFile;
}

//==================================================================================
// --- Xファイルの破棄処理 ---
//==================================================================================
void CXFile::Unload(void)
{
	for (auto &rData : m_aXData)
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

		if (rData.pIdx != nullptr)
		{ // インデックスを解放
			delete[] rData.pIdx;
			rData.pIdx = nullptr;
		}

		if (rData.pXFileName != nullptr)
		{ // 文字列を解放
			delete[] rData.pXFileName;
			rData.pXFileName = nullptr;
		}
	}
}

//==================================================================================
// --- Xデータへのポインタ取得処理 ---
//==================================================================================
bool CXFile::GetAddress(const int nIdxXFile, XDATA **ppOut)
{
	if (nIdxXFile < 0 || nIdxXFile >= MAX_XFILE)
	{ // インデックス外の場合失敗
		return false;
	}

	if (ppOut != nullptr)
	{ // アドレス先が有効なら
		*ppOut = &m_aXData[nIdxXFile];
	}

	// 書き出し成功
	return true;
}