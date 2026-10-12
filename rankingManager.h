//==================================================================================
// 
// ランキング管理クラスのヘッダーファイル [rankingManager.h]
// Author : TENMA SAITO
// Date   : 2026/9/10
// 
//==================================================================================
#ifndef _RANKING_MANAGER_H_		// インクルードガード
#define _RANKING_MANAGER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_RANKING_NUM		(5)		// ランキングで表示する数
#define CURRENT_SCORE_PATH	"data/SCORE/current.bin"			// ゲームシーン終了時のスコアを書き出すファイルパス

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CRankingDrawer;
class CObject2D;

//**********************************************************************************
// *** ランキング管理クラス ***
//**********************************************************************************
class CRankingManager : public CObject
{
public:
	CRankingManager();
	~CRankingManager();

	static CRankingManager *Create(const bool bAddCurrent);

	HRESULT Init(const bool bAddCurrent);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	float GetScore(const int nRank) const { return m_aScore[nRank]; }
	int GetCurrentRank(void) const { return m_nCurrentRank; }

private:
	CObject2D *m_pRankingPolygon;						// ランキングの文字表示用ポリゴンへのポインタ
	std::array<float, MAX_RANKING_NUM> m_aScore;		// 各順位のスコア
	std::array<std::unique_ptr<CRankingDrawer>, MAX_RANKING_NUM> m_apRankingDrawer;		// 順位表示クラスへのポインタ
	int m_nCurrentRank;				// 今回の順位
};
#endif