//==================================================================================
// 
// ランキングシーンクラスのヘッダーファイル [ranking.h]
// Author : TENMA SAITO
// Date   : 2026/9/10
// 
//==================================================================================
#ifndef _RANKING_H_		// インクルードガード
#define _RANKING_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlanet;
class CRankingManager;

//**********************************************************************************
// *** タイトルクラス ***
//**********************************************************************************
class CRanking : public CSceneBase<CRanking, CScene::MODE_RANKING>
{
public:
	CRanking();
	~CRanking();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	CPlanet *GetPlanet(void) const { return m_pPlanet; }
	CRankingManager *GetRankingManager(void) const { return m_pRankingManager; }

private:
	void Start(void);

	CPlanet *m_pPlanet;		// 惑星へのポインタ
	CRankingManager *m_pRankingManager;		// ランキングマネージャへのポインタ
};
#endif