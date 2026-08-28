//==================================================================================
// 
// 光の柱クラスのヘッダーファイル [lightingPillar.h]
// Author : TENMA SAITO
// Date   : 2026/6/2
// 
//==================================================================================
#ifndef _LIGHTING_PILLAR_H_		// インクルードガード
#define _LIGHTING_PILLAR_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectBillboard3D.h"

//**********************************************************************************
// *** 光の柱クラス ***
//**********************************************************************************
class CLightingPillar : public CObjectBillboard3D
{
public:
	CLightingPillar();
	~CLightingPillar();

	static CLightingPillar *Create(const Vector3 &pos, 
		const Vector2 &scale,
		const Color &color);

	HRESULT Init(const Vector3 &pos, const Vector2 &scale, const Color &color);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void StartScaleUp(void) { m_bStartScaleUp = true; }

private:
	float m_fEndHeight;			// 最終的な縦幅
	float m_fCurrentHeight;		// 現在の縦幅
	bool m_bStartScaleUp;		// スケールの増加をするか
};
#endif