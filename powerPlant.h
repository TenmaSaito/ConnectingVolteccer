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

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define POWERPLANT_PRIORITY		(DEFAULT_OBJ_PRIORITY)		// 発電所のプライオリティ

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CUtilityPole;
class CElectricalCable;
class CObjectBillboard3D;
class CPlayer;

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
		const float fAngle,
		const int nID);
	static CPowerPlant *Create(const Vector3 &pos, const int nID);

	HRESULT Init(const Vector3 &pos,
		const Vector3 &vecQua,
		const float fAngle,
		const int nID);
	HRESULT Init(const Vector3 &pos, const int nID);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool Connect(CUtilityPole *pPole);
	void InvokeElectric(void);
	void RemoveConnected(void);
	std::span<CUtilityPole *const> GetConnectPole(void) const { return std::span(m_vpPole); }
	int GetID(void) const { return m_nID; }
	void BindPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }

private:
	std::vector<CUtilityPole*> m_vpPole;	// 今繋がっている電柱へのポインタ
	CUtilityPole *m_pCurrentPole;			// 今回のコンボで繋げている電柱へのポインタ
	CElectricalCable *m_pCurrentCable;		// 今回のコンボで繋げてた電線へのポインタ
	CPlayer *m_pPlayer;			// プレイヤーへのポインタ
	int m_nID;		// 生成されたID
	bool m_bHitByPlayerCamRay;	// プレイヤーカメラとプレイヤの間にいるか
	bool m_bDisp;				// 描画フラグ
};
#endif