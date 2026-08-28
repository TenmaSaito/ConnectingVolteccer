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
#include <memory>

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CUtilityPole;
class CBillboard3D;
class CLightingPillar;

// 各モデルには、電気のついていない窓とついている窓のポリゴンがそれぞれ別のマテリアルで設定されている
// その為、基本は一番最後のマテリアルは描画せず、電気がついた場合のみ描画するようにすること

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
		const Vector3 &position,
		const Vector3 &vecQua,
		const float fAngle);

	static CBuilding *Create(const TYPE type,
		const Vector3 &position);

	CBuilding(const TYPE type, const int nPriority = DEFAULT_OBJ_PRIORITY);
	~CBuilding();

	HRESULT Init(const Vector3 &position,
		const Vector3 &vecQua,
		const float fAngle);
	HRESULT Init(const Vector3 &position);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	TYPE GetType(void) const { return m_buildingType; }

private:
	void FindUtilityPole(void);

	const CUtilityPole *m_pNearPole;		// 最も近い電柱
	CLightingPillar *m_pPillar;		// 光の柱へのポインタ
	bool m_bFound;				// 最も近い電柱の検索済みフラグ
	bool m_bLighting;			// 電流が流れたか
	float m_fLerp;				// 線形補間用変数
	float m_fValue;				// 補間時の変化量
	TYPE m_buildingType;		// 建物の種類
};
#endif