//==================================================================================
// 
// ポーズ画面クラスのソースファイル [pause.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/3
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "pause.h"
#include "texture.h"
#include "polygon2D.h"
#include "vec3math.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "joypad.h"
#include "sceneTransition.h"
#include "util.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_SIZE		(Vector2(200.0f, 200.0f))		// メニューの基本サイズ
#define MENU_SCALE_MAGNI	(1.5f)			// カーソルと当たった時のサイズ拡大倍率
#define BACK_COLOR			(Color(0.0f, 0.0f, 0.0f, 0.5f))	// ポーズ中の背景ポリゴンの色

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asPausePath[CPause::TYPE_MAX] =
	{ // ポーズ画面の各メニューのテクスチャパス
		"data/TEXTURE/pause_ui/continue.png",		// 続行
		"data/TEXTURE/pause_ui/replay.png",			// やり直し
		"data/TEXTURE/pause_ui/exit.png",			// 終了
	};

	const Vector3 c_aPosMenu[CPause::TYPE_MAX] =
	{ // ポーズ画面の各メニューのポリゴンの座標
		Vector3(240.0f, 320.0f, 0.0f),
		Vector3(640.0f, 320.0f, 0.0f),
		Vector3(1040.0f, 320.0f, 0.0f),
	};
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPause *CPause::Create(void)
{
	CPause *pPause = new CPause;		// 生成したポーズへのポインタ
	if (pPause != nullptr)
	{ // 生成されていれば初期化
		pPause->Init();
	}

	return pPause;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPause::CPause()
{ // メンバ変数のクリア
	m_bDisp = false;
	m_type = TYPE_CONTINUE;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPause::~CPause()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPause::Init(void)
{
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャ管理オブジェクトへのポインタ

	for (int nCntPoly = 0; nCntPoly < TYPE_MAX; nCntPoly++)
	{ // 各テクスチャを読み込み
		// 各メニューのポリゴンを作成
		m_apMenu[nCntPoly].reset(CPolygon2D::Create(c_aPosMenu[nCntPoly], VECTOR3_NULL, DEFAULT_SIZE));

		// テクスチャを紐づけ
		m_apMenu[nCntPoly]->BindTexture(pTexture->Register(c_asPausePath[nCntPoly]));
	}

	// 背景ポリゴンを生成
	m_pBack.reset(CPolygon2D::Create(SCREEN_MIDDLE, VECTOR3_NULL, SCREEN_SIZE));
	m_pBack->SetColor(BACK_COLOR);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPause::Uninit(void)
{
	if (m_pBack != nullptr)
	{ // ポリゴンを破棄 (nullならスキップ)
		m_pBack->Uninit();
		m_pBack.reset();
	}

	for (auto &pPoly : m_apMenu)
	{ // ポリゴンを破棄 (nullならスキップ)
		if (pPoly == nullptr) continue;

		pPoly->Uninit();
		pPoly.reset();
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPause::Update(void)
{ 
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ

	// 描画フラグが立っていない、もしくは既にモード移行が始まっているならスキップ
	if (m_bDisp != true || pManager->GetState() != CSceneTransition::STATE_STAY) return;

	// 入力関連の更新
	UpdateInput();

	for (int nCntType = 0; nCntType < TYPE_MAX; nCntType++)
	{ // タイプ分繰り返し
		if (nCntType == m_type)
		{ // 当たっていれば、サイズを拡大 + 現在のタイプを変更
			m_apMenu[nCntType]->SetSize(DEFAULT_SIZE * MENU_SCALE_MAGNI);
		}
		else
		{ // 当たっていなければ、サイズを戻す + タイプを無効値に設定
			m_apMenu[nCntType]->SetSize(DEFAULT_SIZE);
		}
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPause::Draw(void)
{ // 描画フラグが降りている場合、スキップ
	if (m_bDisp != true) return;

	// 背景ポリゴンを先に描画
	if(m_pBack != nullptr) m_pBack->Draw();

	for (auto &pPoly : m_apMenu)
	{ // ポリゴンを描画 (nullならスキップ)
		if (pPoly == nullptr) continue;

		pPoly->Draw();
	}
}

//==================================================================================
// --- 入力関連の更新処理 ---
//==================================================================================
void CPause::UpdateInput(void)
{
	CManager *pManager = CManager::GetInstance();					// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CInputMouse *pMouse = pManager->GetInputMouse();				// マウスへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();						// ジョイパッドへのポインタ
	CSound *pSound = pManager->GetSound();				// サウンドへのポインタ
	Vector3 mousePos = pMouse->GetPositionInClient();	// マウス座標

	for (int nCntType = 0; nCntType < TYPE_MAX; nCntType++)
	{ // タイプ分繰り返し
		if (m_apMenu[nCntType]->Collision(mousePos))
		{ // 前回と違うものを選んでいたら
			if (m_type != nCntType)
			{ // セレクト音を流す
				pSound->Play(CSound::LABEL_SE_SELECT);
			}
			
			// 当たっていれば、現在のタイプを変更
			m_type = static_cast<TYPE>(nCntType);
		}
	}

	// タイプが範囲になった場合、アサーション
	assert(m_type < TYPE_MAX || m_type >= TYPE_MAX);

	if (pKeyboard->GetTrigger(DIK_D)
		|| pJoypad->GetTrigger(CJoypad::KEY_RIGHT)
		|| pJoypad->GetStick(CJoypad::STICK_LEFT_LEFT))
	{ // ユーザーが右入力をした際、タイプを進める
		m_type = Util::AddEnum(m_type, 1);
		if (m_type >= TYPE_MAX) m_type = TYPE_CONTINUE;

		// セレクト音を流す
		pSound->Play(CSound::LABEL_SE_SELECT);
	}
	else if (pKeyboard->GetTrigger(DIK_A)
		|| pJoypad->GetTrigger(CJoypad::KEY_LEFT)
		|| pJoypad->GetStick(CJoypad::STICK_LEFT_RIGHT))
	{ // ユーザーが左入力をした際、タイプを戻す
		m_type = Util::AddEnum(m_type, -1);
		if (m_type < TYPE_CONTINUE) m_type = TYPE_EXIT;

		// セレクト音を流す
		pSound->Play(CSound::LABEL_SE_SELECT);
	}
	else if (pKeyboard->GetTrigger(DIK_RETURN)
		|| pJoypad->GetTrigger(CJoypad::KEY_A)
		|| (pMouse->GetTrigger(CInputMouse::KEY_LEFT) && m_apMenu[m_type]->Collision(mousePos)))
	{ // ユーザーが決定ボタンを押したとき (Enter || A || 左クリック)、今のタイプで処理実行
		ActionType();
	}
}

//==================================================================================
// --- タイプ別処理 ---
//==================================================================================
void CPause::ActionType(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ

	// 現在のタイプで処理を分ける
	switch (m_type)
	{
	case TYPE_CONTINUE:		// ゲーム再開 (ポーズ解除)
		m_bDisp = false;	// 描画フラグを下ろす
		break;

	case TYPE_RESTART:		// リスタート (現在のシーン読み込み直し)
		pManager->SetTransition(pManager->GetMode());
		break;

	case TYPE_EXIT:			// タイトルへ戻る
		pManager->SetTransition(CScene::MODE_TITLE);
		break;

	default:
		break;
	}
}