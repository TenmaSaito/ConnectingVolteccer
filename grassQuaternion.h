//==================================================================================
// 
// 草(クォータニオン)クラスのヘッダーファイル [grassQuaternion.h]
// Author : TENMA SAITO
// Date   : 2026/9/18
// 
//==================================================================================
#ifndef _GRASS_QUATERNION_H_
#define _GRASS_QUATERNION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "grass.h"

//**********************************************************************************
// *** 草(クォータニオン)クラス ***
//**********************************************************************************
class CGrassQuaternion : public CGrass
{
public:
	CGrassQuaternion();
	~CGrassQuaternion();

	static CGrassQuaternion *Create(const Vector3 &offset,
		const Vector3 &vecQua, 
		const float fAngle,
		const Vector2 &size);

	static CGrassQuaternion *Create(const Vector3 &offset,
		const Quaternion &qua,
		const Vector2 &size);

	HRESULT Init(const Vector3 &offset,
		const Vector3 &vecQua,
		const float fAngle,
		const Vector2 &size);
	HRESULT Init(const Vector3 &offset,
		const Quaternion &qua,
		const Vector2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(const Matrix *pMtxWorld) { m_pMtxParent = pMtxWorld; }

private:
	Vector3 m_offset;		// オフセット
	Quaternion m_qua;		// クォータニオン
	Matrix m_mtxWorld;		// クォータニオンによって生成されたマトリックス
	const Matrix *m_pMtxParent;		// 親マトリックスへのポインタ
};
#endif