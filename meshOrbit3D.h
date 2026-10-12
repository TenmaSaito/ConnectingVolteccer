//==================================================================================
// 
// メッシュオービット3Dクラスのヘッダーファイル [meshOrbit3D.h]
// Author : TENMA SAITO
// Date   : 2026/5/30
// 
//==================================================================================
#ifndef _MESHORBIT3D_H_
#define _MESHORBIT3D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "mesh3D.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_ORBIT_NUM		(30)		// 基本のオービット数

//**********************************************************************************
// *** メッシュオービット3Dクラス ***
//**********************************************************************************
class CMeshOrbit3D : public CMesh3D
{
public:
	static CMeshOrbit3D *Create(const Matrix *pMtxParent,
		const Vector3 &offset1 = VECTOR3_NULL,
		const Vector3 &offset2 = VECTOR3_NULL,
		const int nNumOrbit = DEFAULT_ORBIT_NUM);

	CMeshOrbit3D(const int nPriority = DEFAULT_OBJ_PRIORITY);
	~CMeshOrbit3D();

	HRESULT Init(const Vector3 &offset1,
		const Vector3 &offset2,
		const int nNumOrbit);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetMtxParent(const Matrix *pMtxParent);
	void SetOffset(const Vector3 &offset1,
		const Vector3 &offset2);
	bool IsFinish(const float fEpsilon = 0.0f) const;

private:
	const Matrix *m_pMtxParent;		// 親マトリックスへのポインタ
	Vector3 m_aOffset[2];	// オービットの各オフセット
	Vector3 *m_pPosition;	// オービットの頂点座標へのポインタ
};
#endif