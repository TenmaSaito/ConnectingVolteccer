//==================================================================================
// 
// ランダムクラスのヘッダーファイル [rand.h]
// Author : TENMA SAITO
// Date   : 2026/5/25
// 
//==================================================================================
#ifndef _RAND_H_
#define _RAND_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** ランダムクラス ***
//**********************************************************************************
class CRand
{
public:
	static CRand *GetInstance(void);

	char Generate(const char cMin, const char cMax);
	int Generate(const int nMin, const int nMax);
	float Generate(const float fMin, const float fMax);
	double Generate(const double dMin, const double dMax);

private:
	CRand();
	~CRand();

	unsigned int m_nSeed;			// 乱数の種
};
#endif