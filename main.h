//==================================================================================
// 
// メインヘッダーファイル [main.h]
// Author : TENMA SAITO
// Date   : 2026/5/3
// 
//==================================================================================
#ifndef _MAIN_H_		// インクルードガード
#define _MAIN_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "pch.h"
#define _ENABLE_DBG_NEW
#include "memdbg.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define ENABLE_WINDOW		(TRUE)			// ウィンドウフラグ
#define ENABLE_NULLPOINTER_ASSERT			// NULL時のアサートの有効化マクロ
#ifdef ENABLE_NULLPOINTER_ASSERT
#define NULLPOINTER_ASSERT(pointer)		assert(pointer != nullptr)		// NULLアサート
#else
#define NULLPOINTER_ASSERT(pointer)		__noop(pointer)					// NULLアサート
#endif

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace Constant
{
	const Vector3 Middle = Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);		// スクリーンの中心座標
	const Color ZeroCol = Color(0, 0, 0, 0);	// Colorの0クリア
	const Color White = Color(1, 1, 1, 1);		// Colorの基本色(白)
}

//**********************************************************************************
// *** プロトタイプ宣言 ***
//**********************************************************************************
template<class T> void SafeRelease(T *&pRelease)
{ // 安全なCOMオブジェクト解放処理
	if (pRelease != nullptr)
	{ // nullptr出なければ破棄
		pRelease->Release();
		pRelease = nullptr;
	}
}
#endif