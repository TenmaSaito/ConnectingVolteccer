//==================================================================================
// 
// パーセント表示クラスのヘッダーファイル [percent.h]
// Author : TENMA SAITO
// Date   : 2026/8/31
// 
//==================================================================================
#ifndef _PERCENT_H_
#define _PERCENT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
#define MAX_PERCENT_NUM		(5)		// パーセント表示の最大桁数数 (100.00%)

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CNumber;
class CPolygon2D;

//**********************************************************************************
// *** パーセント表示クラス ***
//**********************************************************************************
class CPercent : public CObject
{
public:
	// クラス生成時の設定
	struct Setting
	{ 
		Vector3 start;		// 表示の開始位置
		Vector2 numScale;	// 数字のサイズ
		Vector2 dotScale;	// 小数点のサイズ
		Vector2 percentScale;		// パーセントのサイズ
		float fValue;				// 値
		int nNumberType;			// 数字のテクスチャタイプ
		UINT nIdxDotTexture;		// 小数点のテクスチャインデックス
		UINT nIdxPercentTexture;	// パーセントのテクスチャインデックス
	};

	CPercent();
	~CPercent();

	static CPercent *Create(const Setting &setting);

	HRESULT Init(const Setting &setting);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetValue(const float fValue);
	float GetValue(void) const { return m_setting.fValue; }

private:
	std::array<std::unique_ptr<CNumber>, MAX_PERCENT_NUM> m_apNumber;		// 各数字へのポインタ
	std::unique_ptr<CPolygon2D> m_pDot;		// 小数点へのポインタ
	std::unique_ptr<CPolygon2D> m_pPercent;	// パーセントへのポインタ
	Setting m_setting;		// 各種設定
};
#endif