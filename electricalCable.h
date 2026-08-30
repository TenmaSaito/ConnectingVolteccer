//==================================================================================
// 
// 電線クラスのヘッダーファイル [electricalCable.h]
// Author : TENMA SAITO
// Date   : 2026/6/8
// 
//==================================================================================
#ifndef _ELECTRICALCABLE_H_		// インクルードガード
#define _ELECTRICALCABLE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"
#include <array>
#include <span>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define CABLE_PRIORITY			(DEFAULT_OBJ_PRIORITY)		// 電線の優先順位
#define ENABLE_QUATERNION		// クォータニオンの有効化

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CObjectXQuaternion;
class CUtilityPole;
class CPowerPlant;
class CPlanet;

//**********************************************************************************
// *** オブジェクトXクラス ***
//**********************************************************************************
class CElectricalCable : public CObject
{
public:
	static CElectricalCable *Create(CObjectXQuaternion *pStart,
		CObjectXQuaternion *pEnd,
		const CPlanet *pPlanet);

	CElectricalCable();
	~CElectricalCable();

	HRESULT Init(CObjectXQuaternion *pStart,
		CObjectXQuaternion *pEnd,
		const CPlanet *pPlanet);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	void SetColor(const Color &col);
	const Matrix *GetMatrix(void) const { return &m_mtxWorld; }
	std::array<Vector3, DEFAULT_VERTEX_NUM> const GetVtxPosition(void) { return m_aVtxPos; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	int m_nIdxTexture;		// テクスチャインデックス
	Vector3 m_pos;			// 位置
	Vector3 m_vecQua;		// 任意軸
	float m_fAngle;			// 軸角度
	Quaternion m_qua;		// クォータニオン
	Matrix m_mtxWorld;		// ワールドマトリックス
	const Matrix *m_pMtxParent;		// 親のワールドマトリックス
	std::array<Vector3, DEFAULT_VERTEX_NUM> m_aVtxPos;		// 各頂点の座標
	bool m_bElectric;		// プレイヤーが端に着き、電流が流れたか
};
#endif