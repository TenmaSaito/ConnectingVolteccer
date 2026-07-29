//==================================================================================
// 
// シーンクラスのソースファイル [scene.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/24
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene.h"
#include "title.h"
#include "game.h"
#include "gameover.h"
#include "gameclear.h"

//==================================================================================
// --- モードの設定処理 ---
//==================================================================================
CScene *CScene::Create(const MODE mode)
{
	CScene *pScene = nullptr;		// 生成したシーン

	// モード別にインスタンス生成
	switch (mode)
	{
	case MODE_TITLE:		// タイトルシーンを生成
		pScene = new CTitle;
		break;

	case MODE_GAME:			// ゲームシーンを生成
		pScene = new CGame;
		break;

	case MODE_GAMEOVER:		// ゲームオーバーシーンを生成
		pScene = new CGameOver;
		break;

	case MODE_GAMECLEAR:		// ゲームクリアシーンを生成
		pScene = new CGameClear;
		break;
	}

	if (pScene != nullptr)
	{ // 生成成功時、シーンの初期化
		pScene->Init();
	}

	return pScene;
}

//==================================================================================
// --- モードの設定処理 (pOutに代入後初期化) ---
//==================================================================================
CScene *CScene::Create(const MODE mode, CScene **ppOut)
{
	CScene *pScene = nullptr;		// 生成したシーン

	// モード別にインスタンス生成
	switch (mode)
	{
	case MODE_TITLE:		// タイトルシーンを生成
		pScene = new CTitle;
		break;

	case MODE_GAME:			// ゲームシーンを生成
		pScene = new CGame;
		break;

	case MODE_GAMEOVER:		// ゲームオーバーシーンを生成
		pScene = new CGameOver;
		break;

	case MODE_GAMECLEAR:		// ゲームクリアシーンを生成
		pScene = new CGameClear;
		break;
	}

	if (ppOut != nullptr)
	{ // nullptrでは無ければ代入
		*ppOut = pScene;
	}

	if (pScene != nullptr)
	{ // 生成成功時、シーンの初期化
		pScene->Init();
	}

	return pScene;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CScene::CScene(const MODE mode)
{ // メンバ変数のクリア
	m_mode = mode;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CScene::~CScene()
{
}