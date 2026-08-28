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
UINT CXFile::Resister(const std::string_view sXFileName, const bool bCopy)
{ // ファイル名がnullの場合無効
	if (sXFileName.empty() == true) return INVALID_XFILE_ID;

	for (UINT uCnt = 0U; uCnt < m_vXData.size(); uCnt++)
	{ // 既に読み込まれていないか確認
		if (m_vXData.at(uCnt).sXFileName == sXFileName && bCopy == false)
		{ // 読み込まれている且つコピーを生成しない場合、そのインデックスを返す
			return uCnt;
		}
	}

	return Load(sXFileName.data());
}

//==================================================================================
// --- Xファイルの登録処理 (nullptr対策) ---
//==================================================================================
UINT CXFile::Resister(const char *pXFileName, const bool bCopy)
{ // ファイル名がnullの場合無効
	if (pXFileName == nullptr) return INVALID_XFILE_ID;

	// 存在すれば正式に処理を呼び出す
	return Resister(std::string_view(pXFileName));
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

	// 配列をクリア
	m_vXData.clear();
}

//==================================================================================
// --- Xデータへのポインタ取得処理 ---
//==================================================================================
bool CXFile::GetAddress(const UINT uIdxXFile, XDATA **ppOut)
{
	if (uIdxXFile >= m_vXData.size() || uIdxXFile == INVALID_TEX_ID)
	{ // インデックス外の場合失敗
		return false;
	}

	if (ppOut != nullptr)
	{ // アドレス先が有効なら
		*ppOut = &m_vXData[uIdxXFile];
	}

	// 書き出し成功
	return true;
}

//==================================================================================
// --- Xデータの読み込み処理 ---
//==================================================================================
UINT CXFile::Load(const char *pXFileName)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	UINT uIdxXFile = m_vXData.size();	// インデックス
	XDATA newData = {};				// 新規で読み込んだデータ
	HRESULT hr = S_OK;				// モデル読み込み結果

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
		return INVALID_XFILE_ID;
	}

	D3DXMATERIAL *pMat = NULL;		// マテリアルへのポインタ
	int nNumVtx = 0;				// 頂点数
	DWORD dwSizeFVF = 0;			// 頂点フォーマットのサイズ
	BYTE *pVtxBuff = NULL;			// 頂点バッファへのポインタ

	// マテリアルデータへのポインタを取得
	pMat = static_cast<D3DXMATERIAL *>(newData.pBuffMat->GetBufferPointer());

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
	newData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void **)&pVtxBuff);

	// 頂点の最大、最小値を取得
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		Vector3 vtx = *(Vector3 *)pVtxBuff;	// 頂点座標の代入

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

	// データを移してインデックスを返す
	m_vXData.push_back(std::move(newData));
	return uIdxXFile;
}