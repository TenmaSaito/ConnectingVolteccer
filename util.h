//==================================================================================
// 
// ユーティリティ関数をまとめたヘッダーファイル [vec2math.h]
// Author : TENMA SAITO
// Date   : 2026/6/22
// 
//==================================================================================
#ifndef _UTIL_H_
#define _UTIL_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <iostream>
#include <sstream>

//**********************************************************************************
// *** ユーティリティ関数関連名前空間 ***
//**********************************************************************************
namespace Util
{
	int FindString(const char **ppStr, const int nNumString, const char *pFind);
	float FixedRotation(const float fRadian);
	float Lerp(const float fStart, const float fEnd, const float ft);
	void SetStringSetting(const int nNumPrecision);
	template<class T> std::string ToString(const T &value);
#ifdef _DEBUG
	template<class T> T *GetTemp(const T &value);
#else
	template<class T>
	[[deprecated("is not safe function. please use local pointer.")]] T* GetTemp(const T& value);
#endif
}

//==================================================================================
// --- 文字列変換処理 ---
//==================================================================================
template<class T> std::string Util::ToString(const T &value)
{
	std::string str;				// 変換後文字列
	std::stringstream ss;			// 文字列のストリーム
	std::streambuf *pDefBuff;		// 元の出力ストリーム

	// 出力ストリーム先を文字列へ変更
	pDefBuff = std::cout.rdbuf(ss.rdbuf());

	// 変数を入力
	std::cout << value;

	// 文字列を代入
	str += ss.str();

	// 出力ストリーム先を元に戻す
	std::cout.rdbuf(pDefBuff);

	// 変換後の文字列を返す
	return str;
}

//==================================================================================
// --- 一時アドレス取得処理 ---
//==================================================================================
#ifdef _DEBUG
template<class T> T *Util::GetTemp(const T &value)
{
	static T tempValue;		// アドレス化用変数

	// バッファをコピー
	memcpy(&tempValue, &value, sizeof(T));

	// 変数へのアドレスを返す
	return &tempValue;
}
#else
template<class T>
[[deprecated("Is not safe function. Please use local pointer.")]] T* Util::GetTemp(const T& value)
{
	static T tempValue;		// アドレス化用変数

	// バッファをコピー
	memcpy(&tempValue, &value, sizeof(T));

	// 変数へのアドレスを返す
	return &tempValue;
}
#endif
#endif