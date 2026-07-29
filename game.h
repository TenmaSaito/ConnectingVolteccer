//==================================================================================
// 
// ゲームクラスのヘッダーファイル [game.h]
// Author : TENMA SAITO
// Date   : 2026/7/1
// 
//==================================================================================
#ifndef _GAME_H_		// インクルードガード
#define _GAME_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlayer;
class CPlanet;
class CTimer;

//**********************************************************************************
// *** タイトルクラス ***
//**********************************************************************************
class CGame : public CScene
{
public:
	CGame();
	~CGame();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetEnablePause(const bool bEnable) { m_bPause = bEnable; }
	CPlayer *GetPlayer(void) const { return m_pPlayer; }
	CPlanet *GetPlanet(void) const { return m_pPlanet; }

private:
	void Start(void);

	CPlayer *m_pPlayer;		// プレイヤーへのポインタ
	CPlanet *m_pPlanet;		// 惑星へのポインタ
	CTimer *m_pTimer;		// タイマーへのポインタ
	bool m_bPause;			// ポーズ状態
	int m_nCounterFrame;	// フレームカウント
};

#endif