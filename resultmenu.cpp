//==================================================================================
// 
// リザルト画面のメニュークラスのソースファイル [resultmenu.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "resultmenu.h"
#include "polygon2D.h"
#include "percent.h"
#include "texture.h"
#include "number.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "joypad.h"
#include "mapManager.h"
#include "util.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DOT_PATH			"data/TEXTURE/dot.png"			// 小数点テクスチャのパス
#define PERCENT_PATH		"data/TEXTURE/percent.png"		// パーセントテクスチャのパス
#define PERCENT_INCREASE	(0.0027f)		// パーセントの増加する値
#define MENU_SCALE_MAGNI	(1.5f)			// カーソルと当たった時のサイズ拡大倍率

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asPathMenu[CResultMenu::TYPE_MAX] =	// 各メニューのテクスチャパス
	{
		"data/TEXTURE/result_ui/replay.png",	// リプレイボタン
		"data/TEXTURE/result_ui/exit.png",		// 終了ボタン
	};

	constexpr std::string_view c_asPathScore[CResultMenu::SCORE_MAX] =	// 各スコア説明のテクスチャパス
	{
		"data/TEXTURE/result_ui/scoreBefore.png",	// スコアの上の説明
		"data/TEXTURE/result_ui/scoreAfter.png",	// スコアの下の説明
	};

	const Vector3 c_aPosMenu[CResultMenu::TYPE_MAX] =		// 各メニューの座標
	{
		Vector3(780.0f, 600.0f, 0.0f),		// リプレイボタン
		Vector3(1040.0f, 600.0f, 0.0f)		// 終了ボタン
	};

	const Vector3 c_aPosScore[CResultMenu::SCORE_MAX] =		// 各スコア説明ポリゴンの座標
	{
		Vector3(700.0f, 115.0f, 0.0f),		// スコアの上の説明
		Vector3(1000.0f, 385.0f, 0.0f)		// スコアの下の説明
	};

	const Vector2 c_aDefScaleMenu[CResultMenu::TYPE_MAX] =	// 各メニューの基本サイズ
	{
		Vector2(150.0f, 150.0f),		// リプレイボタン
		Vector2(150.0f, 150.0f)			// 終了ボタン
	};

	const Vector2 c_aDefScaleScore[CResultMenu::SCORE_MAX] =	// 各スコア説明ポリゴンの基本サイズ
	{
		Vector2(300.0f, 90.0f),		// スコアの上の説明
		Vector2(500.0f, 90.0f),		// スコアの下の説明
	};

	const CPercent::Setting c_defSettingPercent =			// パーセント表示の基本設定
	{
		Vector3(675.0f, 250.0f, 0.0f),		// 開始位置
		Vector2(64.0f, 128.0f),				// 数字一桁のサイズ
		Vector2(64.0f, 128.0f),				// 小数点のサイズ
		Vector2(72.0f, 146.0f),				// パーセントのサイズ
		0.0f,								// パーセントの初期値
		CNumber::TYPE_VOLTNUM_003,			// 数値テクスチャの種類 
		INVALID_TEX_ID,						// テクスチャインデックス (静的に読み込めないため無効値)
		INVALID_TEX_ID,						// テクスチャインデックス (静的に読み込めないため無効値)
	};
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CResultMenu *CResultMenu::Create(const float fValue)
{
	CResultMenu *pResultMenu = new CResultMenu;		// 生成したオブジェクト
	if (pResultMenu != nullptr)
	{ // 生成出来ていれば初期化
		pResultMenu->Init(fValue);
	}

	return pResultMenu;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CResultMenu::CResultMenu() : CObject(DEFAULT_UI_PRIORITY)
{ // メンバ変数をクリア
	m_pPercent = nullptr;
	m_fValue = 0.0f;
	m_fCurrentValue = 0.0f;
	m_currentType = TYPE_REPLAY;

	// タイプ設定
	SetType(TYPE_RESULTMENU);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CResultMenu::~CResultMenu()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CResultMenu::Init(const float fValue)
{ // 引数を保存
	m_fValue = fValue;
	
	// 各バッファを生成
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャ管理へのポインタ

	for (int nCntMenu = 0; nCntMenu < TYPE_MAX; nCntMenu++)
	{ // 各メニューのポリゴンを作成
		m_apMenu[nCntMenu].reset(CPolygon2D::Create(c_aPosMenu[nCntMenu],
			VECTOR3_NULL,
			c_aDefScaleMenu[nCntMenu]));
		
		// テクスチャを指定
		m_apMenu[nCntMenu]->BindTexture(pTexture->Register(c_asPathMenu[nCntMenu]));
	}

	for (int nCntScore = 0; nCntScore < SCORE_MAX; nCntScore++)
	{ // 各スコア説明のポリゴンを作成
		m_apScore[nCntScore].reset(CPolygon2D::Create(c_aPosScore[nCntScore],
			VECTOR3_NULL,
			c_aDefScaleScore[nCntScore]));

		// テクスチャを指定
		m_apScore[nCntScore]->BindTexture(pTexture->Register(c_asPathScore[nCntScore]));
	}

	// パーセント表示の生成
	CPercent::Setting setting = c_defSettingPercent;		// 基本設定をコピー

	// テクスチャインデックスを変更
	setting.nIdxDotTexture = pTexture->Register(DOT_PATH);
	setting.nIdxPercentTexture = pTexture->Register(PERCENT_PATH);

	// 生成した設定で作成
	m_pPercent = CPercent::Create(setting);

	CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

	// パーセント上昇音を再生
	pSound->Play(CSound::LABEL_SE_PERCENT_UP);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CResultMenu::Uninit(void)
{
	for (auto &pPoly : m_apMenu)
	{ // 各メニューのポリゴンを破棄
		if (pPoly == nullptr) continue;
		pPoly->Uninit();
		pPoly.reset();
	}

	for (auto &pPoly : m_apScore)
	{ // 各スコア説明のポリゴンを破棄
		if (pPoly == nullptr) continue;
		pPoly->Uninit();
		pPoly.reset();
	}

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CResultMenu::Update(void)
{ 
	// 入力関連処理
	UpdateInput();

	if (m_fValue > m_fCurrentValue)
	{ // 現在のパーセントが指定値未満なら増加
		m_fCurrentValue += PERCENT_INCREASE;
		if (m_fCurrentValue > m_fValue)
		{ // 超えた場合は修正
			m_fCurrentValue = m_fValue;

			CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

			// パーセント上昇音を停止
			pSound->Stop(CSound::LABEL_SE_PERCENT_UP);
		}

		// パーセント表示の値を更新
		m_pPercent->SetValue(m_fCurrentValue);
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CResultMenu::Draw(void)
{
	for (auto &pPoly : m_apMenu)
	{ // 各メニューのポリゴンを描画
		if (pPoly == nullptr) continue;
		pPoly->Draw();
	}

	for (auto &pPoly : m_apScore)
	{ // 各スコア説明のポリゴンを描画
		if (pPoly == nullptr) continue;
		pPoly->Draw();
	}
}

//==================================================================================
// --- 入力関連の更新処理 ---
//==================================================================================
void CResultMenu::UpdateInput(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CInputMouse *pMouse = pManager->GetInputMouse();				// マウスへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();						// ジョイパッドへのポインタ
	CSound *pSound = pManager->GetSound();							// サウンドへのポインタ
	Vector3 mousePos = pMouse->GetPositionInClient();				// マウス座標
	
	// 既に遷移を開始している場合、スキップ
	if (pManager->GetState() != CSceneTransition::STATE_STAY) return;

	for (int nCntType = 0; nCntType < TYPE_MAX; nCntType++)
	{ // タイプ分繰り返し
		if (m_apMenu[nCntType]->Collision(mousePos))
		{ // 前回と違うものを選んでいたら
			if (m_currentType != nCntType)
			{ // セレクト音を流す
				pSound->Play(CSound::LABEL_SE_SELECT);
			}

			// 当たっていれば、現在のタイプを変更
			m_currentType = static_cast<TYPE>(nCntType);
		}
	}

	if (pKeyboard->GetTrigger(DIK_D)
		|| pJoypad->GetTrigger(CJoypad::KEY_RIGHT)
		|| pJoypad->GetStick(CJoypad::STICK_LEFT_LEFT))
	{ // ユーザーが右入力をした際、タイプを進める
		m_currentType = Util::AddEnum(m_currentType, 1);
		if (m_currentType >= TYPE_MAX) m_currentType = TYPE_REPLAY;

		// セレクト音を流す
		pSound->Play(CSound::LABEL_SE_SELECT);
	}
	else if (pKeyboard->GetTrigger(DIK_A)
		|| pJoypad->GetTrigger(CJoypad::KEY_LEFT)
		|| pJoypad->GetStick(CJoypad::STICK_LEFT_RIGHT))
	{ // ユーザーが左入力をした際、タイプを戻す
		m_currentType = Util::AddEnum(m_currentType, -1);
		if (m_currentType < TYPE_REPLAY) m_currentType = TYPE_EXIT;

		// セレクト音を流す
		pSound->Play(CSound::LABEL_SE_SELECT);
	}
	else if (pKeyboard->GetTrigger(DIK_RETURN)
		|| pJoypad->GetTrigger(CJoypad::KEY_A)
		|| (pMouse->GetTrigger(CInputMouse::KEY_LEFT) && m_apMenu[m_currentType]->Collision(mousePos)))
	{ // ユーザーが決定ボタンを押したとき (Enter || A || 左クリック)、今のタイプで処理実行
		ActionType();
	}

	for (int nCntType = 0; nCntType < TYPE_MAX; nCntType++)
	{ // タイプ分繰り返し
		if (nCntType == m_currentType)
		{ // 当たっていれば、サイズを拡大 + 現在のタイプを変更
			m_apMenu[nCntType]->SetSize(c_aDefScaleMenu[nCntType] * MENU_SCALE_MAGNI);
		}
		else
		{ // 当たっていなければ、サイズを戻す + タイプを無効値に設定
			m_apMenu[nCntType]->SetSize(c_aDefScaleMenu[nCntType]);
		}
	}
}

//==================================================================================
// --- タイプ別処理 ---
//==================================================================================
void CResultMenu::ActionType(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ

	// 現在のタイプで処理を分ける
	switch (m_currentType)
	{
	case TYPE_REPLAY:		// ゲーム開始 (チュートリアルスキップ)
		pManager->SetTransition(CScene::MODE_GAME);
		break;

	case TYPE_EXIT:			// タイトルへ戻る
		pManager->SetTransition(CScene::MODE_TITLE);
		break;

	default:
		break;
	}
}