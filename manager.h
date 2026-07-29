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

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_APPLICATION_FRAMERATE		(120)		// デフォルトのフレームレート
#define ENABLE_PLANET		// 惑星モードの切り替え(定義されているとクォータニオンでの仕様)
//#undef ENABLE_PLANET

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
	CRenderer *GetRenderer(void) const { return m_pRenderer; }
	CWindowCapture *GetCapture(void) const { return m_pCapture; }
	CInputKeyboard *GetInputKeyboard(void) const { return m_pInputKeyboard; }
	CInputMouse *GetInputMouse(void) const { return m_pInputMouse; }
	CJoypad *GetJoypad(void) const { return m_pJoypad; }
	CDebugProc *GetDebugProc(void) const { return m_pDebugProc; }
	CSound *GetSound(void) const { return m_pSound; }
	CLight *GetLight(void) const { return m_pLight; }
	CSceneTransition *GetTransition(void) const { return m_pTransition; }
	HRESULT SetMode(const CScene::MODE modeNext);
	CScene::MODE GetMode(void) const { return m_pScene->GetMode(); }
	void SetTransition(const CScene::MODE modeNext);
	template<class T> inline T *GetScene(void) const;
	CScene *GetScene(void) const { return m_pScene; }
	HWND GetWindowHandle(void) const { return m_hWnd; }

	static CManager *GetInstance(void);
	static LPDIRECT3DDEVICE9 GetDeviceByInstance(void);

private:
	CManager();
	~CManager();

	HWND m_hWnd;						// ウィンドウハンドル
	CRenderer *m_pRenderer;				// レンダラーオブジェクトへのポインタ
	CWindowCapture *m_pCapture;			// 画面キャプチャオブジェクトへのポインタ
	CInputKeyboard *m_pInputKeyboard;	// キーボードオブジェクトへのポインタ
	CInputMouse *m_pInputMouse;			// マウスオブジェクトへのポインタ
	CJoypad *m_pJoypad;					// ジョイパッドオブジェクトへのポインタ
	CDebugProc *m_pDebugProc;			// デバッグ表示オブジェクトへのポインタ
	CSound *m_pSound;					// サウンドオブジェクトへのポインタ
	CLight *m_pLight;					// ライトオブジェクトへのポインタ
	CScene *m_pScene;					// シーンへのポインタ
	CSceneTransition *m_pTransition;	// 遷移演出へのポインタ
	int m_nFrameRate;					// フレームレート
	int m_nCountFPS;					// FPSカウント
	int m_nCounterFrame;				// フレームカウンター
	int m_nNumPole;						// ポールの数
	bool m_bPause;						// ポーズ状態
};

//==================================================================================
// --- キャスト後シーン取得処理 ---
//==================================================================================
template<class T> inline T *CManager::GetScene(void) const
{
	T *pScene = nullptr;		// キャスト先

	// シーンのポインタをキャスト
	pScene = static_cast<T*>(m_pScene);
	NULLPOINTER_ASSERT(pScene);

	return pScene;
}
#endif