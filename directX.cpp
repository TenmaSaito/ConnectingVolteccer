//==================================================================================
// 
// DirectXクラスのソースファイル [directX.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "directX.h"
#include "manager.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define TIME_PERIOD			(1)						// 分解能の値

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CDirectX::CDirectX()
{ // メンバ変数をクリア
	m_bThreadLoop = false;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CDirectX::~CDirectX()
{
}

//==================================================================================
// --- DirectXのプロシージャ処理 ---
//==================================================================================
void CDirectX::DirectXProc(HINSTANCE hInstance, HWND hWnd, BOOL bWindowed)
{
	CManager *pManager = nullptr;		// マネージャへのポインタ
	DWORD dwCurrentTime;			// 現在時刻
	DWORD dwExecLastTimeUpdate;		// 最後にUpdateした時刻
	DWORD dwExecLastTimeDraw;		// 最後にDrawした時刻
	DWORD dwFrameCount;				// フレームカウント
	DWORD dwFPSLastTime;			// 最後にFPSを計測した時刻

	// ループフラグを立てる
	m_bThreadLoop = true;

	// マネージャを生成
	pManager = CManager::GetInstance();

	// マネージャーを初期化
	if (SUCCEEDED(pManager->Init(hInstance, hWnd, bWindowed)))
	{ // 成功時
		// FPS計測用変数を初期化
		dwCurrentTime = 0;							// 初期化
		dwExecLastTimeUpdate = timeGetTime();		// 現在時刻を取得
		dwExecLastTimeDraw = timeGetTime();			// 現在時刻を取得
		dwFrameCount = 0;							// 初期化
		dwFPSLastTime = timeGetTime();				// 現在時刻を取得

		// 分解能を設定
		timeBeginPeriod(TIME_PERIOD);

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

				if ((dwCurrentTime - dwExecLastTimeUpdate) >= static_cast<DWORD>(1000 / DEFAULT_APPLICATION_UPDATE_PER_SECOND))
				{ // 60分の1秒経過
					dwExecLastTimeUpdate = dwCurrentTime;		// 処理開始時刻[現在時刻]を保存

					// マネージャの更新処理
					pManager->Update();
				}

				if ((dwCurrentTime - dwExecLastTimeDraw) >= static_cast<DWORD>(1000 / DEFAULT_APPLICATION_DRAW_PER_SECOND))
				{ // 1 / フレームレート秒経過
					dwExecLastTimeDraw = dwCurrentTime;			// 処理開始時刻[現在時刻]を保存

					// マネージャの描画処理
					pManager->Draw();

					dwFrameCount++;								// フレームカウントを加算
				}
			}
		}

		// 分解能を解除
		timeEndPeriod(TIME_PERIOD);
	}
	else
	{ // 初期化失敗時
		MessageBox(hWnd, "DirectXの初期化に失敗しました...", "DirectX Initialize Failed", MB_ICONERROR);
	}

	// マネージャの終了処理
	pManager->Uninit();
}

//==================================================================================
// --- DirectXの終了処理 ---
//==================================================================================
void CDirectX::Quit(void)
{ // フラグ終了
	m_bThreadLoop = false;
}