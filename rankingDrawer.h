//==================================================================================
// 
// ランキング描画クラスのヘッダーファイル [rankingDrawer.h]
// Author : TENMA SAITO
// Date   : 2026/9/10
// 
//==================================================================================
#ifndef _RANKING_DRAWER_H_		// インクルードガード
#define _RANKING_DRAWER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;
class CPercent;

//**********************************************************************************
// *** ランキング描画クラス ***
//**********************************************************************************
class CRankingDrawer
{
public:
	CRankingDrawer();
	~CRankingDrawer();

	static CRankingDrawer *Create(const Vector3 &pos,
		const Vector2 &scale,
		const int nRank,
		const float fScore,
		const bool bBlink);

	HRESULT Init(const Vector3 &pos,
		const Vector2 &scale,
		const int nRank,
		const float fScore,
		const bool bBlink);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	std::unique_ptr<CPolygon2D> m_pRank;	// 順位
	CPercent *m_pScore;						// スコア
	int m_nCountFrame;						// フレームカウント
};
#endif