//==================================================================================
// 
// メッシュシリンダークラスのヘッダーファイル [meshCylinder.h]
// Author : TENMA SAITO
// Date   : 2026/6/10
// 
//==================================================================================
#ifndef _MESHCYLINDER_H_
#define _MESHCYLINDER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "mesh3D.h"

//**********************************************************************************
// *** メッシュシリンダークラス ***
//**********************************************************************************
class CMeshCylinder : public CMesh3D
{
public:
	CMeshCylinder();
	~CMeshCylinder();

	static CMeshCylinder *Create(const int nNumAngle,
		const int nNumVertical,
		const Vector3 &pos,
		const Vector3 &rot,
		const float fRadius,
		const float fHeight);

	HRESULT Init(const int nNumAngle,
		const int nNumVertical,
		const Vector3 &pos,
		const Vector3 &rot,
		const float fRadius,
		const float fHeight);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }

private:
	Vector3 m_pos;		// 座標
	Vector3 m_rot;		// 角度
	float m_fRadius;	// 半径
	float m_fHeight;	// 高さ
	int m_nNumAngle;	// 角の数
	int m_nNumVertical;	// 縦の分割数
	Matrix m_mtxWorld;	// ワールドマトリックス
	const Matrix *m_pMtxParent;		// 親マトリックスへのポインタ
};
#endif
