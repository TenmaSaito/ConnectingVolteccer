//==================================================================================
// 
// デバッグプロシージャクラスのヘッダーファイル [debugproc.h]
// Author : TENMA SAITO
// Date   : 2026/5/17
// 
//==================================================================================
#ifndef _DEBUGPROC_H_
#define _DEBUGPROC_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_SIZE	(23)		// 基本サイズ
#define DEFAULT_FONT	"Terminal"	// 基本フォント
#define MAX_STRING		(2048)		// 最大文字数

// VECTOR2の表示簡略マクロ
#define PRINT_VECTOR2(vec)	(vec.x, vec.y)		

// VECTOR3の表示簡略マクロ
#define PRINT_VECTOR3(vec)	vec.x, vec.y, vec.z		

//**********************************************************************************
// *** デバッグプロシージャクラス ***
//**********************************************************************************
class CDebugProc
{
public:
	CDebugProc();
	~CDebugProc();

	HRESULT Init(const UINT &rHeight, const char *pFontname);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Print(const char *pFmt, ...);

private:
	LPD3DXFONT m_pFont;			// フォントへのポインタ
	D3DXCOLOR m_colFont;		// フォントカラー
	char *m_pString;			// 文字列へのポインタ
};
#endif