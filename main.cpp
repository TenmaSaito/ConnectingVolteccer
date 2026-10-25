//==================================================================================
// 
// DX9の復習！！ [main.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/3
// 
//==================================================================================
#include "main.h"
#include "directX.h"
#include <thread>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define CLASS_NAME			"AppClass"				// ウィンドウクラスの名前
#define WINDOW_NAME			"Connecting VoltTackle"	// キャプションに表示される名前
#define ENABLE_WINDOW		(TRUE)		// ウィンドウフラグ

//**********************************************************************************
// *** プロトタイプ宣言 ***
//**********************************************************************************
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//==================================================================================
// --- メイン関数 ---
//==================================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lpCmdLine, int nCmdShow)
{
	
#ifdef _DEBUG
	// メモリリーク検知用のフラグを立てる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);	
#endif

	int i;
	class A;
	HWND hWnd;					// ウィンドウハンドル
	MSG msg = {};				// メッセージを格納する変数
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };	// ウィンドウサイズ
	WNDCLASSEX wcex =								// ウィンドウ作成時の情報
	{
		sizeof(WNDCLASSEX),						// ウィンドウクラスのメモリサイズ
		CS_CLASSDC,								// ウィンドウのスタイル
		WindowProc,								// ウィンドウプロシージャ
		0,										// 0
		0,										// 0
		hInstance,								// インスタンスハンドル
		LoadIcon(NULL,IDI_APPLICATION),			// タスクバーのアイコン
		LoadCursor(NULL,IDC_ARROW),				// マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),				// クライアント領域の背景色
		NULL,									// メニューバー
		CLASS_NAME,								// ウィンドウクラスの名前
		NULL									// ファイルのアイコン
	};

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウの生成
	hWnd = CreateWindowEx(
		0,									// 拡張ウィンドウスタイル
		CLASS_NAME,							// ウィンドウクラスの名前
		WINDOW_NAME,						// ウィンドウの名前
		WS_OVERLAPPEDWINDOW,				// ウィンドウスタイル
		CW_USEDEFAULT,						// ウィンドウの左上X座標
		CW_USEDEFAULT,						// ウィンドウの左上Y座標
		(rect.right - rect.left),			// ウィンドウの幅
		(rect.bottom - rect.top),			// ウィンドウの高さ
		NULL,								// 親ウィンドウのハンドル
		NULL,								// メニュー(もしくは子ウィンドウ)ハンドル
		hInstance,							// インスタンスハンドル
		NULL);								// ウィンドウ作成データ

	// IMEを無効化 (マルチスレッドだとここが原因でコード1を吐く)
	ImmDisableIME(0);

	// DirectXの処理を別スレッドにて開始
	std::unique_ptr pDirectX = std::make_unique<CDirectX>();
	std::thread DirectXThread(&CDirectX::DirectXProc, pDirectX.get(), hInstance, hWnd, ENABLE_WINDOW, nCmdShow);

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{ // Windowsの処理
			if (msg.message == WM_QUIT)
			{ // WM_QUITメッセージを受けると、メッセージループを抜ける
				break;
			}
			else
			{ // メッセージの設定
				TranslateMessage(&msg);					// 仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);					// ウィンドウプロシージャへメッセージを送出
			}
		}
	}

	// DirectXスレッドに終了するよう通知
	pDirectX->Quit();

	// DirectXスレッド終了を待機
	DirectXThread.join();

	// ウィンドウクラスの登録解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);
	return (int)msg.wParam;
}

//==================================================================================
// --- ウィンドウプロシージャ ---
//==================================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// メッセージによって処理を分岐
	switch (uMsg)
	{
	case WM_DESTROY:			// ウィンドウ破棄のメッセージ
		// WM_QUITメッセージを送る
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:			// キー押下のメッセージ
		// 押されたキーの判定
		switch (wParam)
		{
		case VK_ESCAPE:			// ESCだった場合
			int nRet = MessageBox(hWnd, "終了しますか？", "確認", MB_ICONINFORMATION | MB_YESNO);

			// 本当に終了するか確認
			if (nRet == IDYES)
			{ // ウィンドウを破棄する(WM_DESTROYメッセージを送る)
				DestroyWindow(hWnd);
			}
			break;
		}

		break;

	case WM_CLOSE:				// 閉じるボタン押下のメッセージ
		// ウィンドウを破棄する(WM_DESTROYメッセージを送る)
		DestroyWindow(hWnd);
		break;

	case WM_SUCCEEDED_INIT:		// 初期化成功
		// ウィンドウの表示
		ShowWindow(hWnd, wParam);				// ウィンドウの表示状態を設定
		UpdateWindow(hWnd);						// クライアント領域を更新
		break;

	case WM_FAILED_INIT:		// 初期化失敗
		// ウィンドウの破棄
		DestroyWindow(hWnd);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}