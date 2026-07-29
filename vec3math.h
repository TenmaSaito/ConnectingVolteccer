//==================================================================================
// 
// D3DXVECTOR3の計算関連関数をまとめたヘッダーファイル [vec3math.h]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
#ifndef _VEC3MATH_H_
#define _VEC3MATH_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define VEC3_INLINE					// 各関数をインライン展開するか

//**********************************************************************************
// *** D3DXVECTOR3計算関連名前空間 ***
//**********************************************************************************
namespace Vec3
{
	//******************************************************************************
	// *** プロトタイプ宣言 ***
	//******************************************************************************
	float Length(const D3DXVECTOR3 &vec);
	float Length(const D3DXVECTOR3 &To, const D3DXVECTOR3 &From);
	float LengthSq(const D3DXVECTOR3 &vec);
	float LengthSq(const D3DXVECTOR3 &To, const D3DXVECTOR3 &From);
	float Dot(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool IsVertical(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool IsParallel(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool IsInsideTriangle(const D3DXVECTOR3 &pos, const D3DXVECTOR3 *pVtx, const bool bInverse = false);
	bool IsInsideViewOfBeside(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &posV, const D3DXVECTOR3 &posR, const float fovy);
	bool IsInsideViewOfVertical(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &posV, const D3DXVECTOR3 &posR, const float fovy);
	float Height(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &vtx, const D3DXVECTOR3 &nor);
	D3DXVECTOR3 Nor(const D3DXVECTOR3 &origin, const D3DXVECTOR3 *pVtx, const bool bInverse = false);
	D3DXVECTOR3 Cross(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	D3DXVECTOR3 Normalize(const D3DXVECTOR3 &vec);
	D3DXVECTOR3 Lerp(const D3DXVECTOR3 &start, const D3DXVECTOR3 &end, const float t);
	D3DXVECTOR3 Middle(const D3DXVECTOR3 &p1, const D3DXVECTOR3 &p2);
	D3DXVECTOR3 Direction(const D3DXVECTOR3 &To, const D3DXVECTOR3 &From);
	D3DXVECTOR3 Direction(const D3DXVECTOR3 &angle);
	bool Intersect(const LPD3DXBASEMESH pMesh, const D3DXVECTOR3 &pos, const D3DXVECTOR3 &ray, 
		const float *pLength, const DWORD *pFaceIndex = nullptr, const D3DXVECTOR2 *pUV = nullptr);
	D3DXVECTOR3 Random(void);
	D3DXVECTOR3 Clamp(const D3DXVECTOR3 &vec, const D3DXVECTOR3 &min, const D3DXVECTOR3 &max);
	D3DXVECTOR3 FixedRotation(const D3DXVECTOR3 &rot);
	D3DXVECTOR3 Arc(const float fRadius, const float fTheta, const float fPhi, const D3DXVECTOR3 &offset = VECTOR3_NULL);
	D3DXVECTOR3 &Fill(D3DXVECTOR3 &rVec, const float fValue);
	D3DXVECTOR3 Fill(const float fValue);
	D3DXVECTOR3 ToRadian(const D3DXVECTOR3 &degree, const bool bFixed = false);
	D3DXVECTOR3 ToDegree(const D3DXVECTOR3 &radian, const bool bFixed = false);
	D3DXVECTOR2 ToVector2(const D3DXVECTOR3 &vec);
}

#ifdef VEC3_INLINE
#include "vec3math.inl"
#endif
#endif