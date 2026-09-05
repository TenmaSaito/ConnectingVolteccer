//==================================================================================
// 
// 電柱クラスのヘッダーファイル [utilityPole.h]
// Author : TENMA SAITO
// Date   : 2026/6/8
// 
//==================================================================================
#ifndef _UTILITYPOLE_H_		// インクルードガード
#define _UTILITYPOLE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectXQuaternion.h"
#include <variant>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_CONNECT_POLE		(5)		// 接続できる電線の数
#define UTILITYPOLE_PRIORITY	(DEFAULT_OBJ_PRIORITY)		// 電柱のプライオリティ

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CObjectBillboard3D;
class CPowerPlant;
class CPlayer;
class CElectricalCable;

//**********************************************************************************
// *** 電柱クラス ***
//**********************************************************************************
class CUtilityPole : public CObjectXQuaternion
{
public:
	// ビルボードの種類
	typedef enum
	{
		ICON_CAN = 0,	// 可能アイコン
		ICON_CANT,		// 不可能アイコン
		ICON_AIMING,	// ターゲットアイコン
		ICON_MAX
	} ICON;

	// 高さの種類
	typedef enum
	{
		HEIGHT_0 = 0,
		HEIGHT_1,
		HEIGHT_2,
		HEIGHT_MAX
	} HEIGHT;

	// std::variantに含まれている変数の種類
	typedef enum
	{
		CPOWERPLANT_PTR = 0,	// 発電所へのポインタ
		CUTILITYPOLE_PTR,		// 電柱へのポインタ
		PTRTYPE_MAX
	} PTRTYPE;

	static CUtilityPole *Create(const Vector3 &pos,
		const Vector3 &vecQua,
		const float fAngle,
		const int nID);

	CUtilityPole();
	~CUtilityPole();

	HRESULT Init(const Vector3 &pos,
		const Vector3 &vecQua,
		const float fAngle,
		const int nID);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }
	bool Connect(CUtilityPole *pPole);
	bool Connected(CUtilityPole *pPole);
	bool Connected(CPowerPlant *pPowerPlant);
	void GenerateElectricity(void);
	void RemoveConnected(void);
	bool CanFocus(const CPlayer *pPlayer);
	void SetEnableSelect(const bool bEnable);
	ICON GetIconType(void) const { return m_enableType; }
	bool IsSelected(void) const { return m_bSelected; }
	void SetEnableElectriced(const bool bEnable) { m_bElectriced = bEnable; }
	bool IsElectriced(void) const { return m_bElectriced; }
	int GetID(void) const { return m_nID; }

private:
	CUtilityPole *m_pConnect;						// 電線でつながっている電柱へのポインタ
	std::variant<CPowerPlant*, CUtilityPole*> m_pConnected;		// 繋げてきたオブジェクトへのポインタ
	CPlayer *m_pPlayer;			// プレイヤーへのポインタ
	CElectricalCable *m_pCurrentCable;					// 繋げた電線へのポインタ
	CObjectBillboard3D *m_apBillboard[ICON_MAX];		// ビルボードへのポインタ
	int m_nNumConnect;			// 接続されている電柱の数
	bool m_bElectriced;			// 既に電流が流れたかどうか
	bool m_bSelected;			// 選択されているか(プレイヤーの視点の中心に最も近いか)
	ICON m_enableType;			// 有効な種類
	int m_nID;			// 生成されたID
};
#endif