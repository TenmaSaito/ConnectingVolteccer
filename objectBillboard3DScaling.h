//==================================================================================
// 
// オブジェクトビルボード3D(自動拡縮)クラスのヘッダーファイル [objectBillboard3DScaling.h]
// Author : TENMA SAITO
// Date   : 2026/9/25
// 
//==================================================================================
#ifndef _OBJECTBILLBOARD_3D_SCALING_H_		// インクルードガード
#define _OBJECTBILLBOARD_3D_SCALING_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectBillboard3D.h"

//**********************************************************************************
// *** オブジェクトビルボード(自動拡縮)クラス ***
//**********************************************************************************
class CObjectBillboard3DScaling : public CObjectBillboard3D
{
public:
	static CObjectBillboard3DScaling *Create(const Vector3 &pos = VECTOR3_NULL,
		const Vector2 &size = DEFAULT_OBJBILL_SIZE,
		const float fScalingVal = 0.0f,
		const int nLife = INT_MAX);

	CObjectBillboard3DScaling(const int nPriority = DEFAULT_OBJ_PRIORITY);
	~CObjectBillboard3DScaling();

	HRESULT Init(const Vector3 &pos,
		const Vector2 &size,
		const float fScalingVal,
		const int nLife);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	Vector2 m_defSize;		// 基準サイズ
	float m_fScalingVal;	// 一フレームに加算されるスケーリング倍率
	int m_nDefLife;			// 基準体力
	int m_nCounterFrame;	// フレームカウンター
};
#endif