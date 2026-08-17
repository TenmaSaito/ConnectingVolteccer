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

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define INVALID_TEXID	((UINT)-1)	// テクスチャの無効値

//**********************************************************************************
// *** テクスチャクラス ***
//**********************************************************************************
class CTexture
{
public:
	// テクスチャデータ構造体
	struct TEX_BUFFER
	{
		LPDIRECT3DTEXTURE9 pTexture;	// テクスチャポインタ
		std::string sFilename;			// ファイル名
	};

	static CTexture *GetInstance(void);
	HRESULT Load(void);
	void Unload(void);
	UINT Register(const char *pFileName);
	LPDIRECT3DTEXTURE9 GetAddress(const UINT uIdx);

private:
	CTexture();
	~CTexture();

	UINT Load(const char *pFileName);

	std::vector<TEX_BUFFER> m_vTexBuff;			// テクスチャの配列
};
#endif