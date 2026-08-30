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
#include "connectingEvaluate.h"
#include "delegate_t.h"
#include "building.h"
#include "stopwatch.h"
#include "directXSubDrawer.h"
#include "sceneTransition.h"
#include "thunderCamera.h"
#include "meshCylinder.h"
#include "filestream.h"
#include <string>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define TIMER_POS			Vector3(SCREEN_MIDDLE.x - 75.0f, 35.0f, 0.0f)		// タイマーの座標
#define TIMER_SIZE			Vector2(150.0f, 85.0f)				// タイマーのサイズ
#define COMBO_POS			Vector3(1000.0f, 200.0f, 0.0f)		// コンボ表示の座標
#define EVALUATE_POS		Vector3(1000.0f, 350.0f, 0.0f)		// 評価表示の座標
#define EVALUATE_SCALE		Vector2(328.0f, 64.0f)				// 評価表示のサイズ
#define PLAYER_MOTION_PATH	"data/SCRIPT/motion_nabeatsu.txt"	// プレイヤーのモーションパス
#define THUNDER_CAM_LENGTH	(1000.0f)		// 電流とカメラの距離
#define CURRENT_SCORE_PATH	"data/SCORE/current.bin"		// ゲームシーン終了時のスコアを書き出すファイルパス

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CGame::CGame()
{ // メンバ変数のクリア
	m_pPlayer = nullptr;
	m_pPlanet = nullptr;
	m_pTimer = nullptr;
	m_pCombo = nullptr;
	m_pEvaluate = nullptr;
	m_pThunderCam = nullptr;
	m_bEdit = false;
	m_bPause = false;
	m_bCreateConnectEffect = false;
	m_nCounterFrame = 0;
	m_nNumLightingHouse = 0;
	m_nCurrentConnectLighting = 0;
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
	// DirectXの処理を別スレッドにて開始
	std::unique_ptr<CDirectXSubDrawer> pSubDrawer = std::make_unique<CDirectXSubDrawer>();
	hyp::Action<> draw;		// 描画時に呼び出す関数
	draw.Add(&CSceneTransition::Draw, CManager::GetInstance()->GetTransition());
	pSubDrawer->FunctionSetUp(CDirectXSubDrawer::FUNCTION_DRAW, draw);
	std::thread thSubDrawer(&CDirectXSubDrawer::DirectXSubDrawerProc, pSubDrawer.get());

	Start();

	pSubDrawer->Quit();
	thSubDrawer.join();

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

	// プレイヤーのワールド座標
	Vector3 posWorld = *m_pPlayer->GetPosition();
	D3DXVec3TransformCoord(&posWorld, &posWorld, m_pPlayer->GetMatrix());
	pProc->Print("[プレイヤーの絶対座標 : {:.2f} {:.2f} {:.2f}]\n", posWorld.x, posWorld.y, posWorld.z);

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

	if (m_nCurrentConnectLighting > 0 && m_bCreateConnectEffect == false)
	{ // 電気のついた家が1軒以上あり、未だその演出を出していないならば
		// 評価演出を追加 + 演出フラグを立てる
		m_pEvaluate->AddEvaluate(m_nCurrentConnectLighting);
		m_bCreateConnectEffect = true;

		m_nNumLightingHouse += m_nCurrentConnectLighting;		// 電気のついた家分総数増加
	}

	if (m_bEdit == true)
	{ // エディットモードの場合
		UpdateEdit();
	}

	// 全カメラの更新処理
	CCamera::UpdateAll();

	if (m_pTimer->GetTimer() <= 0 && pManager->GetTransition()->GetState() == CSceneTransition::STATE_STAY)
	{ // 0以下になった場合リザルトへ移行
		std::unique_ptr pFile = std::make_unique<CFileStream>();		// ファイルストリームへのポインタ

		// 今回の結果をファイルに書き出し
		if (pFile->CreateFile(CURRENT_SCORE_PATH, true, CFileStream::FLAG_OVERWRITE))
		{ // ファイル生成成功時
			float fPercent = static_cast<float>(m_nNumLightingHouse)	// 電気のついた家の割合
				/ static_cast<float>(CMap::GetInstance()->GetNumBuilding());

			// 割合を書き出し
			pFile->Write(fPercent);
			
			// ファイルを閉じる
			pFile->CloseFile();
		}

		pManager->SetTransition(MODE_RESULT);
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
// --- 評価演出用変数リセット処理 ---
//==================================================================================
void CGame::ResetCurrentLightingHouseNum(void)
{ // 電気がついた家の数と演出生成の有無をリセット
	m_nCurrentConnectLighting = 0;
	m_bCreateConnectEffect = false;
}

//==================================================================================
// --- 初期化後呼び出し処理 ---
//==================================================================================
void CGame::Start(void)
{
	CMap *pMap = CMap::GetInstance();		// マップへのポインタ

	// マップをリセット
	pMap->Reset();

	// タイマー生成
	m_pTimer = CTimer::Create(TIMER_POS, TIMER_SIZE, 3, 120);

	// コンボ表示生成
	m_pCombo = CCombo::Create(COMBO_POS, VECTOR3_NULL);

	// コンボ表示生成
	m_pEvaluate = CConnectingEvaluate::Create(EVALUATE_POS, EVALUATE_SCALE);

	// 惑星配置
	m_pPlanet = CPlanet::Create();

	// ポインタを登録
	pMap->SetCurrentScenePlanet(m_pPlanet);
	NULLPOINTER_ASSERT(m_pPlanet);

	// プレイヤー出現
	m_pPlayer = CPlayer::Create(PLAYER_MOTION_PATH, Vector3(0.0f, m_pPlanet->GetVtxMax().y, 0.0f), VECTOR3_NULL);
	NULLPOINTER_ASSERT(m_pPlayer);

	// 電流用カメラ生成
	m_pThunderCam = CThunderCamera::Create(THUNDER_CAM_LENGTH);

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
{ // マップ関連
	MapEdit();
}

//==================================================================================
// --- マップのエディット処理 ---
//==================================================================================
void CGame::MapEdit(void)
{
	CManager *pManager = CManager::GetInstance();					// マネージャーへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	Vector3 pos = Vector3(0.0f, m_pPlanet->GetVtxMax().y, 0.0f);	// 設置位置
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