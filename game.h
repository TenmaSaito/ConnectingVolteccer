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
#include <memory>

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlayer;
class CPlanet;
class CTimer;
class CCombo;
class CStopWatch;

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
	bool GetEnableEdit(void) { return m_bEdit; }
	constexpr CPlayer *GetPlayer(void) const { return m_pPlayer; }
	constexpr CPlanet *GetPlanet(void) const { return m_pPlanet; }
	constexpr CTimer *GetTimer(void) const { return m_pTimer; }
	constexpr CCombo *GetCombo(void) const { return m_pCombo; }

	static constexpr MODE GetMyMode(void) { return CScene::MODE_GAME; }

private:
	void Start(void);
	void UpdateEdit(void);
	void MapEdit(void);

	std::unique_ptr<CStopWatch> m_pStopWatch;		// ストップウォッチへのポインタ
	CPlayer *m_pPlayer;		// プレイヤーへのポインタ
	CPlanet *m_pPlanet;		// 惑星へのポインタ
	CTimer *m_pTimer;		// タイマーへのポインタ
	CCombo *m_pCombo;		// コンボ表示へのポインタ
	bool m_bEdit;			// エディットモードフラグ
	bool m_bPause;			// ポーズ状態
	int m_nCounterFrame;	// フレームカウント
};
#endif