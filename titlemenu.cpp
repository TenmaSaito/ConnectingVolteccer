//==================================================================================
// 
// タイトル画面のメニュークラスのソースファイル [titlemenu.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/20
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "titlemenu.h"
#include "polygon2D.h"
#include "texture.h"
#include "vec2math.h"
#include "manager.h"
#include "input.h"
#include "joypad.h"
#include "observer_pointer.h"
#include "util.h"
#include <string_view>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MENU_SIZE		Vector2(250.0f, 75.0f)		// 各メニューのサイズ
#define MENU_ANGLE		(QUARTER_PI * 0.5f)			// 各メニューの角度
#define MENU_SELECT_SPD	(0.05f)						// メニュー選択時の速度
#define FIRST_ANGLE		((HALF_PI - QUARTER_PI * 0.5f) + (MENU_ANGLE * (CTitleMenu::TYPE_MAX / 2)))		// メニューの開始角度

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asMenuPath[CTitleMenu::TYPE_MAX] =	// 各メニューのテクスチャパス
	{
		"data/TEXTURE/MENU/.png",		// STARTボタン
		"data/TEXTURE/MENU/.png",		// STARTボタン
		"data/TEXTURE/MENU/.png",		// 終了ボタン
	};
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CTitleMenu *CTitleMenu::Create(void)
{
	CTitleMenu *pTitleMenu = new CTitleMenu;		// 生成したオブジェクト
	if (pTitleMenu != nullptr)
	{ // 生成出来ていれば初期化
		pTitleMenu->Init();
	}

	return pTitleMenu;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTitleMenu::CTitleMenu() : CObject(DEFAULT_OBJ_PRIORITY)
{ // タイプ設定
	SetType(TYPE_TITLEMENU);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CTitleMenu::~CTitleMenu()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTitleMenu::Init(void)
{ // 各バッファを生成
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャ管理へのポインタ

	// 円形の背景を生成
	m_pCircle.reset(CPolygon2D::Create(Vector3(0.0f, WINDOW_MIDDLE.y, 0.0f),
		VECTOR3_NULL,
		Vector2(WINDOW_SIZE.y, WINDOW_SIZE.y)));

	// 色を設定
	m_pCircle->SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));

	// 円形テクスチャを登録
	m_pCircle->BindTexture(CTexture::TYPE_CIRCLE);

	for (UINT uCntPoly = 0U; uCntPoly < TYPE_MAX; uCntPoly++)
	{ // 各メニューのポリゴンを作成
		float fAngle = FIRST_ANGLE - (MENU_ANGLE * uCntPoly);		// 角度

		// ポリゴンの位置を計算
		Vector2 pos2 = Vec2::Arc(WINDOW_SIZE.y * 0.5f,
			fAngle,
			Vector2(0.0f, WINDOW_MIDDLE.y));

		// ポリゴンを生成
		m_apMenu.at(uCntPoly).reset(CPolygon2D::Create(Vec2::ToVector3(pos2),
			Vector3(0.0f, MENU_ANGLE * uCntPoly, 0.0f),
			MENU_SIZE));

		// 色を設定
		if (uCntPoly == 0U)
		{ // 一番最初のメニューのみ不透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 0.0f, 1.0f, 1.0f));
		}
		else
		{ // それ以外のメニューは半透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 0.0f, 1.0f, 0.5f));
		}

		// テクスチャを登録
		m_apMenu.at(uCntPoly)->BindTexture(pTexture->Register(c_asMenuPath[uCntPoly]));
	}

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CTitleMenu::Uninit(void)
{ // 各バッファを解放
	for (auto &pPoly : m_apMenu)
	{ // ポリゴンの破棄
		pPoly->Uninit();
		pPoly.reset();
	}

	// ポリゴンの破棄
	m_pCircle->Uninit();
	m_pCircle.reset();

	// オブジェクト解放
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CTitleMenu::Update(void)
{
	own::ObserverPtr pManager(CManager::GetInstance());			// マネージャへのポインタ
	own::ObserverPtr pKeyboard(pManager->GetInputKeyboard());	// キーボードへのポインタ
	own::ObserverPtr pJoypad(pManager->GetJoypad());			// ジョイパッドへのポインタ

	if (m_lastType == m_currentType)
	{ // タイプ補間が完了している場合
		if (pKeyboard->GetTrigger(DIK_S))
		{ // S入力時
			m_lastType = m_currentType;
			m_currentType = Util::AddEnum(m_currentType, 1);
			if (m_currentType >= TYPE_MAX) m_currentType = TYPE_START;
		}
		else if (pKeyboard->GetTrigger(DIK_W))
		{ // W入力時
			m_lastType = m_currentType;
			m_currentType = Util::AddEnum(m_currentType, -1);
			if (m_currentType < TYPE_START) m_currentType = TYPE_EXIT;
		}
		else if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(CJoypad::KEY_A))
		{ // ENTER入力時
			ActionType();
		}
	}

	for (UINT uCntPoly = 0U; uCntPoly < TYPE_MAX; uCntPoly++)
	{ // 各メニューのポリゴンを更新
		float fAngle = FIRST_ANGLE - (MENU_ANGLE * uCntPoly)
			+ (MENU_ANGLE * m_fTypeDest);		// 角度

		// ポリゴンの位置を計算
		Vector2 pos2 = Vec2::Arc(WINDOW_SIZE.y * 0.5f,
			fAngle,
			Vector2(0.0f, WINDOW_MIDDLE.y));

		// ポリゴンの位置、角度を更新
		m_apMenu.at(uCntPoly)->SetPosition(Vec2::ToVector3(pos2));
		m_apMenu.at(uCntPoly)->SetRotation(Vector3(0.0f,
			-(MENU_ANGLE * uCntPoly) + (MENU_ANGLE * m_fTypeDest),
				0.0f));

		// 色を設定
		if (uCntPoly == 0U + m_currentType && m_lastType == m_currentType)
		{ // 一番最初のメニューのみ不透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 0.0f, 1.0f, 1.0f));
		}
		else
		{ // それ以外のメニューは半透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 0.0f, 1.0f, 0.5f));
		}
	}

	if (m_lastType != m_currentType)
	{ // 直前のタイプと現在のタイプが異なる場合
		m_fTime += 0.05f;		// 線形補間用変数増加

		// 現在の進行度を取得
		m_fTypeDest = Util::Lerp(static_cast<float>(m_lastType), static_cast<float>(m_currentType), m_fTime);
		if (m_fTime >= 1.0f)
		{ // 完全に補間が終了した場合
			m_lastType = m_currentType;		// 直前のタイプを現在のタイプに変更
			m_fTime = 0.0f;			// 線形補間用変数をリセット
		}
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CTitleMenu::Draw(void)
{ // 円形ポリゴンの描画
	m_pCircle->Draw();

	for (auto &pPoly : m_apMenu)
	{ // 各メニューの描画
		pPoly->Draw();
	}
}

//==================================================================================
// --- Enter時のタイプ別処理 ---
//==================================================================================
void CTitleMenu::ActionType(void)
{
	own::ObserverPtr pManager(CManager::GetInstance());			// マネージャへのポインタ

	switch (m_currentType)
	{
	case TYPE_START:		// ゲーム開始
		pManager->SetTransition(CScene::MODE_GAME);
		break;

	case TYPE_UNKNOWN:		// 未設定

		break;

	case TYPE_EXIT:			// exe終了
		PostMessage(pManager->GetWindowHandle(), WM_CLOSE, 0, 0);
		break;

	default:
		break;
	}
}