//==================================================================================
// 
// 感電エフェクトクラスのヘッダーファイル [shock.h]
// Author : TENMA SAITO
// Date   : 2026/8/30
// 
//==================================================================================
#ifndef _SHOCK_H_		// インクルードガード
#define _SHOCK_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CObject2D;
class CObjectBillboard3D;

//**********************************************************************************
// *** 感電エフェクトクラス ***
//**********************************************************************************
class CShock : public CObject
{
public:
	CShock();
	~CShock();

	static CShock *Create(const Matrix *pMtxParent, const Vector3 &offset);

	HRESULT Init(const Matrix *pMtxParent, const Vector3 &offset);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Set(void);
	bool IsLightning(void) const { return (m_nCounterFrame < m_nTime); }

private:
	CObject2D *m_pWarningEffect;			// 警告用ポリゴンへのポインタ
	CObject2D *m_pScreenEffect;				// スクリーンエフェクト用ポリゴンへのポインタ
	CObjectBillboard3D *m_pPlayerEffect;	// プレイヤーエフェクト用ビルボードへのポインタ
	const Matrix *m_pMtxParent;				// 親マトリックスへのポインタ
	Vector3 m_offset;		// オフセット
	int m_nTime;			// 感電の持続時間
	int m_nCounterFrame;	// フレームカウント
};
#endif