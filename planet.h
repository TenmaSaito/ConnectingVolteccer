//==================================================================================
// 
// 惑星クラスのヘッダーファイル [planet.h]
// Author : TENMA SAITO
// Date   : 2026/6/23
// 
//==================================================================================
#ifndef _PLANET_H_
#define _PLANET_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CLasso;

//**********************************************************************************
// *** 惑星クラス ***
//**********************************************************************************
class CPlanet : public CObject
{
public:
	static CPlanet *Create(void);

	CPlanet();
	~CPlanet();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	float GetAngle(void) const { return m_fAngle; }
	const Vector3 *GetVecQua(void) const { return &m_vecQua; }
	const Matrix *GetMatrix(void) const { return &m_mtxWorld; }
	void MultiplyQuaternion(const Quaternion &quaMultiply) { m_qua = m_qua * quaMultiply; }
	const Quaternion *GetQuaternion(void) const { return &m_qua; }
	const Vector3 *GetPosition(void) const { return &m_pos; }

private:
	Vector3 m_pos;			// 惑星の位置
	Vector3 m_vecQua;		// 任意軸
	float m_fAngle;			// 回転角度
	Quaternion m_qua;		// 惑星のクォータニオン
	Matrix m_mtxWorld;		// ワールドマトリックス
	int m_nIdxModel;		// モデルインデックス
};
#endif