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
#include "camera.h"
#include "input.h"
#include "joypad.h"
#include "texture.h"
#include "resultCamera.h"
#include "planet.h"
#include "map.h"
#include "filestream.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define CURRENT_SCORE_PATH	"data/SCORE/current.bin"		// ゲームシーン終了時のスコアを書き出すファイルパス

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CResult::CResult()
{ // メンバ変数をクリア
	m_pPlanet = nullptr;
	m_fPercent = 0.0f;
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

	if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(CJoypad::KEY_A))
	{ // シーン遷移
		pManager->SetTransition(CScene::MODE_TITLE);
	}

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
	m_pPlanet->MultiplyQuaternion(qua);

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
	CMap *pMap = CMap::GetInstance();		// マップへのポインタ

	// 惑星を生成
	m_pPlanet = CPlanet::Create();
	pMap->SetCurrentScenePlanet(m_pPlanet);

	// マップをリセット
	pMap->ReloadByConnectID();

	// 仮置きでカメラを生成 (自動解放)
	// TODO : ここのカメラはゲームオーバー用のカメラを作って置き換える事！
	CResultCamera *pResultCamera = CResultCamera::Create(Vector3(1500.0f, 0.0f, -4000.0f), Vector3(1500.0f, 0.0f, 0.0f));
	pResultCamera->SetFocus();

	std::unique_ptr pFile = std::make_unique<CFileStream>();		// ファイルストリームへのポインタ

	// 今回の結果をファイルから読み込み
	if (pFile->OpenFile(CURRENT_SCORE_PATH, true))
	{ // ファイル生成成功時
		// 割合を読み込み
		pFile->Read(m_fPercent);

		// ファイルを閉じる
		pFile->CloseFile();
	}
}