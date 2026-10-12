//==================================================================================
// 
// 雷エフェクトクラスのヘッダーファイル [thunderEffect.h]
// Author : TENMA SAITO
// Date   : 2026/6/8
// 
//==================================================================================
#ifndef _THUNDEREFFECT_H_
#define _THUNDEREFFECT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CMeshOrbit3D;

//**********************************************************************************
// *** 雷エフェクトクラス ***
//**********************************************************************************
class CThunderEffect : public CObject
{
public:
	static CThunderEffect *Create(const Vector3 &min,
		const Vector3 &max,
		const Matrix *pMtxParent);

	CThunderEffect();
	~CThunderEffect();

	HRESULT Init(const Vector3 &min,
		const Vector3 &max,
		const Matrix *pMtxParent);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool IsEndAnim(const float fEpsilon = 0.0f) const;

private:
	const Matrix *m_pMtxParent;		// 親マトリックスへのポインタ
	CMeshOrbit3D *m_pOrbit;	// オービットへのポインタ
	Vector3 m_pos;			// 位置
	Vector3 m_min, m_max;		// 座標の範囲
	float m_fMinWidth, m_fMaxWidth;	// オフセット座標の範囲
	Matrix m_mtxWorld;	// ワールドマトリックス
};
#endif