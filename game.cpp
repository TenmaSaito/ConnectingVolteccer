//==================================================================================
// 
// ゲームクラスのソースファイル [game.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game.h"
#include "manager.h"
#include "input.h"
#include "joypad.h"
#include "debugproc.h"
#include "player.h"
#include "planet.h"
#include "camera.h"
#include "map.h"
#include "timer.h"
#include "delegate_t.h"
#include <string>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define LOAD_MAP_PATH		"data/map2.txt"		// 読み込むマップ情報へのパス

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CGame::CGame() : CScene(MODE_GAME)
{ // 親クラスのコンストラクタ呼び出し
	// メンバ変数のクリア
	m_pPlayer = nullptr;
	m_pPlanet = nullptr;
	m_pTimer = nullptr;
	m_bPause = false;
	m_nCounterFrame = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CGame::~CGame()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CGame::Init(void)
{
	// 開始処理
	Start();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CGame::Uninit(void)
{ 
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CGame::Update(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();			// ジョイパッドへのポインタ
	CDebugProc *pProc = pManager->GetDebugProc();		// デバッグ表示へのポインタ

	if (pKeyboard->GetTrigger(DIK_P) || pJoypad->GetTrigger(CJoypad::KEY_START))
	{ // ポーズ変更！
		pManager->SetEnablePause(!pManager->GetEnablePause());
	}

	if (pManager->GetEnablePause() == true)
	{ // ポーズ中
		pProc->Print("[現在ポーズ中！]\n");
	}

	// モードの遷移
	pProc->Print("<9/0で遷移 : ゲームオーバー画面 / ゲームクリア画面>\n");
	if (pKeyboard->GetTrigger(DIK_9))
	{
		pManager->SetTransition(MODE_GAMEOVER);
	}
	else if (pKeyboard->GetTrigger(DIK_0))
	{
		pManager->SetTransition(MODE_GAMECLEAR);
	}

	// マップのセーブ・ロード
	if (pKeyboard->GetTrigger(DIK_I) && pKeyboard->GetPress(DIK_LSHIFT))
	{
		time_t t = {};					// 時刻データ
		struct tm *pTime = nullptr;		// 時刻ポインタ
		char aTime[1024] = {};			// 時刻取得用文字列
		std::string filePath;			// ファイル名

		// 時刻を取得し文字列変換
		t = time(NULL);
		pTime = localtime(&t);
		strftime(aTime, sizeof(aTime), "%Y-%m-%d-%H-%M-%S", pTime);

		// ファイル名作成
		filePath = "data/";
		filePath += aTime;
		filePath += ".bin";

		CMap::GetInstance()->Save(filePath.c_str());
	}
	else if (pKeyboard->GetTrigger(DIK_U))
	{
		CMap::GetInstance()->Load("data/2026-07-24-10-18-18.txt");
	}

	// 全カメラの更新処理
	CCamera::UpdateAll();

	if (m_pTimer->GetTimer() <= 0)
	{ // 0以下になった場合ゲームオーバー
		pManager->SetTransition(MODE_GAMEOVER);
	}

	m_nCounterFrame++;
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CGame::Draw(void)
{
}

//==================================================================================
// --- 初期化後呼び出し処理 ---
//==================================================================================
void CGame::Start(void)
{
	CMap *pMap = CMap::GetInstance();		// マップへのポインタ

	const char *apBuildingPath[CMap::BUILDING_MAX] =
	{ // 各建物のモデルパス
		"data/MODEL/house000.x",
		"data/MODEL/house001.x",
		"data/MODEL/house002.x",
		"data/MODEL/house003.x",
	};

	for (int nCntBuilding = 0; nCntBuilding < CMap::BUILDING_MAX; nCntBuilding++)
	{ // モデルパスを登録
		pMap->Resister(static_cast<CMap::BUILDING>(nCntBuilding), apBuildingPath[nCntBuilding]);
	}

	// タイマー生成
	m_pTimer = CTimer::Create(D3DXVECTOR3(WINDOW_MIDDLE.x - 75.0f, 35.0f, 0.0f), D3DXVECTOR2(150.0f, 85.0f), 3, 120);

	// プレイヤー出現
	m_pPlayer = CPlayer::Create("data/SCRIPT/motion_nabeatsu.txt", D3DXVECTOR3(0.0f, 1125.0f, 0.0f), VECTOR3_NULL);
	NULLPOINTER_ASSERT(m_pPlayer);

	// 惑星配置
	m_pPlanet = CPlanet::Create();
	NULLPOINTER_ASSERT(m_pPlanet);

	// マップ読み込み
	CMap::GetInstance()->Load("data/2026-07-30-12-30-30.bin");
}