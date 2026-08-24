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
#include "combo.h"
#include "delegate_t.h"
#include "building.h"
#include "stopwatch.h"
#include <string>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define TIMER_POS			Vector3(WINDOW_MIDDLE.x - 75.0f, 35.0f, 0.0f)		// タイマーの座標
#define TIMER_SIZE			Vector2(150.0f, 85.0f)				// タイマーのサイズ
#define COMBO_POS			Vector3(1000.0f, 200.0f, 0.0f)		// コンボ表示の座標
#define PLAYER_MOTION_PATH	"data/SCRIPT/motion_nabeatsu.txt"	// プレイヤーのモーションパス

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CGame::CGame() : CScene(MODE_GAME)
{ // 親クラスのコンストラクタ呼び出し
	// メンバ変数のクリア
	m_pPlayer = nullptr;
	m_pPlanet = nullptr;
	m_pTimer = nullptr;
	m_pCombo = nullptr;
	m_bEdit = false;
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
{ // 開始処理
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

	if (pKeyboard->GetPress(DIK_LSHIFT) && pKeyboard->GetTrigger(DIK_E))
	{ // エディットモードの変更！
		m_bEdit = !m_bEdit;
		m_pTimer->SetUpdate(!m_bEdit);
	}

	if (pManager->GetEnablePause() == true)
	{ // ポーズ中
		pProc->Print("[現在ポーズ中！]\n");
	}

	// 経過時間を表示
	pProc->Print("[モード開始後からの経過時間 : {:.2f}]\n", m_pStopWatch->GetElapsed<float, std::ratio<1>>());

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
		filePath = "data/Maps/";
		filePath += aTime;
		filePath += ".bin";

		// マップを保存
		CMap::GetInstance()->Save(filePath.c_str());
	}

	if (m_bEdit == true)
	{ // エディットモードの場合
		UpdateEdit();
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

	// タイマー生成
	m_pTimer = CTimer::Create(TIMER_POS, TIMER_SIZE, 3, 120);

	// コンボ表示生成
	m_pCombo = CCombo::Create(COMBO_POS, VECTOR3_NULL);

	// プレイヤー出現
	m_pPlayer = CPlayer::Create(PLAYER_MOTION_PATH, Vector3(0.0f, 1125.0f, 0.0f), VECTOR3_NULL);
	NULLPOINTER_ASSERT(m_pPlayer);

	// 惑星配置
	m_pPlanet = CPlanet::Create();
	NULLPOINTER_ASSERT(m_pPlanet);

	// ストップウォッチを生成 + スタート
	m_pStopWatch = std::make_unique<CStopWatch>();
	m_pStopWatch->Start();

	// マップ読み込み
	CMap::GetInstance()->LoadLatest();
}

//==================================================================================
// --- エディットモードの更新処理 ---
//==================================================================================
void CGame::UpdateEdit(void)
{
	// マップ関連
	MapEdit();
}

//==================================================================================
// --- マップのエディット処理 ---
//==================================================================================
void CGame::MapEdit(void)
{
	CManager *pManager = CManager::GetInstance();					// マネージャーへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	Vector3 pos = Vector3(0.0f, m_pPlayer->GetPosition()->y, 0.0f);	// 設置位置
	CMap *pMap = CMap::GetInstance();			// マップへのポインタ

	if (pKeyboard->GetTrigger(DIK_1))
	{ // 建物0生成
		pMap->AddBulding(CBuilding::TYPE_0, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_2))
	{ // 建物1生成
		pMap->AddBulding(CBuilding::TYPE_1, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_3))
	{ // 建物2生成
		pMap->AddBulding(CBuilding::TYPE_2, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_4))
	{ // 建物3生成
		pMap->AddBulding(CBuilding::TYPE_3, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_5))
	{ // 電柱生成
		pMap->AddUtilityPole(pos);
	}
	else if (pKeyboard->GetTrigger(DIK_6))
	{ // 発電所生成
		pMap->AddPowerPlant(pos);
	}
}