//==================================================================================
// 
// マネージャクラスのヘッダーファイル [manager.h]
// Author : TENMA SAITO
// Date   : 2026/5/12
// 
//==================================================================================
#ifndef _MANAGER_H_		// インクルードガード
#define _MANAGER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "scene.h"
#include <memory>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_APPLICATION_FRAMERATE		(120)		// デフォルトのフレームレート
#define ENABLE_PLANET		// 惑星モードの切り替え(定義されているとクォータニオンでの仕様)

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CRenderer;
class CWindowCapture;
class CInputKeyboard;
class CInputMouse;	
class CJoypad;		
class CDebugProc;	
class CSound;		
class CLight;
class CSceneTransition;

//**********************************************************************************
// *** マネージャクラス ***
//**********************************************************************************
class CManager
{
public:
	HRESULT Init(const HINSTANCE hInstance,
		const HWND hWnd,
		const BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPause(const bool bPause) { m_bPause = bPause; }
	bool GetPause(void) { return m_bPause; }
	void SetFrameRate(const int nFrameRate) { m_nFrameRate = nFrameRate; }
	int GetFrameRate(void) { return m_nFrameRate; }
	void SetFPS(const int nFPS) { m_nCountFPS = nFPS; }
	int GetFPS(void) { return m_nCountFPS; }
	void SetEnablePause(const bool bEnable) { m_bPause = bEnable; }
	bool GetEnablePause(void) { return m_bPause; }
	CRenderer *GetRenderer(void) const { return m_pRenderer.get(); }
	CWindowCapture *GetCapture(void) const { return m_pCapture.get(); }
	CInputKeyboard *GetInputKeyboard(void) const { return m_pInputKeyboard.get(); }
	CInputMouse *GetInputMouse(void) const { return m_pInputMouse.get(); }
	CJoypad *GetJoypad(void) const { return m_pJoypad.get(); }
	CDebugProc *GetDebugProc(void) const { return m_pDebugProc.get(); }
	CSound *GetSound(void) const { return m_pSound.get(); }
	CLight *GetLight(void) const { return m_pLight.get(); }
	CSceneTransition *GetTransition(void) const { return m_pTransition.get(); }
	HRESULT SetMode(const CScene::MODE modeNext);
	CScene::MODE GetMode(void) const { return m_pScene->GetMode(); }
	void SetTransition(const CScene::MODE modeNext);
	template<class T> T *GetScene(void) const;
	CScene *GetScene(void) const { return m_pScene.get(); }
	HWND GetWindowHandle(void) const { return m_hWnd; }

	static CManager *GetInstance(void);
	static LPDIRECT3DDEVICE9 GetDeviceByInstance(void);
	template<class T> static T *GetSceneByInstance(void);

private:
	CManager();
	~CManager();

	HWND m_hWnd;						// ウィンドウハンドル
	std::unique_ptr<CRenderer> m_pRenderer;
	std::unique_ptr<CWindowCapture> m_pCapture;			// 画面キャプチャオブジェクトへのポインタ
	std::unique_ptr<CInputKeyboard> m_pInputKeyboard;	// キーボードオブジェクトへのポインタ
	std::unique_ptr<CInputMouse> m_pInputMouse;			// マウスオブジェクトへのポインタ
	std::unique_ptr<CJoypad> m_pJoypad;					// ジョイパッドオブジェクトへのポインタ
	std::unique_ptr<CDebugProc> m_pDebugProc;			// デバッグ表示オブジェクトへのポインタ
	std::unique_ptr<CSound> m_pSound;					// サウンドオブジェクトへのポインタ
	std::unique_ptr<CLight> m_pLight;					// ライトオブジェクトへのポインタ
	std::unique_ptr<CScene> m_pScene;					// シーンへのポインタ
	std::unique_ptr<CSceneTransition> m_pTransition;	// 遷移演出へのポインタ
	int m_nFrameRate;					// フレームレート
	int m_nCountFPS;					// FPSカウント
	int m_nCounterFrame;				// フレームカウンター
	int m_nNumPole;						// ポールの数
	bool m_bPause;						// ポーズ状態
};

//==================================================================================
// --- キャスト後シーン取得処理 ---
//==================================================================================
template<class T> T *CManager::GetScene(void) const
{
	T *pScene = nullptr;		// キャスト先

	// シーンのポインタをキャスト
	pScene = static_cast<T*>(m_pScene.get());
	NULLPOINTER_ASSERT(pScene);

	return pScene;
}

//==================================================================================
// --- シーン取得処理省略版 ---
//==================================================================================
template<class T> T *CManager::GetSceneByInstance(void)
{
	T *pScene = nullptr;		// キャスト先

	// シーンのポインタをキャスト
	pScene = static_cast<T *>(CManager::GetInstance()->m_pScene.get());
	NULLPOINTER_ASSERT(pScene);

	return pScene;
}
#endif