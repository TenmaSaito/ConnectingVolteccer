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
#include "renderer.h"
#include "sound.h"
#include "input.h"
#include "joypad.h"
#include "texture.h"
#include "debugproc.h"
#include "player.h"
#include "planet.h"
#include "camera.h"
#include "mapManager.h"
#include "timer.h"
#include "combo.h"
#include "connectingEvaluate.h"
#include "delegate_t.h"
#include "stopwatch.h"
#include "directXSubDrawer.h"
#include "sceneTransition.h"
#include "thunderCamera.h"
#include "filestream.h"
#include "rankingManager.h"
#include "object2D.h"
#include "vec2math.h"
#include <string>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define TIMER_POS			Vector3(SCREEN_MIDDLE.x - 85.0f, SCREEN_HEIGHT - 100.0f, 0.0f)		// タイマーの座標
#define TIMER_SIZE			Vector2(150.0f, 85.0f)				// タイマーのサイズ
#define COMBO_POS			Vector3(1000.0f, 200.0f, 0.0f)		// コンボ表示の座標
#define EVALUATE_POS		Vector3(1000.0f, 350.0f, 0.0f)		// 評価表示の座標
#define EVALUATE_SCALE		Vector2(328.0f, 64.0f)				// 評価表示のサイズ
#define PLAYER_MOTION_PATH	"data/SCRIPT/motion_player.txt"	// プレイヤーのモーションパス
#define THUNDER_CAM_LENGTH	(1000.0f)						// 電流とカメラの距離
#define FRAME_PATH			"data/TEXTURE/frame.png"		// 画面フレームのパス
#define BACK_COLOR			Color(0.0f, 0.0f, 0.0f, 0.5f)	// 黒い半透明ポリゴンの色
#define READY_DISP_DURATION		(CManager::SecToFrame(3))		// Readyポリゴンが表示される時間
#define GO_DISP_DURATION		(CManager::SecToFrame(0.5f))	// Goポリゴンが表示される時間
#define FINISH_DISP_DURATIONE	(CManager::SecToFrame(3))		// Finishポリゴンが表示される時間
#define POLYGON_SCALE_VALUE		(2.5f)		// ポリゴンの初期の拡大倍率
#define POLYGON_ALPHA_VALUE		(0.05f)		// ポリゴンのα値の増減値

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	const CObject2D::Info c_infoReady =			// Readyポリゴンの情報
	{
		Vector3(SCREEN_MIDDLE.x, SCREEN_MIDDLE.y, 0.0f),		// Readyポリゴンの位置
		Vector2(800.0f, 100.0f),				// Readyポリゴンのサイズ
		"data/TEXTURE/game_ui/ready.png"		// Readyポリゴンのテクスチャパス
	};

	const CObject2D::Info c_infoGo =			// Goポリゴンの情報
	{
		Vector3(SCREEN_MIDDLE.x, SCREEN_MIDDLE.y, 0.0f),		// Goポリゴンの位置
		Vector2(800.0f, 100.0f),				// Goポリゴンのサイズ
		"data/TEXTURE/game_ui/go.png"			// Goポリゴンのテクスチャパス
	};

	const CObject2D::Info c_infoFinish =		// Finishポリゴンの情報
	{
		Vector3(SCREEN_MIDDLE.x, SCREEN_MIDDLE.y, 0.0f),		// Finishポリゴンの位置
		Vector2(800.0f, 100.0f),				// Finishポリゴンのサイズ
		"data/TEXTURE/game_ui/finish.png"		// Finishポリゴンのテクスチャパス
	};
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CGame::CGame()
{ // メンバ変数のクリア
	m_pPlayer = nullptr;
	m_pTimer = nullptr;
	m_pCombo = nullptr;
	m_pEvaluate = nullptr;
	m_pThunderCam = nullptr;
	m_pBack = nullptr;
	m_pReady = nullptr;
	m_pGo = nullptr;
	m_pFinish = nullptr;
	m_bCreateConnectEffect = false;
	m_nCounterFrame = 0;
	m_nNumLightingHouse = 0;
	m_nNumLightingLandmark = 0;
	m_nCurrentConnectLighting = 0;
	m_nCounterState = 0;
	m_state = STATE_NONE;
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

	// 準備状態に変更
	SetState(STATE_READY);
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
	CSound *pSound = pManager->GetSound();				// サウンドへのポインタ

	if (pKeyboard->GetTrigger(DIK_P) || pJoypad->GetTrigger(CJoypad::KEY_START))
	{ // ポーズ変更！
		pManager->SetEnablePause(!pManager->GetEnablePause());

		// ポーズ状態になったなら、歩行音と通電音を一時停止
		pSound->Pause(pManager->GetEnablePause(), CSound::LABEL_SE_WALK);
		pSound->Pause(pManager->GetEnablePause(), CSound::LABEL_SE_ELECTRIC);
	}

	// 状態別更新処理
	UpdateState();

#if _DEBUG
	CDebugProc *pProc = pManager->GetDebugProc();		// デバッグ表示へのポインタ

	// 経過時間を表示
	pProc->Print("[モード開始後からの経過時間 : {:.2f}]\n", m_pStopWatch->GetElapsed<float, std::ratio<1>>());

	// プレイヤーのワールド座標
	Vector3 posWorld = *m_pPlayer->GetPosition();
	D3DXVec3TransformCoord(&posWorld, &posWorld, m_pPlayer->GetMatrix());
	pProc->Print("[プレイヤーの絶対座標 : {:.2f} {:.2f} {:.2f}]\n", posWorld.x, posWorld.y, posWorld.z);
#endif

	// 全カメラの更新処理
	CCamera::UpdateAll();

	m_nCounterFrame++;		// フレームカウンターを増加　
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CGame::Draw(void)
{
}

//==================================================================================
// --- 状態設定処理 ---
//==================================================================================
void CGame::SetState(const STATE state)
{ // 引数を保存
	m_state = state;
	m_nCounterState = 0;		// 状態カウンターをリセット

	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();		// レンダラーへのポインタ

	switch (m_state)
	{
	case STATE_READY:		// 開始待機
		// タイマーを停止
		m_pTimer->SetUpdate(false);

		// 黒い半透明ポリゴンとReadyポリゴンの非表示を解除
		m_pBack->SetDisp(true);
		m_pReady->SetDisp(true);
		break;

	case STATE_NORMAL:		// 進行中
		// タイマーを再開
		m_pTimer->SetUpdate(true);

		// プレイヤーの動作制限を解除
		m_pPlayer->SetMovable(true);
		break;

	case STATE_END:			// 終了待機
		// タイマーを停止
		m_pTimer->SetUpdate(false);

		// 黒い半透明ポリゴンとFinishポリゴンの非表示を解除
		m_pBack->SetDisp(true);
		m_pFinish->SetDisp(true);

		// Finishポリゴンのサイズを拡大
		m_pFinish->SetSize(*m_pFinish->GetSize() * POLYGON_SCALE_VALUE);

		// フィードバックエフェクトを無効にする
		pRenderer->SetEnableFeedBack(false);
		break;
	}
}

//==================================================================================
// --- 初期化後呼び出し処理 ---
//==================================================================================
void CGame::Start(void)
{
	CMapManager *pMap = CMapManager::GetInstance();		// マップへのポインタ

	// フレームを生成
	CObject2D *pFrame = CObject2D::Create(SCREEN_MIDDLE, SCREEN_SIZE);
	pFrame->BindTexture(CTexture::GetInstance()->Register(FRAME_PATH));

	// タイマー生成
	m_pTimer = CTimer::Create(TIMER_POS, TIMER_SIZE, 3, 120);

	// コンボ表示生成
	m_pCombo = CCombo::Create(COMBO_POS, VECTOR3_NULL);

	// 評価表示生成
	m_pEvaluate = CConnectingEvaluate::Create(EVALUATE_POS, EVALUATE_SCALE);
	pMap->BindConnectingEvaluate(m_pEvaluate);

	// プレイヤー出現
	m_pPlayer = CPlayer::Create(PLAYER_MOTION_PATH, Vector3(0.0f, PLANET_HEIGHT, 0.0f), VECTOR3_NULL);
	m_pPlayer->BindCombo(m_pCombo);
	pMap->BindPlayer(m_pPlayer);
	NULLPOINTER_ASSERT(m_pPlayer);

	// マップ読み込み + 惑星へのポインタを取得
	pMap->LoadLatest();

	// 電流用カメラ生成
	m_pThunderCam = CThunderCamera::Create(THUNDER_CAM_LENGTH);

	// ストップウォッチを生成 + スタート
	m_pStopWatch = std::make_unique<CStopWatch>();
	m_pStopWatch->Start();

	// 黒い半透明ポリゴンを作成
	m_pBack = CObject2D::Create(SCREEN_MIDDLE, SCREEN_SIZE);
	m_pBack->SetColor(BACK_COLOR);
	m_pBack->SetDisp(false);

	// Readyポリゴンを作成
	m_pReady = CObject2D::Create(c_infoReady);
	m_pReady->SetAlpha(0.0f);
	m_pReady->SetDisp(false);

	// Goポリゴンを作成
	m_pGo = CObject2D::Create(c_infoGo);
	m_pGo->SetDisp(false);

	// Finishポリゴンを作成
	m_pFinish = CObject2D::Create(c_infoFinish);
	m_pFinish->SetAlpha(0.0f);
	m_pFinish->SetDisp(false);

	CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

	// タイトル画面のBGMを流す
	pSound->Play(CSound::LABEL_BGM_GAME);
}

//==================================================================================
// --- 状態別更新処理 ---
//==================================================================================
void CGame::UpdateState(void)
{
	switch (m_state)
	{
	case STATE_READY:		// 開始待機
		UpdateReady();
		break;

	case STATE_NORMAL:		// 進行中
		UpdateNormal();
		break;

	case STATE_END:			// 終了待機
		UpdateEnd();
		break;
	}
}

//==================================================================================
// --- 開始待機時更新処理 ---
//==================================================================================
void CGame::UpdateReady(void)
{
	// ここにREADY GO等のポリゴン表示処理を作成
	if (m_pReady->GetDisp() == true)
	{ // Readyポリゴンが表示されているなら
		// Readyポリゴンのα値が1.0f未満なら
		if (m_pReady->GetAlpha() < 1.0f)
		{ // Readyポリゴンのα値を増加
			float fAlpha = m_pReady->GetAlpha() + POLYGON_ALPHA_VALUE;		// 増加後のα値
			m_pReady->SetAlpha((fAlpha > 1.0f) ? 1.0f : fAlpha);
		}

		// Readyポリゴンを一定時間表示したなら
		if (m_nCounterState >= READY_DISP_DURATION)
		{ // Readyポリゴンを非表示にし、Goポリゴンを作成
			m_pReady->SetDisp(false);
			m_pGo->SetDisp(true);
			m_nCounterState = 0;		// 状態カウンターをリセット
		}
	}
	else if(m_pGo->GetDisp() == true)
	{ // Goポリゴンが表示されているなら
		// Readyポリゴンを一定時間表示したなら
		if (m_nCounterState >= GO_DISP_DURATION)
		{ // Goポリゴンと黒い半透明ポリゴンを徐々に透明にしていく
			if (m_pBack->GetAlpha() > 0.0f)
			{ // α値が0.0fよりも大きいなら、α値を減少
				float fAlpha = m_pBack->GetAlpha() - POLYGON_ALPHA_VALUE;		// 減少後のα値
				m_pBack->SetAlpha((fAlpha < 0.0f) ? 0.0f : fAlpha);

				// α値が0.0f未満になったなら、非表示に設定
				if (m_pBack->GetAlpha() <= 0.0f) m_pBack->SetDisp(false);
			}

			if (m_pGo->GetAlpha() > 0.0f)
			{ // α値が0.0fよりも大きいなら、α値を減少
				float fAlpha = m_pGo->GetAlpha() - POLYGON_ALPHA_VALUE;			// 減少後のα値
				m_pGo->SetAlpha((fAlpha < 0.0f) ? 0.0f : fAlpha);

				// α値が0.0f未満になったなら、非表示に設定
				if (m_pGo->GetAlpha() <= 0.0f) m_pGo->SetDisp(false);
			}

			if (m_pBack->GetDisp() == false && m_pGo->GetDisp() == false)
			{ // どちらのポリゴンも非表示になったなら、状態を変更
				SetState(STATE_NORMAL);
			}
		}
	}

	m_nCounterState++;		// 状態カウンターを増加
}

//==================================================================================
// --- 通常時更新処理 ---
//==================================================================================
void CGame::UpdateNormal(void)
{
	int nNumCurrentLightingHouse = m_pEvaluate->GetLightingHouse();		// 今回の通電で電気のついた家の数
	if (nNumCurrentLightingHouse > 0)
	{ // 電気のついた家が1軒以上あり、未だその演出を出していないならば
		m_nNumLightingHouse += nNumCurrentLightingHouse;		// 電気のついた家分総数増加

		// 評価演出を追加
		m_pEvaluate->Evaluate();
	}

	if (m_pTimer->GetTimer() <= 0)
	{ // タイマーがゼロになったら、終了状態に変更
		SetState(STATE_END);
	}
}

//==================================================================================
// --- 終了時更新処理 ---
//==================================================================================
void CGame::UpdateEnd(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();		// レンダラーへのポインタ

	// 黒い半透明ポリゴンのα値が規定値以下なら
	if (m_pBack->GetAlpha() < BACK_COLOR.a)
	{ // α値を増加
		float fAlpha = m_pBack->GetAlpha() + POLYGON_ALPHA_VALUE;			// 増加後のα値
		m_pBack->SetAlpha((fAlpha > BACK_COLOR.a) ? BACK_COLOR.a : fAlpha);
	}

	// Finishポリゴンのα値が規定値以下なら
	if (m_pFinish->GetAlpha() < 1.0f)
	{ // α値を増加
		float fAlpha = m_pFinish->GetAlpha() + POLYGON_ALPHA_VALUE;			// 増加後のα値
		m_pFinish->SetAlpha((fAlpha > 1.0f) ? 1.0f : fAlpha);

		// サイズを元に戻す
		Vector2 size = Vec2::Lerp(c_infoFinish.size * POLYGON_SCALE_VALUE, c_infoFinish.size, fAlpha);
		m_pFinish->SetSize(size);
	}

	if (m_nCounterState > READY_DISP_DURATION
		&& pManager->GetState() == CSceneTransition::STATE_STAY)
	{ // Finishが表示されてから一定時間経った場合、リザルトへ移行
		std::unique_ptr pFile = std::make_unique<CFileStream>();		// ファイルストリームへのポインタ
		CMapManager *pMap = CMapManager::GetInstance();					// マップマネージャへのポインタ

		// 今回の結果をファイルに書き出し
		if (pFile->CreateFile(CURRENT_SCORE_PATH, true, CFileStream::FLAG_OVERWRITE))
		{ // ファイル生成成功時
			int nNumLighting = m_nNumLightingHouse - m_nNumLightingLandmark;	// 電気のついた建物数
			float fPercent = static_cast<float>(nNumLighting)					// 電気のついた家の割合
				/ static_cast<float>(pMap->GetNumBuilding());

			fPercent += static_cast<float>(m_nNumLightingLandmark) * 0.1f;		// 電気のついたランドマークの割合を追加

			// 割合を書き出し
			pFile->Write(fPercent);

			// ファイルを閉じる
			pFile->CloseFile();
		}

		// 今回の接続インデックスを保存
		pMap->SaveConnectID();

		// シーンを遷移
		pManager->SetTransition(MODE_RESULT);
	}

	m_nCounterState++;		// 状態カウンターを増加
}