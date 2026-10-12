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

	constexpr int GetNumInt(const int nValue)
	{ // 引数の桁数取得処理
		int nValueWork = nValue;		// 計算用変数
		int nNumValue = 1;				// 引数の桁数

		while (nValueWork / 10 > 0)
		{ // 10で割った値が0以下の場合ループ終了
			nNumValue++;
			nValueWork /= 10;
		}

		return nNumValue;
	}

	template<class E> E AddEnum(const E &num, const int nValue)
	{ // 列挙型の加減演算処理
		int nValueWork = static_cast<int>(num);
		nValueWork += nValue;
		return static_cast<E>(nValueWork);
	}
}
#endif