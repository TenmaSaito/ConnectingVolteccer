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
#include "sound.h"
#include "joypad.h"
#include "observer_pointer.h"
#include "util.h"
#include "sceneTransition.h"
#include <string_view>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MENU_SIZE		Vector2(400.0f, 75.0f)		// 各メニューのサイズ
#define MENU_ANGLE		(QUARTER_PI * 0.5f)			// 各メニューの角度
#define MENU_SELECT_SPD	(0.1f)						// メニュー選択時の速度
#define FIRST_ANGLE		((HALF_PI - QUARTER_PI * 0.5f) + (MENU_ANGLE * (CTitleMenu::TYPE_MAX / 2)))		// メニューの開始角度
#define LOGO_POSITION	Vector3(400.0f, 150.0f, 0.0f)	// ロゴの位置
#define LOGO_SIZE		Vector2(700.0f, 200.0f)			// ロゴのサイズ
#define LOGO_FILEPATH	"data/TEXTURE/title_ui/titlelogo.png"	// ロゴテクスチャへのパス

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asMenuPath[CTitleMenu::TYPE_MAX] =	// 各メニューのテクスチャパス
	{
		"data/TEXTURE/title_ui/start.png",		// STARTボタン
		"data/TEXTURE/title_ui/edit.png",		// EDITボタン
		"data/TEXTURE/title_ui/exit.png",		// 終了ボタン
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
	m_pCircle.reset(CPolygon2D::Create(Vector3(0.0f, SCREEN_MIDDLE.y, 0.0f),
		VECTOR3_NULL,
		Vector2(SCREEN_SIZE.y, SCREEN_SIZE.y)));

	// 色を設定
	m_pCircle->SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));

	// 円形テクスチャを登録
	m_pCircle->BindTexture(CTexture::TYPE_CIRCLE);

	// ロゴを生成
	m_pLogo.reset(CPolygon2D::Create(LOGO_POSITION, VECTOR3_NULL, LOGO_SIZE));
	m_pLogo->BindTexture(CTexture::GetInstance()->Register(LOGO_FILEPATH));

	for (UINT uCntPoly = 0U; uCntPoly < TYPE_MAX; uCntPoly++)
	{ // 各メニューのポリゴンを作成
		float fAngle = FIRST_ANGLE - (MENU_ANGLE * uCntPoly);		// 角度

		// ポリゴンの位置を計算
		Vector2 pos2 = Vec2::Arc(SCREEN_SIZE.y * 0.5f,
			fAngle,
			Vector2(0.0f, SCREEN_MIDDLE.y));

		// ポリゴンを生成
		m_apMenu.at(uCntPoly).reset(CPolygon2D::Create(Vec2::ToVector3(pos2),
			Vector3(0.0f, MENU_ANGLE * uCntPoly, 0.0f),
			MENU_SIZE));

		// 色を設定
		if (uCntPoly == 0U)
		{ // 一番最初のメニューのみ不透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{ // それ以外のメニューは半透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));
		}

		// テクスチャを登録
		m_apMenu.at(uCntPoly)->BindTexture(pTexture->Register(c_asMenuPath[uCntPoly]));
	}

	for (UINT uCntPoly = 0U; uCntPoly < TYPE_MAX; uCntPoly++)
	{ // 各メニューのポリゴンを更新
		float fAngle = FIRST_ANGLE - (MENU_ANGLE * uCntPoly)
			+ (MENU_ANGLE * m_fTypeDest);		// 角度

		// ポリゴンの位置を計算
		Vector2 pos2 = Vec2::Arc(SCREEN_SIZE.y * 0.75f,
			fAngle,
			Vector2(0.0f, SCREEN_MIDDLE.y));

		// ポリゴンの位置、角度を更新
		m_apMenu.at(uCntPoly)->SetPosition(Vec2::ToVector3(pos2));
		m_apMenu.at(uCntPoly)->SetRotation(Vector3(0.0f,
			-(MENU_ANGLE * uCntPoly) + (MENU_ANGLE * m_fTypeDest),
			0.0f));

		// 色を設定
		if (uCntPoly == 0U + m_currentType && m_lastType == m_currentType)
		{ // 一番最初のメニューのみ不透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{ // それ以外のメニューは半透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
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
		if (pPoly == nullptr) continue;
		pPoly->Uninit();
		pPoly.reset();
	}

	if (m_pCircle != nullptr)
	{ // 円背景ポリゴンの破棄
		m_pCircle->Uninit();
		m_pCircle.reset();
	}

	if (m_pLogo != nullptr)
	{ // ロゴポリゴンの破棄
		m_pLogo->Uninit();
		m_pLogo.reset();
	}

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
	CSound *pSound = pManager->GetSound();			// サウンドへのポインタ

	// 既に遷移を開始している場合、スキップ
	if (pManager->GetState() != CSceneTransition::STATE_STAY) return;

	if (m_lastType == m_currentType)
	{ // タイプ補間が完了している場合
		if (pKeyboard->GetTrigger(DIK_S)
			|| pJoypad->GetRepeat(CJoypad::KEY_DOWN)
			|| pJoypad->GetStick(CJoypad::STICK_LEFT_DOWN))
		{ // S入力時、タイプを1進める
			m_lastType = m_currentType;
			m_currentType = Util::AddEnum(m_currentType, 1);
			if (m_currentType >= TYPE_MAX) m_currentType = TYPE_START;

			// セレクト音を流す
			pSound->Play(CSound::LABEL_SE_SELECT);
		}
		else if (pKeyboard->GetTrigger(DIK_W)
			|| pJoypad->GetRepeat(CJoypad::KEY_UP)
			|| pJoypad->GetStick(CJoypad::STICK_LEFT_UP))
		{ // W入力時、タイプを1戻す
			m_lastType = m_currentType;
			m_currentType = Util::AddEnum(m_currentType, -1);
			if (m_currentType < TYPE_START) m_currentType = TYPE_EXIT;

			// セレクト音を流す
			pSound->Play(CSound::LABEL_SE_SELECT);
		}
		else if ((pKeyboard->GetTrigger(DIK_RETURN)
			|| pJoypad->GetTrigger(CJoypad::KEY_A))
			&& pManager->GetTransition()->GetState() == CSceneTransition::STATE_STAY)
		{ // ENTER入力時、現在のタイプの処理を実行
			ActionType();
		}
	}

	for (UINT uCntPoly = 0U; uCntPoly < TYPE_MAX; uCntPoly++)
	{ // 各メニューのポリゴンを更新
		float fAngle = FIRST_ANGLE - (MENU_ANGLE * uCntPoly)
			+ (MENU_ANGLE * m_fTypeDest);		// 角度

		// ポリゴンの位置を計算
		Vector2 pos2 = Vec2::Arc(SCREEN_SIZE.y * 0.75f,
			fAngle,
			Vector2(0.0f, SCREEN_MIDDLE.y));

		// ポリゴンの位置、角度を更新
		m_apMenu.at(uCntPoly)->SetPosition(Vec2::ToVector3(pos2));
		m_apMenu.at(uCntPoly)->SetRotation(Vector3(0.0f,
			-(MENU_ANGLE * uCntPoly) + (MENU_ANGLE * m_fTypeDest),
				0.0f));

		// 色を設定
		if (uCntPoly == 0U + m_currentType && m_lastType == m_currentType)
		{ // 一番最初のメニューのみ不透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{ // それ以外のメニューは半透明にする
			m_apMenu.at(uCntPoly)->SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
	}

	if (m_lastType != m_currentType)
	{ // 直前のタイプと現在のタイプが異なる場合
		m_fTime += MENU_SELECT_SPD;		// 線形補間用変数増加

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
{ 
	// 円形ポリゴンの描画
	m_pCircle->Draw();

	for (auto &pPoly : m_apMenu)
	{ // 各メニューの描画
		pPoly->Draw();
	}

	// ロゴの描画
	m_pLogo->Draw();
}

//==================================================================================
// --- Enter時のタイプ別処理 ---
//==================================================================================
void CTitleMenu::ActionType(void)
{
	own::ObserverPtr pManager(CManager::GetInstance());			// マネージャへのポインタ

	switch (m_currentType)
	{
	case TYPE_START:		// チュートリアル開始
		pManager->SetTransition(CScene::MODE_TUTORIAL);
		break;

	case TYPE_UNKNOWN:		// エディット開始
		pManager->SetTransition(CScene::MODE_EDIT);
		break;

	case TYPE_EXIT:			// exe終了
		PostMessage(pManager->GetWindowHandle(), WM_CLOSE, 0, 0);
		break;

	default:
		break;
	}
}