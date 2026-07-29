//==================================================================================
// 
// D3DXVECTOR2の計算関連関数をまとめたヘッダーファイル [vec2math.h]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
#ifndef _VEC2MATH_H_
#define _VEC2MATH_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define VEC2_INLINE				// 各関数をインライン展開するか

//**********************************************************************************
// *** D3DXVECTOR2計算関連名前空間 ***
//**********************************************************************************
namespace Vec2
{
	//******************************************************************************
	// *** プロトタイプ宣言 ***
	//******************************************************************************
	float Length(const D3DXVECTOR2 &vec);
	float Length(const D3DXVECTOR2 &To, const D3DXVECTOR2 &From);
	float LengthSq(const D3DXVECTOR2 &vec);
	float LengthSq(const D3DXVECTOR2 &To, const D3DXVECTOR2 &From);
	float Dot(const D3DXVECTOR2 &vec1, const D3DXVECTOR2 &vec2);
	float Cross(const D3DXVECTOR2 &vec1, const D3DXVECTOR2 &vec2);
	bool IsVertical(const D3DXVECTOR2 &vec1, const D3DXVECTOR2 &vec2);
	bool IsParallel(const D3DXVECTOR2 &vec1, const D3DXVECTOR2 &vec2);
	bool IsInScreen(const D3DXVECTOR2 &pos);
	D3DXVECTOR2 Normalize(const D3DXVECTOR2 &vec);
	D3DXVECTOR2 Lerp(const D3DXVECTOR2 &start, const D3DXVECTOR2 &end, const float fT);
	D3DXVECTOR2 Middle(const D3DXVECTOR2 &p1, const D3DXVECTOR2 &p2);
	D3DXVECTOR2 Direction(const float fAngle);
	float Direction(const D3DXVECTOR2 &To, const D3DXVECTOR2 &From);
	D3DXVECTOR2 Random(void);
	D3DXVECTOR2 Clamp(const D3DXVECTOR2 &vec, const D3DXVECTOR2 &min, const D3DXVECTOR2 &max);
	D3DXVECTOR2 FixedRotation(const D3DXVECTOR2 &Rot);
	D3DXVECTOR2 Arc(const float fRadius, const float fRadian, const D3DXVECTOR2 &offset = VECTOR2_NULL);
	D3DXVECTOR2 &Fill(D3DXVECTOR2 &vec, const float fValue);
	D3DXVECTOR2 Fill(const float fValue);
	D3DXVECTOR2 ToSquareSize(const float fRadius);
	D3DXVECTOR2 ToRadian(const D3DXVECTOR2 &degree, const bool bFixed = false);
	D3DXVECTOR2 ToDegree(const D3DXVECTOR2 &radian, const bool bFixed = false);
	D3DXVECTOR3 ToVector3(const D3DXVECTOR2 &vec, const float fZ = 0.0f);
}

#ifdef VEC2_INLINE
#include "vec2math.inl"
#endif
#endif