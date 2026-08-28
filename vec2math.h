//==================================================================================
// 
// Vector2の計算関連関数をまとめたヘッダーファイル [vec2math.h]
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
// *** Vector2計算関連名前空間 ***
//**********************************************************************************
namespace Vec2
{
	//******************************************************************************
	// *** プロトタイプ宣言 ***
	//******************************************************************************
	float Length(const Vector2 &vec);
	float Length(const Vector2 &To, const Vector2 &From);
	float LengthSq(const Vector2 &vec);
	float LengthSq(const Vector2 &To, const Vector2 &From);
	float Dot(const Vector2 &vec1, const Vector2 &vec2);
	float Cross(const Vector2 &vec1, const Vector2 &vec2);
	bool IsVertical(const Vector2 &vec1, const Vector2 &vec2);
	bool IsParallel(const Vector2 &vec1, const Vector2 &vec2);
	bool IsInScreen(const Vector2 &pos);
	Vector2 Normalize(const Vector2 &vec);
	Vector2 Lerp(const Vector2 &start, const Vector2 &end, const float fT);
	Vector2 Middle(const Vector2 &p1, const Vector2 &p2);
	Vector2 Direction(const float fAngle);
	float Direction(const Vector2 &To, const Vector2 &From);
	Vector2 Random(void);
	Vector2 Random(const Vector2 &min, const Vector2 &max);
	Vector2 Clamp(const Vector2 &vec, const Vector2 &min, const Vector2 &max);
	Vector2 FixedRotation(const Vector2 &Rot);
	Vector2 Arc(const float fRadius, const float fRadian, const Vector2 &offset = VECTOR2_NULL);
	Vector2 &Fill(Vector2 &vec, const float fValue);
	Vector2 Fill(const float fValue);
	Vector2 ToSquareSize(const float fRadius);
	Vector2 ToRadian(const Vector2 &degree, const bool bFixed = false);
	Vector2 ToDegree(const Vector2 &radian, const bool bFixed = false);
	Vector3 ToVector3(const Vector2 &vec, const float fZ = 0.0f);
}
#endif