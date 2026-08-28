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
#include <string_view>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_CAPACITY		(64)		// インスタンス生成時確保するTEX_BUFFERの初期サイズ

//**********************************************************************************
// *** 図形のファイル名 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_sPrimitivePath[CTexture::TYPE_MAX] =	// 図形テクスチャのパス
	{
		"data/TEXTURE/base/triangle.png",	// 三角形
		"data/TEXTURE/base/circle.png",		// 円形
		"data/TEXTURE/base/star.png",		// 星型
	};
}

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
{ // メンバ変数のクリア + バッファの事前確保
	m_vTexBuff.reserve(DEFAULT_CAPACITY);
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
	for (const auto &path : c_sPrimitivePath)
	{ // 事前に用意されたテクスチャを読み込み
		Register(path);
	}

	return S_OK;
}

//==================================================================================
// --- 全テクスチャの解放処理 ---
//==================================================================================
void CTexture::Unload(void)
{
	for (auto &texbuf : m_vTexBuff)
	{ // 読み込んだテクスチャの解放
		if (texbuf.pTexture != nullptr)
		{ // テクスチャが読み込まれていた場合
			texbuf.pTexture->Release();
			texbuf.pTexture = nullptr;
		}

		// ファイル名をクリア
		texbuf.sFilename.clear();
	}

	// 配列をクリア
	m_vTexBuff.clear();
}

//==================================================================================
// --- テクスチャの登録処理 ---
//==================================================================================
UINT CTexture::Register(const std::string_view path)
{ // ファイル名がnullもしくはインデックスが無効値手前の場合失敗
	if (path.empty()) return INVALID_TEX_ID;
	if (m_vTexBuff.size() == INVALID_TEX_ID - 1U) return INVALID_TEX_ID;

	// 既に読み込んでいないかを確認
	for (UINT uCntTexture = 0; uCntTexture < m_vTexBuff.size(); uCntTexture++)
	{
		if (m_vTexBuff.at(uCntTexture).sFilename == path)
		{ // 既に読み込み済みのテクスチャなら、そのインデックスを返す
			return uCntTexture;
		}
	}

	// 新規で読み込み
	return Load(path);
}

//==================================================================================
// --- テクスチャの登録処理 (nullptr対策) ---
//==================================================================================
UINT CTexture::Register(const char *pPath)
{ // nullptrの場合、無効値を渡す
	if (pPath == nullptr) return INVALID_TEX_ID;

	// 存在すれば正式に処理を呼び出す
	return Register(std::string_view(pPath));
}

//==================================================================================
// --- 読み込んだテクスチャの読み込み処理 ---
//==================================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(const UINT uIdx)
{ // 無効なインデックスならnullptrを返す
	if (uIdx >= m_vTexBuff.size() || uIdx == INVALID_TEX_ID) return nullptr;

	// テクスチャへのポインタを返す
	return m_vTexBuff.at(uIdx).pTexture;
}

//==================================================================================
// --- テクスチャの読み込み処理 ---
//==================================================================================
UINT CTexture::Load(const std::string_view path)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// テクスチャの読み込み
	UINT uIdxTex = m_vTexBuff.size();		// テクスチャのインデックス
	HRESULT hr = S_OK;			// 処理結果
	BUFFER texbuf = {};			// テクスチャのバッファ

	// テクスチャを読み込み
	hr = D3DXCreateTextureFromFile(pDevice,
		path.data(),
		&texbuf.pTexture);
	if (FAILED(hr))
	{
		return INVALID_TEX_ID;
	}
	
	// ファイル名を保存
	texbuf.sFilename.append(path);

	// 配列に保存
	m_vTexBuff.push_back(texbuf);

	// インデックスを返す
	return uIdxTex;
}