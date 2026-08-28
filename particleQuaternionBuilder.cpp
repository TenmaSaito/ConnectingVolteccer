//==================================================================================
// 
// パーティクル(クォータニオン仕様)の生成クラスのソースファイル [particleQuaternionBuilder.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/27
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "particleQuaternionBuilder.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CParticleQuaternionBuilder::CParticleQuaternionBuilder()
{ // メンバ変数のクリア
	ZeroMemory(&m_setting, sizeof(m_setting));
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CParticleQuaternionBuilder::~CParticleQuaternionBuilder()
{
}

//==================================================================================
// --- 位置の設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetPosition(const Vector3 &pos)
{
	m_setting.pos = pos;
	return *this;
}

//==================================================================================
// --- 任意軸の設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetVecQua(const Vector3 &vecQua)
{
	m_setting.vecQua = vecQua;
	return *this;
}

//==================================================================================
// --- 回転度数の設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetAngle(const float fAngle)
{
	m_setting.fAngle = fAngle;
	return *this;
}

//==================================================================================
// --- 移動量の設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetMove(const Vector3 &move)
{
	m_setting.move = move;
	return *this;
}

//==================================================================================
// --- スケールの設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetScale(const Vector2 &scale)
{
	m_setting.scale = scale;
	return *this;
}

//==================================================================================
// --- 色の設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetColor(const Color &color)
{
	m_setting.color = color;
	return *this;
}

//==================================================================================
// --- 1フレーム当たりの発生数の設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetNumEffectFrame(const int nNumEffectFrame)
{
	m_setting.nNumEffectFrame = nNumEffectFrame;
	return *this;
}

//==================================================================================
// --- 持続時間の設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetLife(const int nLife)
{
	m_setting.nLife = nLife;
	return *this;
}

//==================================================================================
// --- 位置のぶれの設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetPositionVariation(const Vector3 &posVariation)
{
	m_setting.posVariation = posVariation;
	return *this;
}

//==================================================================================
// --- 移動量のぶれの設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetMoveVariation(const Vector3 &moveVariation)
{
	m_setting.moveVariation = moveVariation;
	return *this;
}

//==================================================================================
// --- スケールのぶれの設定 ---
//==================================================================================
CParticleQuaternionBuilder &CParticleQuaternionBuilder::SetScaleVariation(const Vector2 &scaleVariation)
{
	m_setting.scaleVariation = scaleVariation;
	return *this;
}