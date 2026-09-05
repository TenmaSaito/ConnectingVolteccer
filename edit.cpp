//==================================================================================
// 
// エディットリアルクラスのソースファイル [edit.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/2
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "edit.h"
#include "manager.h"
#include "input.h"
#include "joypad.h"
#include "debugproc.h"
#include "mapManager.h"
#include "player.h"
#include "planet.h"
#include "building.h"
#include "filestream.h"
#include "camera.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define PLAYER_MOTION_PATH	"data/SCRIPT/motion_player.txt"	// プレイヤーのモーションパス


//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CEdit::CEdit()
{ // メンバ変数のクリア
	m_pPlayer = nullptr;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CEdit::~CEdit()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CEdit::Init(void)
{ // 開始処理
	Start();
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CEdit::Uninit(void)
{
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CEdit::Update(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();			// ジョイパッドへのポインタ
	CDebugProc *pProc = pManager->GetDebugProc();		// デバッグ表示へのポインタ

	if (pKeyboard->GetTrigger(DIK_P) || pJoypad->GetTrigger(CJoypad::KEY_START))
	{ // ポーズ変更！
		pManager->SetEnablePause(!pManager->GetEnablePause());
	}

	if (pKeyboard->GetPress(DIK_LSHIFT))
	{ // 押し間違え防止
		if (pKeyboard->GetTrigger(DIK_R))
		{ // リロード
			pManager->SetMode(CScene::MODE_EDIT);
			return;
		}
		else if(pKeyboard->GetTrigger(DIK_E))
		{ // タイトル画面へ戻る
			pManager->SetMode(CScene::MODE_TITLE);
			return;
		}
		else if (pKeyboard->GetTrigger(DIK_S))
		{ // マップ保存
			time_t t = {};					// 時刻データ
			struct tm *pTime = nullptr;		// 時刻ポインタ
			char aTime[1024] = {};			// 時刻取得用文字列
			std::string filePath;			// ファイル名

			// 時刻を取得し文字列変換
			t = time(NULL);
			pTime = localtime(&t);
			strftime(aTime, sizeof(aTime), "%Y-%m-%d-%H-%M-%S", pTime);

			// ファイル名作成
			filePath = "data/Maps/";
			filePath += aTime;
			filePath += ".bin";

			// マップを保存
			CMapManager::GetInstance()->Save(filePath.c_str(), false);
		}
	}

	// デバッグ表示
	Print();

	// マップ関連のエディット
	MapEdit();

	// 全カメラの更新処理
	CCamera::UpdateAll();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CEdit::Draw(void)
{
}

//==================================================================================
// --- 初期化後呼び出し処理 ---
//==================================================================================
void CEdit::Start(void)
{
	CMapManager *pMap = CMapManager::GetInstance();		// マップへのポインタ
	CPlanet *pPlanet = nullptr;		// 惑星へのポインタ

	// プレイヤー出現
	m_pPlayer = CPlayer::Create(PLAYER_MOTION_PATH, Vector3(0.0f, 1115.0f, 0.0f), VECTOR3_NULL);
	pMap->BindPlayer(m_pPlayer);

	// マップ読み込み + 惑星へのポインタを取得
	pMap->Load("");
}

//==================================================================================
// --- エディット内のprint関数の呼び出し処理 ---
//==================================================================================
void CEdit::Print(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CDebugProc *pProc = pManager->GetDebugProc();		// デバッグ表示へのポインタ

	// 操作方法を表示
	pProc->Print("[操作方法 : 配置]\n");
	pProc->Print("  1～4 : 各建物の配置\n");
	pProc->Print("  5 : 電柱の配置\n");
	pProc->Print("  6 : 発電所の配置\n\n");

	pProc->Print("[操作方法 : データ]\n");
	pProc->Print("  SHIFT + I : ファイル書き出し\n");
	pProc->Print("  SHIFT + R : マップリセット\n");
	pProc->Print("  SHIFT + E : タイトル画面へ戻る\n\n");

	// 現在の建物の数を取得
	pProc->Print("[建物の総数 : {}]\n", CMapManager::GetInstance()->GetNumBuilding());
	pProc->Print("[電柱の総数 : {}]\n", CMapManager::GetInstance()->GetNumPole());
	pProc->Print("[発電所の総数 : {}]\n", CMapManager::GetInstance()->GetNumPowerPlant());
}

//==================================================================================
// --- マップのエディット処理 ---
//==================================================================================
void CEdit::MapEdit(void)
{
	CManager *pManager = CManager::GetInstance();					// マネージャーへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CMapManager *pMap = CMapManager::GetInstance();		// マップへのポインタ
	CPlanet *pPlanet = pMap->GetPlanet();				// 惑星へのポインタ
	Vector3 pos = Vector3(0.0f, pPlanet->GetVtxMax().y, 0.0f);	// 設置位置

	if (pKeyboard->GetTrigger(DIK_1))
	{ // 建物0生成
		pMap->AddBulding(CBuilding::TYPE_0, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_2))
	{ // 建物1生成
		pMap->AddBulding(CBuilding::TYPE_1, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_3))
	{ // 建物2生成
		pMap->AddBulding(CBuilding::TYPE_2, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_4))
	{ // 建物3生成
		pMap->AddBulding(CBuilding::TYPE_3, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_5))
	{ // 電柱生成
		pMap->AddUtilityPole(pos);
	}
	else if (pKeyboard->GetTrigger(DIK_6))
	{ // 発電所生成
		pMap->AddPowerPlant(pos);
	}
}