//==================================================================================
// 
// テクスチャクラスのソースファイル [texture.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "texture.h"
#include "manager.h"
#include "renderer.h"

//==================================================================================
// --- インスタンス取得処理 ---
//==================================================================================
CTexture *CTexture::GetInstance(void)
{
	static CTexture instance;		// インスタンス

	return &instance;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTexture::CTexture()
{ // 各メンバ変数のクリア
	memset(m_apTexture, 0, sizeof(m_apTexture));
	memset(m_apFileName, 0, sizeof(m_apFileName));
	m_nNumAll = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CTexture::~CTexture()
{
}

//==================================================================================
// --- 既存テクスチャの読み込み処理 ---
//==================================================================================
HRESULT CTexture::Load(void)
{
	return S_OK;
}

//==================================================================================
// --- 全テクスチャの解放処理 ---
//==================================================================================
void CTexture::Unload(void)
{
	for (int nCntRelease = 0; nCntRelease < m_nNumAll; nCntRelease++)
	{ // 読み込んだテクスチャの解放
		if (m_apTexture[nCntRelease] != nullptr)
		{ // テクスチャの解放
			m_apTexture[nCntRelease]->Release();
			m_apTexture[nCntRelease] = nullptr;
		}

		if (m_apFileName[nCntRelease] != nullptr)
		{ // 文字列を解放
			delete[] m_apFileName[nCntRelease];
			m_apFileName[nCntRelease] = nullptr;
		}
	}

	// 総数をリセット
	m_nNumAll = 0;
}

//==================================================================================
// --- テクスチャの登録処理 ---
//==================================================================================
int CTexture::Register(const char *pFileName)
{
	// 既に読み込んでいないかを確認
	for (int nCntTexture = 0; nCntTexture < m_nNumAll; nCntTexture++)
	{
		if (m_apFileName[nCntTexture] == nullptr)
		{ // nullptrならスキップ
			continue;
		}

		if (strcmp(m_apFileName[nCntTexture], pFileName) == 0)
		{ // 既に読み込み済みのテクスチャなら、そのインデックスを返す
			return nCntTexture;
		}
	}

	// 新規で読み込み
	return Load(pFileName);
}

//==================================================================================
// --- 読み込んだテクスチャの読み込み処理 ---
//==================================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(const int nIdx)
{
	// 無効なインデックスならnullptrを返す
	if (nIdx < 0) return nullptr;

	// テクスチャへのポインタを返す
	return m_apTexture[nIdx];
}

//==================================================================================
// --- テクスチャの読み込み処理 ---
//==================================================================================
int CTexture::Load(const char *pFileName)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetDeviceByInstance();	// テクスチャの読み込み
	int nIdx = m_nNumAll;		// 読み込んだテクスチャのインデックス

	// テクスチャを読み込み
	D3DXCreateTextureFromFile(pDevice,
		pFileName,
		&m_apTexture[m_nNumAll]);

	// ファイル名の長さを取得
	int nLenString = (int)strlen(pFileName) + 1;		// ファイル名の長さ

	// ファイル名を保存
	m_apFileName[m_nNumAll] = new char[nLenString];
	strcpy(m_apFileName[m_nNumAll], pFileName);

	// 総数増加
	m_nNumAll++;

	// インデックスを返す
	return nIdx;
}