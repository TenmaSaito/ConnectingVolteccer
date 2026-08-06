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

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_CONNECT_POLE		(5)		// 接続できる電線の数
#define UTILITYPOLE_PRIORITY	(3)		// 電柱のプライオリティ

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CObjectBillboard;

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

	static CUtilityPole *Create(const D3DXVECTOR3 &pos,
		const D3DXVECTOR3 &vecQua,
		const float fAngle);

	CUtilityPole();
	~CUtilityPole();

	HRESULT Init(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &vecQua, const float fAngle);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool Connect(CUtilityPole *pPole);
	bool Connected(CUtilityPole *pPole);
	void GenerateElectricity(void);
	void SetEnableSelect(const bool bEnable);
	ICON GetIconType(void) const { return m_enableType; }
	bool IsSelected(void) const { return m_bSelected; }
	void SetEnableElectriced(const bool bEnable) { m_bElectriced = bEnable; }
	bool IsElectriced(void) const { return m_bElectriced; }

private:
	CUtilityPole *m_apPole[MAX_CONNECT_POLE];		// 電線でつながっている電柱へのポインタ
	CUtilityPole *m_pConnected;						// 接続してきた電柱へのポインタ
	CObjectBillboard *m_apBillboard[ICON_MAX];		// ビルボードへのポインタ
	int m_nNumConnect;			// 接続されている電柱の数
	bool m_bElectriced;			// 既に電流が流れたかどうか
	bool m_bSelected;			// 選択されているか(プレイヤーの視点の中心に最も近いか)
	ICON m_enableType;			// 有効な種類
};
#endif