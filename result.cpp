//==================================================================================
// 
// リザルトクラスのソースファイル [result.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/14
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "result.h"
#include "object.h"
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "camera.h"
#include "input.h"
#include "joypad.h"
#include "texture.h"
#include "resultCamera.h"
#include "planet.h"
#include "resultmenu.h"
#include "mapManager.h"
#include "filestream.h"
#include "number.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define CURRENT_SCORE_PATH	"data/SCORE/current.bin"		// ゲームシーン終了時のスコアを書き出すファイルパス
#define SCORE_POS			(Vector3(700.0f, 300.0f, 0.0f))	// スコアの開始座標

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CResult::CResult()
{ // メンバ変数をクリア
	m_pPlanet = nullptr;
	m_pMenu = nullptr;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CResult::~CResult()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CResult::Init(void)
{ // 開始処理
	Start();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CResult::Uninit(void)
{
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CResult::Update(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();			// ジョイパッドへのポインタ
	CMapManager *pMap = CMapManager::GetInstance();		// マップへのポインタ

	// 惑星を回転
	Vector3 vecQua = Vector3(0.0f, D3DX_PI, 0.0f);		// 軸
	float fAngle = 0.005f;	// 回転度数
	Quaternion qua;			// かけ合わせるクォータニオン

	// クォータニオンを計算
	D3DXQuaternionIdentity(&qua);
	D3DXQuaternionRotationAxis(&qua,
		&vecQua,
		fAngle);

	// 惑星に掛ける
	pMap->GetPlanet()->MultiplyQuaternion(qua);

	// 全カメラの更新処理
	CCamera::UpdateAll();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CResult::Draw(void)
{
}

//==================================================================================
// --- 初期化後呼び出し処理 ---
//==================================================================================
void CResult::Start(void)
{ 
	CMapManager *pMap = CMapManager::GetInstance();		// マップへのポインタ

	// 今回の接続インデックスを読み込み
	pMap->LoadConnectID();

	// リザルト画面カメラを生成
	CResultCamera *pResultCamera = CResultCamera::Create(Vector3(1500.0f, 0.0f, -3800.0f), Vector3(1500.0f, 0.0f, 0.0f));
	pResultCamera->SetFocus();

	std::unique_ptr pFile = std::make_unique<CFileStream>();		// ファイルストリームへのポインタ
	float fPercent = 0.0f;		// 今回の結果

	// 今回の結果をファイルから読み込み
	if (pFile->OpenFile(CURRENT_SCORE_PATH, true))
	{ // ファイル生成成功時
		// 割合を読み込み
		pFile->Read(fPercent);

		// ファイルを閉じる
		pFile->CloseFile();
	}

	// リザルト用メニューを生成
	m_pMenu = CResultMenu::Create(fPercent);

	CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

	// タイトル画面のBGMを流す
	pSound->Play(CSound::LABEL_BGM_RESULT);
}