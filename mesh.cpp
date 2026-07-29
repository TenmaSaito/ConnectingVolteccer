//==================================================================================
// 
// メッシュクラスのソースファイル [mesh.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/30
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "mesh.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMesh::CMesh(const int nPriority) : CObject(nPriority)
{ // メンバ変数のクリア
	m_pVtxBuff = nullptr;
	m_pIdxBuff = nullptr;
	m_nIdxTexture = -1;
	m_nNumVtx = 0;
	m_nNumIdx = 0;
	m_nNumPrim = 0;
	m_vtxSize = 0;
	m_dwFlags = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMesh::~CMesh()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CMesh::Init(const int nNumVtx, const int nNumIdx, const int nNumPrim, const size_t vtxSize, const DWORD dwFlags)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetDeviceByInstance();		// デバイスへのポインタ
	HRESULT hr;					// 各バッファ生成の判定

	// 頂点バッファ生成
	hr = pDevice->CreateVertexBuffer(vtxSize * 4 * nNumVtx,
		D3DUSAGE_WRITEONLY,
		dwFlags,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);
	if (FAILED(hr))
	{ // バッファ生成失敗時
		return E_FAIL;
	}

	// インデックスバッファ生成
	hr = pDevice->CreateIndexBuffer(sizeof(WORD) * nNumIdx,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);
	if (FAILED(hr))
	{ // バッファ生成失敗時
		return E_FAIL;
	}

	// 引数の値を保存
	m_nNumVtx = nNumVtx;
	m_nNumIdx = nNumIdx;
	m_nNumPrim = nNumPrim;
	m_vtxSize = vtxSize;
	m_dwFlags = dwFlags;

	// 初期化結果を返す
	return S_OK;
}


//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMesh::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{ // NULLじゃなければ
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuff != nullptr)
	{ // NULLじゃなければ
		m_pIdxBuff->Release();
		m_pIdxBuff = nullptr;
	}

	// オブジェクトの破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMesh::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CMesh::Draw(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, m_vtxSize);

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(m_dwFlags);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	//*** インデックスを利用したポリゴンの描画 ***/
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		0,
		m_nNumVtx,		// 頂点数
		0,
		m_nNumPrim);	// 描画するプリミティブ(三角ポリゴン)の数
}