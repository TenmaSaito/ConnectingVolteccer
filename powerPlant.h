//==================================================================================
// 
// 発電所クラスのヘッダーファイル [powerPlant.h]
// Author : TENMA SAITO
// Date   : 2026/8/22
// 
//==================================================================================
#ifndef _POWER_PLANT_H_		// インクルードガード
#define _POWER_PLANT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectXQuaternion.h"
#include <vector>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define POWERPLANT_PRIORITY		(DEFAULT_OBJ_PRIORITY)		// 発電所のプライオリティ

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CUtilityPole;

//**********************************************************************************
// *** 発電所クラス ***
//**********************************************************************************
class CPowerPlant : public CObjectXQuaternion
{
public:
	CPowerPlant(const int nPriority = POWERPLANT_PRIORITY);
	~CPowerPlant();

	static CPowerPlant *Create(const Vector3 &pos,
		const Vector3 &vecQua,
		const float fAngle);
	static CPowerPlant *Create(const Vector3 &pos);

	HRESULT Init(const Vector3 &pos,
		const Vector3 &vecQua,
		const float fAngle);
	HRESULT Init(const Vector3 &pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool Connect(CUtilityPole *pPole);
	const std::vector<CUtilityPole*> *GetConnectPoles(void) const { return &m_vpPole; }

private:
	std::vector<CUtilityPole*> m_vpPole;
};
#endif