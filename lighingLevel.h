//==================================================================================
// 
// 惑星の明るさ表示UIクラスのヘッダーファイル [lightingLevel.h]
// Author : TENMA SAITO
// Date   : 2026/8/28
// 
//==================================================================================
#ifndef _LIGHTING_LEVEL_H_		// インクルードガード
#define _LIGHTING_LEVEL_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** 惑星の明るさ表示UIクラス ***
//**********************************************************************************
class CLightingLevel : public CObject
{
public:
	// テクスチャの種類
	typedef enum
	{
		TEX_DETACHED_HOUSE = 0,			// 一軒家
		TEX_DETACHED_HOUSE_LIGHTING,	// 一軒家 (電気付き)
		TEX_APARTMENT,					// マンション
		TEX_APARTMENT_LIGHTING,			// マンション (電気付き)
		TEX_MAX
	} TEX;

	// UIで表示されるポリゴンの情報
	struct PolyInfo
	{
		std::unique_ptr<CPolygon2D> pPoly;		// ポリゴンへのポインタ
		bool bLighted;		// 光ったテクスチャになったか
		TEX tex;			// テクスチャの種類
	};

	static CLightingLevel *Create(const Vector3 &pos, const float fRadius);

	CLightingLevel();
	~CLightingLevel();

	HRESULT Init(const Vector3 &pos, const float fRadius);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	float GetRatioLighting(void) const { return m_fRatioLighting; }

private:
	std::vector<PolyInfo> m_vPolyInfo;		// ポリゴンの情報の配列
	float m_fRatioLighting;		// 現在の明るさの割合
};
#endif