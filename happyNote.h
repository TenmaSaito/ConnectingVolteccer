//==================================================================================
// 
// 喜び音符出現クラスのヘッダーファイル [happyNote.h]
// Author : TENMA SAITO
// Date   : 2026/8/31
// 
//==================================================================================
#ifndef _HAPPY_NOTE_H_
#define _HAPPY_NOTE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CObjectBillboard3D;
class CParticleQuaternion;
class CBuilding;

//**********************************************************************************
// *** 喜び音符出現クラス ***
//**********************************************************************************
class CHappyNote : public CObject
{
public:
	CHappyNote();
	~CHappyNote();

	static CHappyNote *Create(const Vector3 &offset, 
		const Vector3 &vecQua,
		const float fAngle,
		const CBuilding *pBuilding);

	HRESULT Init(const Vector3 &offset,
		const Vector3 &vecQua, 
		const float fAngle,
		const CBuilding *pBuilding);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	const CBuilding *m_pBuilding;			// 自身を生成した建物へのポインタ
	CParticleQuaternion *m_pParticle;		// パーティクル生成へのポインタ
};
#endif