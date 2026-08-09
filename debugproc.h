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
#include <string>
#include <format>
#include <type_traits>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_SIZE	(23)		// 基本サイズ
#define DEFAULT_FONT	"Terminal"	// 基本フォント
#define DEFAULT_STRING_CAPACITY		(2048)			// 文字数の初期サイズ
#define PRINT_VECTOR2(vec)	(vec.x, vec.y)			// VECTOR2の表示簡略マクロ
#define PRINT_VECTOR3(vec)	vec.x, vec.y, vec.z		// VECTOR3の表示簡略マクロ

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
	template<class... Args>
	void Print(std::_Basic_format_string<char, std::type_identity_t<Args>...> format, Args ...args);

private:
	LPD3DXFONT m_pFont;			// フォントへのポインタ
	D3DXCOLOR m_colFont;		// フォントカラー
	std::string m_sProc;		// 表示する文字列
};

//==================================================================================
// --- デバッグ表示の追加処理 ---
//==================================================================================
template<class... Args> void CDebugProc::Print(const std::_Basic_format_string<char, std::type_identity_t<Args>...> format, Args ...args)
{
	m_sProc += std::format(format, std::forward<Args>(args)...);
}
#endif