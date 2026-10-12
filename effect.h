//==================================================================================
// 
// エフェクトクラスのヘッダーファイル [effect.h]
// Author : TENMA SAITO
// Date   : 2026/5/20
// 
//==================================================================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectBillboard3D.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_EFFECT_RADIUS	(20.0f)					// エフェクトの基本半径
#define DEFAULT_EFFECT_LIFE		(1)						// エフェクトの基本寿命
#define DEFAULT_EFFECT_MOVE		(VECTOR3_NULL)		// エフェクトの基本移動量
#define DEFAULT_EFFECT_GRAVITY	(0.0f)				// エフェクトの基本重力

//**********************************************************************************
// *** エフェクトクラス ***
//**********************************************************************************
class CEffect : public CObjectBillboard3D
{
public:
	CEffect(const int nPriority = DEFAULT_EFFECT_PRIORITY);
	~CEffect();

	static HRESULT Load(void);
	static CEffect *Create(const Vector3 &pos,
		const int nLife = DEFAULT_EFFECT_LIFE,
		const float fRadius = DEFAULT_EFFECT_RADIUS,
		const Vector3 &move = DEFAULT_EFFECT_MOVE,
		const float fGravity = DEFAULT_EFFECT_GRAVITY);

	HRESULT Init(const Vector3 &pos, 
		const int nLife, 
		const float fRadius,
		const Vector3 &move,
		const float fGravity);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	static int m_nIdxTexture;	// テクスチャインデックス
	Vector3 m_move;			// 移動量
	int m_nLife;				// 体力
	float m_fRadius;			// 半径
	float m_fDecreaseRadius;	// 半径の減少割合
	float m_fGravity;			// 重力の強さ
};
#endif