//==================================================================================
// 
// スパークエフェクトクラスのヘッダーファイル [sparkEffect.h]
// Author : TENMA SAITO
// Date   : 2026/6/8
// 
//==================================================================================
#ifndef _SPARKEFFECT_H_
#define _SPARKEFFECT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_SPARK_MIN		(D3DXVECTOR3(0.1f, 0.0f, 0.0f))		// デフォの幅の最小値
#define DEFAULT_SPARK_MAX		(D3DXVECTOR3(15.0f, 0.0f, 0.0f))	// デフォの幅の最大値

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CMeshOrbit3D;

//**********************************************************************************
// *** 雷エフェクトクラス ***
//**********************************************************************************
class CSparkEffect : public CObject
{
public:
	static CSparkEffect *Create(const D3DXVECTOR3 &pos,
		const D3DXVECTOR3 &rot,
		const float fSpeed,
		const int nLife,
		const D3DXVECTOR3 &min = DEFAULT_SPARK_MIN,
		const D3DXVECTOR3 &max = DEFAULT_SPARK_MAX);

	CSparkEffect();
	~CSparkEffect();

	HRESULT Init(const D3DXVECTOR3 &pos,
		const D3DXVECTOR3 &rot,
		const float fSpeed,
		const int nLife,
		const D3DXVECTOR3 &min,
		const D3DXVECTOR3 &max);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	CMeshOrbit3D *m_pOrbit;	// オービットへのポインタ
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_posParent;	// 親の位置
	D3DXVECTOR3 m_rotParent;	// 親の角度
	float m_fSpeed;			// 移動速度
	D3DXVECTOR3 m_min, m_max;		// 座標の範囲
	float m_fMinWidth, m_fMaxWidth;	// オフセット座標の範囲
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	int m_nLife;			// 寿命
};
#endif