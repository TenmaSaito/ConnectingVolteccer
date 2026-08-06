//==================================================================================
// 
// 建造物クラスのヘッダーファイル [building.h]
// Author : TENMA SAITO
// Date   : 2026/7/24
// 
//==================================================================================
#ifndef _BUILDING_H_
#define _BUILDING_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectXQuaternion.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CUtilityPole;

//**********************************************************************************
// *** 建造物クラス ***
//**********************************************************************************
class CBuilding : public CObjectXQuaternion
{
public:
	// 建物の種類
	typedef enum
	{
		TYPE_0 = 0,		// 建物0
		TYPE_1,			// 建物1
		TYPE_2,			// 建物2
		TYPE_3,			// 建物3
		TYPE_MAX
	} TYPE;

	static CBuilding *Create(const TYPE type,
		const D3DXVECTOR3 &position = VECTOR3_NULL,
		const D3DXVECTOR3 &vecQua = VECTOR3_NULL,
		const float fAngle = 0.0f);

	static CBuilding *Create(const TYPE type,
		const D3DXVECTOR3 &position = VECTOR3_NULL);

	CBuilding(const TYPE type, const int nPriority = DEFAULT_OBJ_PRIORITY);
	~CBuilding();

	HRESULT Init(const D3DXVECTOR3 &position,
		const D3DXVECTOR3 &vecQua,
		const float fAngle);
	HRESULT Init(const D3DXVECTOR3 &position);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	TYPE GetType(void) const { return m_buildingType; }

private:
	void FindUtilityPole(void);

	static const char *m_apBulidingPath[TYPE_MAX];		// 各建物のパス
	const CUtilityPole *m_pNearPole;		// 最も近い電柱
	bool m_bHitByPlayerCamRay;				// プレイヤーカメラとのレイ判定
	bool m_bFind;				// 検索
	float m_fLerp;				// 線形補間用変数
	float m_fValue;				// 補間時の変化量
	TYPE m_buildingType;		// 建物の種類
};
#endif