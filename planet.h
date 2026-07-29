//==================================================================================
// 
// 惑星クラスのヘッダーファイル [planet.h]
// Author : TENMA SAITO
// Date   : 2026/6/23
// 
//==================================================================================
#ifndef _PLANET_H_
#define _PLANET_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CLasso;

//**********************************************************************************
// *** 惑星クラス ***
//**********************************************************************************
class CPlanet : public CObject
{
public:
	static CPlanet *Create(void);

	CPlanet();
	~CPlanet();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetFocusLasso(const CLasso *pLasso);
	float GetAngle(void) const { return m_fAngle; }
	const D3DXVECTOR3 *GetVecQua(void) const { return &m_vecQua; }
	const D3DXMATRIX *GetMatrix(void) const { return &m_mtxWorld; }
	void AddQuaternion(const D3DXQUATERNION &quaAdd) { m_qua = m_qua * quaAdd; }
	const D3DXQUATERNION *GetQuaternion(void) const { return &m_qua; }
	const D3DXVECTOR3 *GetPosition(void) const { return &m_pos; }

private:
	D3DXVECTOR3 m_pos;			// 惑星の位置
	D3DXVECTOR3 m_vecQua;		// 任意軸
	D3DXVECTOR3 m_vecQuaOld;	// 過去の任意軸
	D3DXVECTOR3 m_vecQuaDest;	// 遷移予定の任意軸
	float m_fAngle;				// 回転角度
	D3DXQUATERNION m_qua;		// 惑星のクォータニオン
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	int m_nIdxModel;			// モデルインデックス
	const CLasso *m_pFocusLasso;		// フォーカスする投げ縄
};
#endif