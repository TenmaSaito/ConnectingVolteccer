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
class CConnectingEvaluate;
class CStopWatch;
class CThunderCamera;

//**********************************************************************************
// *** タイトルクラス ***
//**********************************************************************************
class CGame : public CSceneBase<CGame, CScene::MODE_GAME>
{
public:
	CGame();
	~CGame();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void AddLightingHouse(void) { m_nCurrentConnectLighting++; }
	void ResetCurrentLightingHouseNum(void);
	void SetEnablePause(const bool bEnable) { m_bPause = bEnable; }
	bool GetEnableEdit(void) { return m_bEdit; }
	constexpr CPlayer *GetPlayer(void) const { return m_pPlayer; }
	constexpr CPlanet *GetPlanet(void) const { return m_pPlanet; }
	constexpr CTimer *GetTimer(void) const { return m_pTimer; }
	constexpr CCombo *GetCombo(void) const { return m_pCombo; }
	constexpr CConnectingEvaluate *GetConnectingEvaluate(void) const { return m_pEvaluate; }
	constexpr CThunderCamera *GetThunderCamera(void) const { return m_pThunderCam; }

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
	CThunderCamera *m_pThunderCam;			// 電流表示用カメラへのポインタ
	CConnectingEvaluate *m_pEvaluate;		// 接続時評価用クラスへのポインタ
	bool m_bEdit;			// エディットモードフラグ
	bool m_bPause;			// ポーズ状態
	int m_nNumLightingHouse;			// 電線の接続で電気のついた家の総数
	int m_nCurrentConnectLighting;		// 今回の電線の接続で電気のついた家の数
	bool m_bCreateConnectEffect;		// 電気のついた家の数で表示が変わる演出を既に生成したか
	int m_nCounterFrame;	// フレームカウント
};
#endif