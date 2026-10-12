//==================================================================================
// 
// スキップ表示クラスのソースファイル [skip.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/25
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "skip.h"
#include "manager.h"
#include "input.h"
#include "joypad.h"
#include "sound.h"
#include "polygon2D.h"
#include "color.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define PRESSING_SKIP_SIZE_REDUCTION	(0.85f)		// 長押しされているときのサイズ縮小倍率
#define PRESS_DURATION		(CManager::SecToFrame(2.0f))			// 長押しする時間
#define GAUGE_COL			(Color(1.0f, 1.0f, 0.0f, 0.45f))		// ゲージの色

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CSkip *CSkip::Create(const Vector3 &pos, const Vector2 &size)
{
	CSkip *pSkip = new CSkip;		// 生成したオブジェクトへのポインタ
	if (pSkip != nullptr)
	{ // 生成出来ていれば、初期化
		pSkip->Init(pos, size);
	}

	return pSkip;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CSkip::CSkip()
{ // メンバ変数をクリア
	m_pos = VECTOR3_NULL;
	m_size = VECTOR2_NULL;
	m_nFramePress = 0;

	// タイプ指定
	SetType(TYPE_SKIP);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CSkip::~CSkip()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CSkip::Init(const Vector3 &pos, const Vector2 &size)
{
	// スキップ表示作成
	m_pSkip.reset(CPolygon2D::Create(pos, VECTOR3_NULL, size));

	// ゲージ作成
	m_pGauge.reset(CPolygon2D::Create(pos, VECTOR3_NULL, size));
	m_pGauge->SetSize(Vector2(m_pSkip->GetSize()->x, 0.0f));
	m_pGauge->SetColor(GAUGE_COL);

	// 生成出来ていれば成功
	return (m_pSkip != nullptr && m_pGauge != nullptr) ? S_OK : E_FAIL;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CSkip::Uninit(void)
{ // 各ポリゴンを解放
	SafeUniqueUninit(m_pSkip);
	SafeUniqueUninit(m_pGauge);

	// 解放処理
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CSkip::Update(void)
{ // シーン遷移中でなければ、各更新処理実行
	CManager *pManager = CManager::GetInstance();		// マネージャーへのポインタ
	if (pManager->GetState() != CSceneTransition::STATE_STAY) return;

	// 入力関連
	UpdateInput();

	// ポリゴン関連
	UpdatePolygon();

	if (m_nFramePress > PRESS_DURATION)
	{ // 一定時間押し続けたら
		// スキップ表示とゲージのサイズを元に戻す
		m_pSkip->SetSize(*m_pSkip->GetSize() * (1.0f + (1.0f - PRESSING_SKIP_SIZE_REDUCTION)));
		m_pGauge->SetSize(Vector2(m_pSkip->GetSize()->x, 0.0f));
		m_nFramePress = 0;		// 長押しフレームカウントをリセット

		// 既に移行中の場合はスキップ
		if (pManager->GetState() == CSceneTransition::STATE_STAY) pManager->SetTransition(CScene::MODE_GAME);
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CSkip::Draw(void)
{ // nullでなければ描画
	if (m_pSkip != nullptr) m_pSkip->Draw();
	if (m_pGauge != nullptr) m_pGauge->Draw();
}

//==================================================================================
// --- テクスチャ割り当て処理 ---
//==================================================================================
void CSkip::BindSkipTexture(const int nIdxTexture)
{
	m_pSkip->BindTexture(nIdxTexture);
}

//==================================================================================
// --- 入力更新処理 ---
//==================================================================================
void CSkip::UpdateInput(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();			// ジョイパッドへのポインタ
	CSound *pSound = pManager->GetSound();				// サウンドへのポインタ

	if (pKeyboard->GetTrigger(DIK_BACK)
		|| pJoypad->GetTrigger(CJoypad::KEY_BACK))
	{ // BACKキー / ボタンが押された瞬間
		// スキップ表示のサイズを縮小
		m_pSkip->SetSize(*m_pSkip->GetSize() * PRESSING_SKIP_SIZE_REDUCTION);

		// 長押し音再生
		pSound->Play(CSound::LABEL_SE_SKIP);
	}
	else if (pKeyboard->GetRelease(DIK_BACK)
		|| pJoypad->GetRelease(CJoypad::KEY_BACK))
	{ // BACKキー / ボタンが離されたなら
		// スキップ表示とゲージのサイズを元に戻す
		m_pSkip->SetSize(*m_pSkip->GetSize() * (1.0f + (1.0f - PRESSING_SKIP_SIZE_REDUCTION)));
		m_pGauge->SetSize(Vector2(m_pSkip->GetSize()->x, 0.0f));
		m_nFramePress = 0;		// 長押しフレームカウントをリセット

		// 長押し音停止
		pSound->Stop(CSound::LABEL_SE_SKIP);
	}
	else if (pKeyboard->GetPress(DIK_BACK)
		|| pJoypad->GetPress(CJoypad::KEY_BACK))
	{ // BACKキー / ボタンが押され続けているなら
		m_nFramePress++;		// フレームカウント増加
	}
}

//==================================================================================
// --- 各ポリゴン更新処理 ---
//==================================================================================
void CSkip::UpdatePolygon(void)
{ // 押され続けていなければ、スキップ
	if (m_nFramePress < 0) return;

	// ゲージのサイズを拡大
	m_pGauge->SetSize(Vector2(m_pSkip->GetSize()->x, m_pSkip->GetSize()->y * (m_nFramePress / PRESS_DURATION)));
}