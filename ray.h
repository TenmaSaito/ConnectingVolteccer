//==================================================================================
// 
// 光線クラスのヘッダーファイル [ray.h]
// Author : TENMA SAITO
// Date   : 2026/7/17
// 
//==================================================================================
#ifndef _RAY_H_
#define _RAY_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <compare>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_RAY_DRAWFRAME		(1)		// デフォルトのレイの描画回数

//**********************************************************************************
// *** 光線クラス ***
//**********************************************************************************
class CRay
{
public:
	CRay();
	CRay(const Vector3 &start, const Vector3 &vec, const float fLength);
	CRay(const Vector3 &start, const Vector3 &end);
	~CRay();

	void Draw(const int nFrame = DEFAULT_RAY_DRAWFRAME);
	void SetStart(const Vector3 &start);
	const Vector3 *GetStart(void) const { return &m_start; }
	void SetEnd(const Vector3 &end);
	const Vector3 *GetEnd(void) const { return &m_end; }
	void SetVector(const Vector3 &vector);
	const Vector3 *GetVector(void) const { return &m_vec; }
	void SetLength(const float fLength);
	float GetLength(void) const { return m_fLength; }
	Vector3 GetRay(void) const { return m_start + (m_vec * m_fLength); }

	auto operator<=>(const CRay &ray) { return GetLength() <=> ray.GetLength(); }
	bool operator==(const CRay &ray) { return GetLength() == ray.GetLength(); }

private:
	Vector3 m_start, m_end;			// 始点、終点
	Vector3 m_vec;		// 始点と方向ベクトル
	float m_fLength;		// ベクトルの長さ
};
#endif