//==================================================================================
// 
// パーティクル(クォータニオン仕様)クラスのヘッダーファイル [particleQuaternion.h]
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

//**********************************************************************************
// *** パーティクル(クォータニオン仕様)クラス ***
//**********************************************************************************
class CParticleQuaternion : public CObject
{
public:
	// パーティクル生成の設定情報
	struct Setting
	{
		Vector3 pos;		// 発生位置
		Vector3 vecQua;		// 任意軸
		float fAngle;		// 回転度数
		Vector3 move;		// 加速度
		Vector2 scale;		// サイズ
		Color color;		// パーティクルの色
		int nNumEffectFrame;	// 一フレームに出現させるエフェクトの数
		int nLife;			// 持続フレーム数
		bool bEnableAlphaBlending;		// αブレンディングフラグ

		// ExSettings
		Vector3 posVariation;		// 発生位置のぶれの範囲 (X >= 0, Y >= 0, Z >= 0)
		Vector3 moveVariation;		// 移動量のぶれの範囲 (X >= 0, Y >= 0, Z >= 0)
		Vector2 scaleVariation;		// サイズのぶれの範囲 (X >= 0, Y >= 0)
		int nNumEffectVariation;	// 発生するエフェクトのぶれの範囲 (X >= 0)
		int nEffectLifeMax;			// エフェクトの体力の最大値 (X < nLife)
		int nPercent;				// エフェクトの発生する確率 (X > 0 && X <= 100)
	};

	CParticleQuaternion(const int nPriority = DEFAULT_EFFECT_PRIORITY);
	~CParticleQuaternion();

	static CParticleQuaternion *Create(const Setting &setting);

	HRESULT Init(const Setting &setting);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetEnable(const bool bEnable) { m_bEnable = bEnable; }

private:
	int m_nIdxTexture = -1;					// テクスチャインデックス
	const Matrix *m_pMtxParent = nullptr;	// 親マトリックスへのポインタ
	Setting m_setting = {};				// 設定
	Quaternion m_qua;		// クォータニオン
	Matrix m_mtxWorld;		// ワールドマトリックス
	bool m_bEnable = false;	// 一時的に生成をやめるか
};
#endif