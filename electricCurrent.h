//==================================================================================
// 
// 電流クラスのヘッダーファイル [electricCurrent.h]
// Author : TENMA SAITO
// Date   : 2026/6/9
// 
//==================================================================================
#ifndef _ELECTRICCURRENT_H_		// インクルードガード
#define _ELECTRICCURRENT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_ELECTRIC_TIME		(60)		// 電流が電柱間を流れ終わる時間

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CUtilityPole;
class CThunderEffect;

//**********************************************************************************
// *** 電流クラス ***
//**********************************************************************************
class CElectricCurrent : public CObject
{
public:
	static CElectricCurrent *Create(CUtilityPole *pStart, 
		CUtilityPole *pEnd,
		const float fTotalTime = DEFAULT_ELECTRIC_TIME);

	CElectricCurrent();
	~CElectricCurrent();

	HRESULT Init(CUtilityPole *pStart, 
		CUtilityPole *pEnd,
		const float fTotalTime);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }

private:
	CUtilityPole *m_pStart;		// 始まりの電柱
	CUtilityPole *m_pEnd;		// 終わりの電柱
	CThunderEffect *m_pThunder;		// 生成した電流エフェクトへのポインタ
	Matrix m_mtxWorld;			// ワールドマトリックス
	const Matrix *m_pMtxParent;	// 親マトリックスへのポインタ
	Vector3 m_pos;	// 位置
	Vector3 m_rot;	// 角度
	Quaternion m_qua;		// クォータニオン
	float m_fTime;		// 線形補間用変数
	float m_fTotalTime;	// 線形補間完了にかかる時間
};
#endif