//==================================================================================
// 
// パーティクル(クォータニオン仕様)の生成クラスのヘッダーファイル [particleQuaternionBuilder.h]
// Author : TENMA SAITO
// Date   : 2026/8/27
// 
//==================================================================================
#ifndef _PARTICLE_QUATERNION_BUILDER_H_		// インクルードガード
#define _PARTICLE_QUATERNION_BUILDER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "particleQuaternion.h"

//**********************************************************************************
// *** パーティクル(クォータニオン仕様)生成クラス ***
//**********************************************************************************
class CParticleQuaternionBuilder
{
public:
	CParticleQuaternionBuilder();
	~CParticleQuaternionBuilder();

	CParticleQuaternionBuilder &SetPosition(const Vector3 &pos);
	CParticleQuaternionBuilder &SetVecQua(const Vector3 &vecQua);
	CParticleQuaternionBuilder &SetAngle(const float fAngle);
	CParticleQuaternionBuilder &SetMove(const Vector3 &move);
	CParticleQuaternionBuilder &SetScale(const Vector2 &scale);
	CParticleQuaternionBuilder &SetColor(const Color &color);
	CParticleQuaternionBuilder &SetNumEffectFrame(const int nNumEffectFrame);
	CParticleQuaternionBuilder &SetLife(const int nLife);
	CParticleQuaternionBuilder &SetPositionVariation(const Vector3 &posVariation);
	CParticleQuaternionBuilder &SetMoveVariation(const Vector3 &moveVariation);
	CParticleQuaternionBuilder &SetScaleVariation(const Vector2 &scaleVariation);
	CParticleQuaternion::Setting Build(void) const { return m_setting; }

private:
	CParticleQuaternion::Setting m_setting;		// パーティクル生成時の設定
};
#endif