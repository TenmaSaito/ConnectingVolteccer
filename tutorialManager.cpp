//==================================================================================
// 
// チュートリアル進行管理クラスのソースファイル [tutorialManager.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/2
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "tutorialManager.h"
#include "mapManager.h"
#include "polygon2D.h"
#include "object2D.h"
#include "electricalCable.h"
#include "util.h"
#include "tutorial.h"
#include "manager.h"
#include "input.h"
#include "joypad.h"
#include "sound.h"
#include "texture.h"
#include "sceneTransition.h"
#include "combo.h"
#include "camera.h"
#include "color.h"
#include "player.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define COMMENTBASE_POS		(Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT - 125.0f, 0.0f))		// コメント土台ポリゴンの位置
#define COMMENTBASE_SIZE	(Vector2(SCREEN_WIDTH * 0.9f, 200.0f))								// コメント土台ポリゴンのサイズ
#define COMMENT_POS			(Vector3(SCREEN_WIDTH * 0.56f, SCREEN_HEIGHT - 130.0f, 0.0f))		// コメントポリゴンの位置
#define COMMENT_SIZE		(Vector2(900.0f, 150.0f))					// コメントポリゴンのサイズ
#define NEXT_BUTTON_POS		(Vector3(1000.0f, 685.0f, 0.0f))			// 次へボタンの位置
#define NEXT_BUTTON_SIZE	(Vector2(475.0f, 75.0f))					// 次へボタンのサイズ
#define TUTORIAL_MAP_PATH	"data/MAPS/tutorial_map.bin"				// チュートリアルのマップへのパス
#define COMMENT_BASE_PATH	"data/TEXTURE/tutorial_ui/commentBase.png"	// コメントの土台ポリゴンのテクスチャパス
#define NEXT_BUTTON_PATH	"data/TEXTURE/tutorial_ui/next.png"			// 次へボタンのテクスチャパス
#define FOG_COLOR			(Colors::GetColor(Colors::C_BLACK))			// フォグの色
#define FOG_START			(400.0f)		// フォグの開始位置
#define FOG_END				(1800.0f)		// フォグの終了位置
#define FOG_INCREASE		(10.0f)			// フォグの増加値
#define PITCH_START			(0.4f)			// 始めのピッチ速度
#define PITCH_END			(1.0f)			// 終わりのピッチ速度
#define PITCH_VALUE			(0.005f)		// ピッチの増加値

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asPhaseCommentPath[CTutorialManager::PHASE_MAX] = // 各フェーズのコメントのパス
	{
		"data/TEXTURE/tutorial_ui/first.png",		// 理由1
		"data/TEXTURE/tutorial_ui/second.png",		// 理由2
		"data/TEXTURE/tutorial_ui/third.png",		// チュートリアル開始
		"data/TEXTURE/tutorial_ui/fourth.png",		// 発電所を見つけよう (操作フォーカス終了)
		"data/TEXTURE/tutorial_ui/fifth.png",		// 近づいて発電所に乗ってみよう
		"data/TEXTURE/tutorial_ui/sixth.png",		// 接続方法のチュートリアル1 (繋げ方)
		"data/TEXTURE/tutorial_ui/seventh.png",		// 接続方法のチュートリアル2 (選び方)
		"data/TEXTURE/tutorial_ui/eighth.png",		// 通電のチュートリアル
		"data/TEXTURE/tutorial_ui/ninth.png",		// 通電による影響 (操作フォーカス開始)
		"data/TEXTURE/tutorial_ui/ten.png",			// チュートリアル終了！
	};
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CTutorialManager *CTutorialManager::Create(void)
{
	CTutorialManager *pTutorialManager = new CTutorialManager;		// 生成したオブジェクトへのポインタ
	if (pTutorialManager != nullptr)
	{ // 生成成功時、初期化
		pTutorialManager->Init();
	}

	return pTutorialManager;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTutorialManager::CTutorialManager()
{ // メンバ変数のクリア
	m_phase = PHASE_REASON;
	m_bInputFocus = false;
	m_fFogStart = 0.0f;
	m_fPitch = 0.0f;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CTutorialManager::~CTutorialManager()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTutorialManager::Init(void)
{
	CMapManager *pMap = CMapManager::GetInstance();		// マップマネージャへのポインタ
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャ管理オブジェクトへのポインタ

	for (auto &path : c_asPhaseCommentPath)
	{ // テクスチャを予め読み込み
		pTexture->Register(path);
	}

	// コメントの土台ポリゴンを作成
	m_pCommentBase.reset(CPolygon2D::Create(COMMENTBASE_POS,
		VECTOR3_NULL,
		COMMENTBASE_SIZE));
	m_pCommentBase->BindTexture(pTexture->Register(COMMENT_BASE_PATH));

	// コメントを作成
	m_pComment.reset(CPolygon2D::Create(COMMENT_POS,
		VECTOR3_NULL,
		COMMENT_SIZE));
	m_pComment->BindTexture(pTexture->Register(c_asPhaseCommentPath[m_phase]));

	// 次へボタンを生成
	m_pNextInfo.reset(CPolygon2D::Create(NEXT_BUTTON_POS,
		VECTOR3_NULL,
		NEXT_BUTTON_SIZE));
	m_pNextInfo->BindTexture(pTexture->Register(NEXT_BUTTON_PATH));

	m_bInputFocus = true;	// フォーカスを設定

	CCamera *pPlayerCam = CCamera::GetCamera(CCamera::TYPE_PLAYER);		// プレイヤーカメラへのポインタ

	// プレイヤーカメラにフォグを掛ける
	pPlayerCam->SetPixelFog(FOG_COLOR, FOG_START, FOG_END);
	pPlayerCam->SetEnablePixelFog(true);

	m_fFogStart = FOG_START;	// フォグの開始位置を保存
	m_fPitch = PITCH_START;		// ピッチの初期値を保存

	CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

	// ゲームBGMのピッチを設定する
	pSound->SetPitch(CSound::LABEL_BGM_GAME, m_fPitch);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CTutorialManager::Uninit(void)
{
	CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

	// ゲームBGMのピッチを元に戻す
	pSound->SetPitch(CSound::LABEL_BGM_GAME);

	// 各ポリゴンを破棄
	SafeUniqueUninit(m_pNextInfo);
	SafeUniqueUninit(m_pComment);
	SafeUniqueUninit(m_pCommentBase);
	SafeUniqueUninit(m_pEffect);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CTutorialManager::Update(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CTutorial *pTutorial = pManager->GetScene(&pTutorial);		// チュートリアルへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();		// ジョイパッドへのポインタ
	CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

	if ((pKeyboard->GetTrigger(DIK_RETURN)
		|| pJoypad->GetTrigger(CJoypad::KEY_A))
		&& m_bInputFocus == true
		&& pManager->GetState() == CSceneTransition::STATE_STAY)
	{ // 入力をフォーカス中なら、AもしくはEnterでフェーズを進める
		SetNextPhase();

		// クリック音再生
		pSound->Play(CSound::LABEL_SE_SELECT);
	}

	if (m_phase >= PHASE_AFTER_LIVELINESS && m_fFogStart < FOG_END)
	{ // チュートリアル内で既に通電していて、フォグが晴れていない場合
		CCamera *pPlayerCam = CCamera::GetCamera(CCamera::TYPE_PLAYER);		// プレイヤーカメラへのポインタ
		m_fFogStart += FOG_INCREASE;		// フォグの開始位置を進める
		if (m_fFogStart >= FOG_END)
		{ // フォグの開始位置が終了位置まで付いた場合、フォグを無効にする
			pPlayerCam->SetEnablePixelFog(false);
		}
		else
		{ // プレイヤーカメラにフォグを掛ける
			pPlayerCam->SetPixelFog(FOG_COLOR, m_fFogStart, FOG_END);
		}
	}

	if (m_phase >= PHASE_AFTER_LIVELINESS && m_fPitch < PITCH_END)
	{ // チュートリアル内で既に通電していて、ピッチが未だ最大ではない場合
		m_fPitch += PITCH_VALUE;
		if (m_fPitch >= PITCH_END)
		{ // 最大値を超えた場合、修正
			m_fPitch = PITCH_END;
		}

		// ゲームBGMのピッチを設定する
		pSound->SetPitch(CSound::LABEL_BGM_GAME, m_fPitch);
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CTutorialManager::Draw(void)
{ // チュートリアル用ポリゴンを描画
	if (m_pEffect != nullptr) m_pEffect->Draw();
	if (m_pCommentBase != nullptr) m_pCommentBase->Draw();
	if (m_pComment != nullptr) m_pComment->Draw();
	if (m_pNextInfo != nullptr && m_bInputFocus == true) m_pNextInfo->Draw();
}

//==================================================================================
// --- フェーズの進行処理 ---
//==================================================================================
void CTutorialManager::SetNextPhase(void)
{
	if (m_phase == PHASE_TUTORIAL_END)
	{ // 既にチュートリアルが終了しているなら、ゲームに移行
		CManager *pManager = CManager::GetInstance();		// マネージャーへのポインタ

		// 既に移行中の場合はスキップ
		if(pManager->GetState() == CSceneTransition::STATE_STAY) pManager->SetTransition(CScene::MODE_GAME);
		return;
	}

	// フェーズを進め、テクスチャを切り替え
	m_phase = Util::AddEnum(m_phase, 1);
	m_pComment->BindTexture(CTexture::GetInstance()->Register(c_asPhaseCommentPath[m_phase]));

	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CTutorial *pTutorial = pManager->GetScene(&pTutorial);		// チュートリアルへのポインタ
	CPlayer *pPlayer = pTutorial->GetPlayer();			// プレイヤーへのポインタ

	if (m_phase == PHASE_RIDEON)
	{ // 発電所を見つけるフェーズになったら、入力のフォーカスを解除
		m_bInputFocus = false;
		pPlayer->SetMovable(true);
	}
	else if (m_phase == PHASE_AFTER_LIVELINESS)
	{ // 通電後のフェーズになったら、入力のフォーカスを設定
		m_bInputFocus = true;
		pPlayer->SetMovable(false);
	}
}