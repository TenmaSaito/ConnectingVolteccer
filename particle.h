//==================================================================================
// 
// パーティクル(クォータニオン仕様)クラスのヘッダーファイル [particle.h]
// Author : TENMA SAITO
// Date   : 2026/8/24
// 
//==================================================================================
#ifndef _PARTICLE_QUATERNION_H_		// インクルードガード
#define _PARTICLE_QUATERNION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"
#include <vector>

//**********************************************************************************
// *** パーティクル(クォータニオン仕様)クラス ***
//**********************************************************************************
class CParticleQuaternion : public CObject
{
public:
	CParticleQuaternion(const int nPriority = DEFAULT_EFFECT_PRIORITY);
	~CParticleQuaternion();

	static CParticleQuaternion *Create(const Vector3 &pos, 
		const Vector2 &baseScale,
		const Color &col,
		const Vector3 &vecQua,
		const float fAngle,
		const int nNumEffect,
		const int nLife);

	HRESULT Init(const Vector3 &pos,
		const Vector2 &baseScale,
		const Color &col,
		const Vector3 &vecQua,
		const float fAngle,
		const int nNumEffect,
		const int nLife);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }

private:

	int m_nIdxTexture = -1;					// テクスチャインデックス
	const Matrix *m_pMtxParent = nullptr;	// 親マトリックスへのポインタ
	Vector3 m_pos = VECTOR3_NULL;		// 発生位置
	Vector3 m_vecQua = VECTOR3_NULL;	// 任意軸
	Vector2 m_baseScale = VECTOR2_NULL;	// 基本サイズ
	float m_fAngle = 0.0f;			// 回転度数
	Quaternion m_qua;			// クォータニオン
	Color m_col = COLOR_NULL;	// 色
	Matrix m_mtxWorld;		// ワールドマトリックス
	int m_nLife = 0;		// 寿命
	int m_nNumEffect = 0;	// 一フレームに発生させるエフェクトの数
};
#endif