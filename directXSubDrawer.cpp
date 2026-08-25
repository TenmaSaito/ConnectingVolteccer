//==================================================================================
// 
// DirectXサブ描画クラスのソースファイル [directXSubDrawer.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "directXSubDrawer.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "joypad.h"
#include "debugproc.h"
#include "sound.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define TIME_PERIOD			(1)						// 分解能の値

//==================================================================================
// --- DirectXサブ描画スレッドのプロシージャ処理 ---
//==================================================================================
void CDirectXSubDrawer::DirectXSubDrawerProc(void)
{
	CManager *pManager = nullptr;		// マネージャへのポインタ
	DWORD dwCurrentTime;			// 現在時刻
	DWORD dwExecLastTimeUpdate;		// 最後に処理した時刻
	DWORD dwExecLastTimeDraw;		// 最後に処理した時刻
	DWORD dwFrameCount;				// フレームカウント
	DWORD dwFPSLastTime;			// 最後にFPSを計測した時刻

	// ループフラグを立てる
	m_bThreadLoop = true;

	// マネージャを取得
	pManager = CManager::GetInstance();

	// FPS計測用変数を初期化
	dwCurrentTime = 0;							// 初期化
	dwExecLastTimeUpdate = timeGetTime();		// 現在時刻を取得
	dwExecLastTimeDraw = timeGetTime();			// 現在時刻を取得
	dwFrameCount = 0;							// 初期化
	dwFPSLastTime = timeGetTime();				// 現在時刻を取得

	// 分解能を設定
	timeBeginPeriod(TIME_PERIOD);

	// 初期化
	Init();

	while (m_bThreadLoop)
	{ // スレッドループ
		{ // DirectXの処理
			dwCurrentTime = timeGetTime();				// 現在時刻を取得
			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{// 0.5秒経過
				// FPSを計測
				int nFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);
				pManager->SetFPS(nFPS);

				dwFPSLastTime = dwCurrentTime;			// FPSを計測した時刻を取得
				dwFrameCount = 0;						// フレームカウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTimeUpdate) >= (1000 / DEFAULT_APPLICATION_UPDATE_PER_SECOND))
			{ // 60分の1秒経過
				dwExecLastTimeUpdate = dwCurrentTime;			// 処理開始時刻[現在時刻]を保存

				// 更新処理
				Update();
			}

			if ((dwCurrentTime - dwExecLastTimeDraw) >= static_cast<DWORD>(1000 / DEFAULT_APPLICATION_DRAW_PER_SECOND))
			{ // 1 / フレームレート秒経過
				dwExecLastTimeDraw = dwCurrentTime;			// 処理開始時刻[現在時刻]を保存

				// 描画処理
				Draw();

				dwFrameCount++;							// フレームカウントを加算
			}
		}
	}

	// 終了
	Uninit();

	// 分解能を解除
	timeEndPeriod(TIME_PERIOD);
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CDirectXSubDrawer::CDirectXSubDrawer()
{ // メンバ変数をクリア
	ZeroMemory(m_aCallFunction, sizeof(m_aCallFunction));
	m_bThreadLoop = false;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CDirectXSubDrawer::~CDirectXSubDrawer()
{
}

//==================================================================================
// --- 各関数の設定 ---
//==================================================================================
void CDirectXSubDrawer::FunctionSetUp(const FUNCTION funcType, hyp::Action<> func)
{
	m_aFunc[funcType] = func;
	m_aCallFunction[funcType] = true;
}

//==================================================================================
// --- DirectXサブ描画スレッドの終了処理 ---
//==================================================================================
void CDirectXSubDrawer::Quit(void)
{ // フラグ終了
	m_bThreadLoop = false;
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CDirectXSubDrawer::Init(void)
{
	if (m_aCallFunction[FUNCTION_INIT])
	{
		m_aFunc[FUNCTION_INIT].InvokeAll();
	}

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CDirectXSubDrawer::Uninit(void)
{
	if (m_aCallFunction[FUNCTION_UNINIT])
	{
		m_aFunc[FUNCTION_UNINIT].InvokeAll();
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CDirectXSubDrawer::Update(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ

	// 各マネージャーのアップデート処理
	pManager->GetInputKeyboard()->Update();
	pManager->GetInputMouse()->Update();
	pManager->GetJoypad()->Update();

	if (m_aCallFunction[FUNCTION_UPDATE])
	{
		m_aFunc[FUNCTION_UPDATE].InvokeAll();
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CDirectXSubDrawer::Draw(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();		// レンダラーへのポインタ
	CDebugProc *pProc = pManager->GetDebugProc();		// デバッグ表示へのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスへのポインタ

	// 画面クリア(バックバッファとZバッファのクリア)
	pDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// 描画開始
	if (SUCCEEDED(pDevice->BeginScene()))
	{ // 描画開始が成功した場合
		if (m_aCallFunction[FUNCTION_DRAW])
		{
			m_aFunc[FUNCTION_DRAW].InvokeAll();
		}

		// デバッグ表示へのポインタ
		pProc->Draw();

		// 描画終了
		pDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	pDevice->Present(NULL, NULL, NULL, NULL);
}