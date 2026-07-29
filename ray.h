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
	CRay(const D3DXVECTOR3 &start, const D3DXVECTOR3 &vec, const float fLength);
	CRay(const D3DXVECTOR3 &start, const D3DXVECTOR3 &end);
	~CRay();

	void Draw(const int nFrame = DEFAULT_RAY_DRAWFRAME);
	void SetStart(const D3DXVECTOR3 &start);
	const D3DXVECTOR3 *GetStart(void) const { return &m_start; }
	void SetEnd(const D3DXVECTOR3 &end);
	const D3DXVECTOR3 *GetEnd(void) const { return &m_end; }
	void SetVector(const D3DXVECTOR3 &vector);
	const D3DXVECTOR3 *GetVector(void) const { return &m_vec; }
	void SetLength(const float fLength);
	float GetLength(void) const { return m_fLength; }
	D3DXVECTOR3 GetRay(void) const { return m_start + (m_vec * m_fLength); }

private:
	D3DXVECTOR3 m_start, m_end;			// 始点、終点
	D3DXVECTOR3 m_vec;		// 始点と方向ベクトル
	float m_fLength;		// ベクトルの長さ
};
#endif