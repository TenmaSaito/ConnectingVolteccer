//==================================================================================
// 
// テクスチャクラスのヘッダーファイル [texture.h]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
#ifndef _TEXTURE_H_		// インクルードガード
#define _TEXTURE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <vector>
#include <string>
#include <string_view>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define INVALID_TEX_ID	((UINT)-1)	// テクスチャの無効値

//**********************************************************************************
// *** テクスチャクラス ***
//**********************************************************************************
class CTexture
{
public:
	// 事前に読み込むテクスチャの種類
	typedef enum
	{
		TYPE_TRIANGLE = 0,		// 三角図形
		TYPE_CIRCLE,			// 円形
		TYPE_STAR,				// 星形
		TYPE_MAX
	} TYPE;

	// テクスチャデータ構造体
	struct BUFFER
	{
		LPDIRECT3DTEXTURE9 pTexture;	// テクスチャポインタ
		std::string sFilename;			// ファイル名
	};

	static CTexture *GetInstance(void);
	HRESULT Load(void);
	void Unload(void);
	UINT Register(const std::string_view path);
	UINT Register(const char *pPath);
	LPDIRECT3DTEXTURE9 GetAddress(const UINT uIdx);

private:
	CTexture();
	~CTexture();

	UINT Load(const std::string_view path);

	std::vector<BUFFER> m_vTexBuff;			// テクスチャの配列
};
#endif