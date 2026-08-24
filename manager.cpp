//==================================================================================
// 
// マネージャクラスのソースファイル [manager.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/12
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "manager.h"
#include "renderer.h"
#include "windowCapture.h"
#include "input.h"
#include "joypad.h"
#include "debugproc.h"
#include "sound.h"
#include "circleTransition.h"
#include "object.h"
#include "object2D.h"
#include "object3D.h"
#include "objectBillboard.h"
#include "objectX.h"
#include "camera.h"
#include "playerCamera.h"
#include "light.h"
#include "texture.h"
#include "xfile.h"
#include "player.h"
#include "utilityPole.h"
#include "electricCurrent.h"
#include "effect.h"
#include "meshField.h"
#include "planet.h"
#include "map.h"
#include "util.h"
#include "scene.h"
#include "meshSphere.h"
#include "motionLoader.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define CAPTURE_DIRECTLY		"data/SCREENSHOT"		// 画面キャプチャの画像のディレクトリパス

//==================================================================================
// --- マネージャの取得処理 ---
//==================================================================================
CManager *CManager::GetInstance(void)
{
	static CManager manager;		// インスタンス
	return &manager;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CManager::CManager()
{ // 各メンバ変数のクリア
	m_hWnd = nullptr;
	m_pRenderer = nullptr;
	m_pCapture = nullptr;
	m_pInputKeyboard = nullptr;
	m_pInputMouse = nullptr;
	m_pJoypad = nullptr;
	m_pDebugProc = nullptr;
	m_pSound = nullptr;
	m_pLight = nullptr;
	m_pTransition = nullptr;
	m_pScene = nullptr;
	m_nCountFPS = 0;
	m_nCounterFrame = 0;
	m_nNumPole = 0;
	m_bPause = false;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CManager::~CManager()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CManager::Init(const HINSTANCE hInstance, const HWND hWnd, const BOOL bWindow)
{
	HRESULT hr = S_OK;		// 各関数の結果

	if (hWnd == nullptr)
	{ // ウィンドウハンドルがNULLの場合、失敗
		return E_FAIL;
	}

	// ウィンドウハンドルを保存
	m_hWnd = hWnd;

	// レンダラーの生成
	if (m_pRenderer == nullptr)
	{ // レンダラーがNULLの場合
		// レンダラーを生成
		m_pRenderer = std::make_unique<CRenderer>();
		if (m_pRenderer == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "レンダラーの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// レンダラーの初期化
		hr = m_pRenderer->Init(hWnd, bWindow);
		if (FAILED(hr))
		{ // レンダラーの初期化失敗
			MessageBox(hWnd, "レンダラーの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// キャプチャオブジェクトの作成
	if (m_pCapture == nullptr)
	{ // もしまだ生成されていないなら
		// キャプチャオブジェクトを生成
		m_pCapture = std::make_unique<CWindowCapture>();
		if (m_pCapture == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "キャプチャオブジェクトの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// キャプチャオブジェクトの初期化
		hr = m_pCapture->Init(CAPTURE_DIRECTLY);
		if (FAILED(hr))
		{ // キーボードの初期化失敗
			MessageBox(hWnd, "キャプチャオブジェクトの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// キーボードの生成
	if (m_pInputKeyboard == nullptr)
	{ // もしまだ生成されていないなら
		// キーボードを生成
		m_pInputKeyboard = std::make_unique<CInputKeyboard>();
		if (m_pInputKeyboard == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "キーボードの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// キーボードの初期化
		hr = m_pInputKeyboard->Init(hInstance, hWnd);
		if (FAILED(hr))
		{ // キーボードの初期化失敗
			MessageBox(hWnd, "キーボードの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// マウスの生成
	if (m_pInputMouse == nullptr)
	{ // もしまだ生成されていないなら
		// マウスを生成
		m_pInputMouse = std::make_unique<CInputMouse>();
		if (m_pInputMouse == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "マウスの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// マウスの初期化
		hr = m_pInputMouse->Init(hInstance, hWnd);
		if (FAILED(hr))
		{ // マウスの初期化失敗
			MessageBox(hWnd, "マウスの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// ジョイパッドの生成
	if (m_pJoypad == nullptr)
	{ // もしまだ生成されていないなら
		// ジョイパッドを生成
		m_pJoypad = std::make_unique<CJoypad>();
		if (m_pJoypad == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "ジョイパッドの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// ジョイパッドの初期化
		m_pJoypad->Init();
	}

	// デバッグ表示の生成
	if (m_pDebugProc == nullptr)
	{ // もしまだ生成されていないなら
		// デバッグ表示を生成
		m_pDebugProc = std::make_unique<CDebugProc>();
		if (m_pDebugProc == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "デバッグ表示の生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// デバッグ表示の初期化
		hr = m_pDebugProc->Init(DEFAULT_SIZE, "PixelMplus12");
		if (FAILED(hr))
		{ // デバッグ表示の初期化失敗
			MessageBox(hWnd, "デバッグ表示の初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// 音声の生成
	if (m_pSound == nullptr)
	{ // もしまだ生成されていないなら
		// サウンドを生成
		m_pSound = std::make_unique<CSound>();
		if (m_pSound == nullptr)
		{ // 生成失敗
			MessageBox(hWnd, "サウンドの生成に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}

		// サウンドの初期化
		hr = m_pSound->Init(hWnd);
		if (FAILED(hr))
		{ // デバッグ表示の初期化失敗
			MessageBox(hWnd, "サウンドの初期化に失敗しました！", "Failed", MB_ICONERROR);
			return E_FAIL;
		}
	}

	// ライトの生成
	m_pLight = std::make_unique<CLight>();
	m_pLight->Init();

	// 遷移演出の作成
	m_pTransition = std::make_unique<CCircleTransition>();
	m_pTransition->Init();

	// テクスチャの読み込み
	CTexture::GetInstance()->Load();

	// 新規シーンの作成
	SetMode(CScene::MODE_TITLE);

	// 成功
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CManager::Uninit(void)
{
	// 全オブジェクトの破棄, 終了処理
	CObject::ReleaseAll();

	// 全カメラの破棄、終了処理
	CCamera::ReleaseAll();

	// テクスチャの破棄
	CTexture::GetInstance()->Unload();

	// Xファイルの破棄
	CXFile::GetInstance()->Unload();
	
	// モーション情報の破棄
	CMotionLoader::GetInstance()->Unload();

	// シーンオブジェクトの破棄
	if (m_pScene != nullptr)
	{ // NULLではなかった場合破棄
		m_pScene->Uninit();
		m_pScene.reset();
	}

	// 遷移オブジェクトの破棄
	if (m_pTransition != nullptr)
	{ // NULLではなかった場合破棄
		m_pTransition->Uninit();
		m_pTransition.reset();
	}

	// ライトオブジェクトの破棄
	if (m_pLight != nullptr)
	{ // NULLではなかった場合破棄
		m_pLight->Uninit();
		m_pLight.reset();
	}

	// サウンドオブジェクトの破棄
	if (m_pSound != nullptr)
	{ // NULLではなかった場合破棄
		m_pSound->Uninit();
		m_pSound.reset();
	}

	// デバッグ表示オブジェクトの破棄
	if (m_pDebugProc != nullptr)
	{ // NULLではなかった場合破棄
		m_pDebugProc->Uninit();
		m_pDebugProc.reset();
	}

	// ジョイパッドオブジェクトの破棄
	if (m_pJoypad != nullptr)
	{ // NULLではなかった場合破棄
		m_pJoypad->Uninit();
		m_pJoypad.reset();
	}

	// マウスオブジェクトの破棄
	if (m_pInputMouse != nullptr)
	{ // NULLではなかった場合破棄
		m_pInputMouse->Uninit();
		m_pInputMouse.reset();
	}

	// キーボードオブジェクトの破棄
	if (m_pInputKeyboard != nullptr)
	{ // NULLではなかった場合破棄
		m_pInputKeyboard->Uninit();
		m_pInputKeyboard.reset();
	}

	// キャプチャオブジェクトの破棄
	if (m_pCapture != nullptr)
	{ // NULLではなかった場合破棄
		m_pCapture->Uninit();
		m_pCapture.reset();
	}

	// レンダラーオブジェクトの破棄
	if (m_pRenderer != nullptr)
	{ // NULLではなかった場合破棄
		m_pRenderer->Uninit();
		m_pRenderer.reset();
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CManager::Update(void)
{
	// キーボードの更新処理
	m_pInputKeyboard->Update();

	// マウスの更新処理
	m_pInputMouse->Update();

	// ジョイパッドの更新処理
	m_pJoypad->Update();

	// デバッグ表示の更新処理
	m_pDebugProc->Update();
	
	// ライトの更新処理
	m_pLight->Update();

	// FPS表示
	m_pDebugProc->Print("FPS : {}\n", m_nCountFPS);

	if (m_pInputKeyboard->GetTrigger(DIK_O))
	{ // スクリーンショット
		m_pCapture->ScreenShot(D3DXIFF_JPG, "Test.jpg");
		m_pInputKeyboard->Update();
	}

	// シーンの更新
	m_pScene->Update();

	// トランジション更新
	m_pTransition->Update();

	if (m_bPause != true)
	{ // レンダラーの更新処理
		m_pRenderer->Update();
	}

	// フレームカウンターを増加
	m_nCounterFrame++;
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CManager::Draw(void)
{
	// シーンの描画
	m_pScene->Draw();

	// レンダラーの描画
	m_pRenderer->Draw();
}

//==================================================================================
// --- モード変更処理 ---
//==================================================================================
HRESULT CManager::SetMode(const CScene::MODE modeNext)
{
	// 全音源の再生の停止
	m_pSound->Stop();

	if (m_pScene != nullptr)
	{ // シーンが生成されていれば、終了処理
		m_pScene->Uninit();
		m_pScene.reset();
	}

	// 全オブジェクトの破棄
	CObject::ReleaseAll();

	// カメラの破棄
	CCamera::ReleaseAll();

	// シーンの生成
	CScene::Create(modeNext, m_pScene);
	NULLPOINTER_ASSERT(m_pScene);

	return (m_pScene == nullptr) ? E_FAIL : S_OK;
}

//==================================================================================
// --- トランジション設定処理 ---
//==================================================================================
void CManager::SetTransition(const CScene::MODE modeNext)
{
	m_pTransition->StartTransition(60, modeNext);
}