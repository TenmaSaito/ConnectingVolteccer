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
#include "renderer.h"
#include "sound.h"
#include "input.h"
#include "joypad.h"
#include "texture.h"
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
#include "object2D.h"
#include "skip.h"
#include "matrix.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define COMBO_POS			Vector3(1000.0f, 200.0f, 0.0f)		// コンボ表示の位置
#define EVALUATE_POS		Vector3(1000.0f, 350.0f, 0.0f)		// 評価表示の位置
#define EVALUATE_SCALE		Vector2(328.0f, 64.0f)				// 評価表示のサイズ
#define SKIP_POS			Vector3(225.0f, 50.0f, 0.0f)		// スキップ表示の位置
#define SKIP_SCALE			Vector2(400.0f, 60.0f)				// スキップ表示のサイズ
#define SKIP_PATH			"data/TEXTURE/tutorial_ui/skip.png"	// スキップ表示のテクスチャパス
#define PLAYER_MOTION_PATH	"data/SCRIPT/motion_player.txt"	// プレイヤーのモーションパス
#define THUNDER_CAM_LENGTH	(1000.0f)		// 電流とカメラの距離
#define FRAME_PATH			"data/TEXTURE/frame.png"		// 画面フレームのパス

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
{ // 生成されていれば、破棄
	SafeUniqueUninit(m_pTutorialManager);
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
// --- 初期化後呼び出し処理 ---
//==================================================================================
void CTutorial::Start(void)
{
	CMapManager *pMap = CMapManager::GetInstance();		// マップへのポインタ
	CPlanet *pPlanet = nullptr;		// 惑星へのポインタ

	// フレームを生成
	CObject2D *pFrame = CObject2D::Create(SCREEN_MIDDLE, SCREEN_SIZE);
	pFrame->BindTexture(CTexture::GetInstance()->Register(FRAME_PATH));

	// コンボ表示生成 + ゲージ減少をストップ
	m_pCombo = CCombo::Create(COMBO_POS, VECTOR3_NULL);
	m_pCombo->SetEnablePause(true);

	// 評価クラス生成
	m_pEvaluate = CConnectingEvaluate::Create(EVALUATE_POS, EVALUATE_SCALE);
	pMap->BindConnectingEvaluate(m_pEvaluate);

	// プレイヤー出現
	m_pPlayer = CPlayer::Create(PLAYER_MOTION_PATH, Vector3(0.0f, PLANET_HEIGHT, 0.0f), VECTOR3_NULL);
	m_pPlayer->BindCombo(m_pCombo);
	pMap->BindPlayer(m_pPlayer);

	// チュートリアルマネージャ生成
	m_pTutorialManager.reset(CTutorialManager::Create());

	// マップ読み込み
	pMap->Load("data/MAPS/tutorial_map.bin");

	// 電流用カメラ生成
	m_pThunderCam = CThunderCamera::Create(THUNDER_CAM_LENGTH);

	CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

	// スキップ表示を生成
	CSkip *pSkip = CSkip::Create(SKIP_POS, SKIP_SCALE);
	pSkip->BindSkipTexture(CTexture::GetInstance()->Register(SKIP_PATH));

	// ゲームBGMを流す
	pSound->Play(CSound::LABEL_BGM_GAME);
}