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
class CUtilityPole;

//**********************************************************************************
// *** オブジェクトXクラス ***
//**********************************************************************************
class CElectricalCable : public CObject
{
public:
	static CElectricalCable *Create(const CUtilityPole *pStart, 
		const CUtilityPole *pEnd);

	CElectricalCable();
	~CElectricalCable();

	HRESULT Init(const CUtilityPole *pStart, 
		const CUtilityPole *pEnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetParent(const D3DXMATRIX *pMtxParent) { m_pMtxParent = pMtxParent; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	int m_nIdxTexture;			// テクスチャインデックス
	CUtilityPole *m_pStart;		// 電線の始点電柱
	CUtilityPole *m_pEnd;		// 電線の終点電柱
	D3DXVECTOR3 m_pos;			// 位置
#ifndef ENABLE_QUATERNION
	D3DXVECTOR3 m_rot;			// 角度
#else
	D3DXVECTOR3 m_vecQua;		// 任意軸
	float m_fAngle;				// 軸角度
	D3DXQUATERNION m_qua;		// クォータニオン
#endif
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	const D3DXMATRIX *m_pMtxParent;		// 親のワールドマトリックス
};
#endif