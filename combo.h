//==================================================================================
// 
// コンボ表示クラスのヘッダーファイル [combo.h]
// Author : TENMA SAITO
// Date   : 2026/8/20
// 
//==================================================================================
#ifndef _COMBO_H_
#define _COMBO_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"
#include <memory>
#include <array>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_COMBO_NUM		(2)		// コンボの最大桁数数

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CNumber;
class CPolygon2D;

//**********************************************************************************
// *** コンボ表示クラス ***
//**********************************************************************************
class CCombo : public CObject
{
public:
	CCombo();
	~CCombo();

	static CCombo *Create(const Vector3 &pos, const Vector3 &rot);

	HRESULT Init(const Vector3 &pos, const Vector3 &rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetCombo(const int nValue);
	void AddCombo(const int nValue);
	void Finish(void);
	void Withdrawal(void);
	void ResetCombo(void);
	int GetCombo(void) const { return m_nCombo; }
	bool GetDisp(void) const { return m_bDisp; }
	bool GetContinuing(void) const { return m_bContinuing; }

private:
	void UpdateAlpha(void);
	void UpdateScale(void);

	std::array<std::unique_ptr<CNumber>, MAX_COMBO_NUM> m_apNumber;		// 数値オブジェクトへのポインタ
	std::unique_ptr<CPolygon2D> m_pCombo;			// コンボ用ポリゴン
	std::unique_ptr<CPolygon2D> m_pGauge;			// ゲージ用ポリゴン
	Vector3 m_pos = VECTOR3_NULL;		// 表示する中心座標
	Vector3 m_rot = VECTOR3_NULL;		// 角度
	float m_fTimeCatmullRom = 0.0f;		// スプライン補間用変数
	float m_fTimeAlphaLerp = 0.0f;	// 透明度の線形補間用変数
	float m_fTimeGaugeLerp = 0.0f;	// ゲージサイズの線形補間用変数
	int m_nNumNumber = 0;		// 現在の描画する数値オブジェクトの数
	int m_nCombo = 0;			// 現在のコンボ数
	int m_nDispLife = 0;		// 描画する時間
	bool m_bDisp = false;		// 描画状態
	bool m_bContinuing = false;	// コンボが続いているか
};
#endif