//==================================================================================
// 
// チュートリアルクラスのソースファイル [tutorial.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/2
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "tutorial.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "joypad.h"
#include "debugproc.h"
#include "combo.h"
#include "connectingEvaluate.h"
#include "delegate_t.h"
#include "directXSubDrawer.h"
#include "sceneTransition.h"
#include "thunderCamera.h"
#include "mapManager.h"
#include "tutorialManager.h"
#include "planet.h"
#include "player.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define COMBO_POS			Vector3(1000.0f, 200.0f, 0.0f)		// コンボ表示の座標
#define EVALUATE_POS		Vector3(1000.0f, 350.0f, 0.0f)		// 評価表示の座標
#define EVALUATE_SCALE		Vector2(328.0f, 64.0f)				// 評価表示のサイズ
#define PLAYER_MOTION_PATH	"data/SCRIPT/motion_player.txt"	// プレイヤーのモーションパス
#define THUNDER_CAM_LENGTH	(1000.0f)		// 電流とカメラの距離

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTutorial::CTutorial()
{ // メンバ変数のクリア
	m_pCombo = nullptr;
	m_pEvaluate = nullptr;
	m_pPlayer = nullptr;
	m_pThunderCam = nullptr;
	m_bCreateConnectEffect = false;
	m_nCurrentConnectLighting = 0;
	m_nNumLightingHouse = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CTutorial::~CTutorial()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTutorial::Init(void)
{
	// DirectXの処理を別スレッドにて開始
	std::unique_ptr<CDirectXSubDrawer> pSubDrawer = std::make_unique<CDirectXSubDrawer>();
	hyp::Action<> draw;		// 描画時に呼び出す関数
	draw.Add(&CSceneTransition::Draw, CManager::GetInstance()->GetTransition());
	pSubDrawer->FunctionSetUp(CDirectXSubDrawer::FUNCTION_DRAW, draw);
	std::thread thSubDrawer(&CDirectXSubDrawer::DirectXSubDrawerProc, pSubDrawer.get());

	// 開始処理
	Start();

	pSubDrawer->Quit();
	thSubDrawer.join();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CTutorial::Uninit(void)
{
	if (m_pTutorialManager != nullptr)
	{ // 生成されていれば、破棄
		m_pTutorialManager->Uninit();
		m_pTutorialManager.reset();
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CTutorial::Update(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();			// ジョイパッドへのポインタ
	CDebugProc *pProc = pManager->GetDebugProc();		// デバッグ表示へのポインタ

	if (pKeyboard->GetTrigger(DIK_P) || pJoypad->GetTrigger(CJoypad::KEY_START))
	{ // ポーズ変更！
		pManager->SetEnablePause(!pManager->GetEnablePause());
	}

	if (pKeyboard->GetTrigger(DIK_9))
	{ // ポーズ変更！
		CObject::ReleaseAll();
		CMapManager::GetInstance()->Unload();
		Start();
	}

	if (m_pEvaluate->GetLightingHouse() > 0)
	{ // 電気のついた家が1軒以上あり、未だその演出を出していないならば
		// 評価演出を追加 + 演出フラグを立てる
		m_nNumLightingHouse += m_pEvaluate->GetLightingHouse();
		m_pEvaluate->Evaluate();
	}

	m_pTutorialManager->Update();

	// 全カメラの更新処理
	CCamera::UpdateAll();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CTutorial::Draw(void)
{ // チュートリアルマネージャの描画
	m_pTutorialManager->Draw();
}

//==================================================================================
// --- 各リソースの生成し直し処理 ---
//==================================================================================
void CTutorial::Restart(std::string_view sMapPath)
{
	CMapManager *pMap = CMapManager::GetInstance();		// マップへのポインタ
	CPlanet *pPlanet = nullptr;		// 惑星へのポインタ

	// オブジェクトの破棄 + 現在のマップ情報を破棄
	CObject::ReleaseAll();
	pMap->Unload();

	// コンボ表示生成
	m_pCombo = CCombo::Create(COMBO_POS, VECTOR3_NULL);

	// コンボ表示生成
	m_pEvaluate = CConnectingEvaluate::Create(EVALUATE_POS, EVALUATE_SCALE);
	pMap->BindConnectingEvaluate(m_pEvaluate);

	// プレイヤー出現
	m_pPlayer = CPlayer::Create(PLAYER_MOTION_PATH, Vector3(0.0f, 1115.0f, 0.0f), VECTOR3_NULL);
	m_pPlayer->BindCombo(m_pCombo);
	pMap->BindPlayer(m_pPlayer);
	NULLPOINTER_ASSERT(m_pPlayer);

	// マップ読み込み + 惑星へのポインタを取得
	pMap->Load(sMapPath);

	// 電流用カメラ生成
	m_pThunderCam = CThunderCamera::Create(THUNDER_CAM_LENGTH);
}

//==================================================================================
// --- 初期化後呼び出し処理 ---
//==================================================================================
void CTutorial::Start(void)
{
	CMapManager *pMap = CMapManager::GetInstance();		// マップへのポインタ
	CPlanet *pPlanet = nullptr;		// 惑星へのポインタ

	// コンボ表示生成
	m_pCombo = CCombo::Create(COMBO_POS, VECTOR3_NULL);

	// コンボ表示生成
	m_pEvaluate = CConnectingEvaluate::Create(EVALUATE_POS, EVALUATE_SCALE);
	pMap->BindConnectingEvaluate(m_pEvaluate);

	// プレイヤー出現
	m_pPlayer = CPlayer::Create(PLAYER_MOTION_PATH, Vector3(0.0f, 1115.0f, 0.0f), VECTOR3_NULL);
	m_pPlayer->BindCombo(m_pCombo);
	pMap->BindPlayer(m_pPlayer);
	NULLPOINTER_ASSERT(m_pPlayer);

	// チュートリアルマネージャ生成
	m_pTutorialManager.reset(CTutorialManager::Create());

	// マップ読み込み
	pMap->Load(m_pTutorialManager->GetTutorialMapPath());

	// 電流用カメラ生成
	m_pThunderCam = CThunderCamera::Create(THUNDER_CAM_LENGTH);

	CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

	// ゲームBGMを流す
	pSound->Play(CSound::LABEL_BGM_GAME);
}