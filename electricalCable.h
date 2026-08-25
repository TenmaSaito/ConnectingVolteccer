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

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define ENABLE_QUATERNION		// クォータニオンの有効化

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CObjectXQuaternion;
class CUtilityPole;
class CPowerPlant;

//**********************************************************************************
// *** オブジェクトXクラス ***
//**********************************************************************************
class CElectricalCable : public CObject
{
public:
	static CElectricalCable *Create(const CObjectXQuaternion *pStart,
		const CObjectXQuaternion *pEnd);

	CElectricalCable();
	~CElectricalCable();

	HRESULT Init(const CObjectXQuaternion *pStart,
		const CObjectXQuaternion *pEnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	int m_nIdxTexture;		// テクスチャインデックス
	Vector3 m_pos;			// 位置
	Vector3 m_vecQua;		// 任意軸
	float m_fAngle;			// 軸角度
	Quaternion m_qua;		// クォータニオン
	Matrix m_mtxWorld;		// ワールドマトリックス
	const Matrix *m_pMtxParent;		// 親のワールドマトリックス
	bool m_bElectric;		// プレイヤーが端に着き、電流が流れたか
};
#endif