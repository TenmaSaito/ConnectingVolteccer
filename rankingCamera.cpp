//==================================================================================
// 
// ランキングカメラクラスのソースファイル [rankingCamera.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/10
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "rankingCamera.h"
#include "manager.h"
#include "input.h"
#include "joypad.h"
#include "vec3math.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_FOVY		(45.0f)		// 視野角
#define DEFAULT_ZN			(1.0f)		// 最短距離
#define DEFAULT_ZF			(10000.0f)	// 最遠距離
#define DEFAULT_SPD			(1.0f)		// 移動速度
#define DEFAULT_ROTSPD		(0.03f)		// 回転速度
#define DEFAULT_LENGTH		(1000.0f)	// 注視点との距離

//==================================================================================
// --- カメラの生成 ---
//==================================================================================
CRankingCamera *CRankingCamera::Create(const Vector3 &posV, const Vector3 &posR)
{
	CRankingCamera *pRankingCamera = nullptr;		// 生成したカメラへのポインタ

	// 自身のタイプのカメラが既に存在しているか確認
	pRankingCamera = static_cast<CRankingCamera*>(CCamera::GetCamera(TYPE_RANKING));
	if (pRankingCamera == nullptr)
	{ // 存在していなかった場合は、新規作成
		pRankingCamera = new CRankingCamera;
	}

	// 初期化処理
	pRankingCamera->Init(posV, posR);
	return pRankingCamera;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CRankingCamera::CRankingCamera() : CCamera(TYPE_RANKING)
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CRankingCamera::~CRankingCamera()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
void CRankingCamera::Init(const Vector3 &posV, const Vector3 &posR)
{ // 親クラスの初期化
	CCamera::Init(posV, posR);

	// カメラの最遠描画距離を変更
	SetFar(DEFAULT_ZF);
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CRankingCamera::Uninit(void)
{ // カメラの解放
	CCamera::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CRankingCamera::Update(void)
{
	CManager *pManager = CManager::GetInstance();	// マネージャへのポインタ
	auto pKeyboard = pManager->GetInputKeyboard();	// キーボードへのポインタ
	auto pJoypad = pManager->GetJoypad();			// ジョイパッドへのポインタ
	Vector3 posV = *CCamera::GetPosV();		// 視点座標
	Vector3 posR = *CCamera::GetPosR();		// 注視点座標
	Vector3 rot = *CCamera::GetRotate();	// 角度
	Vector3 stick;		// スティックの入力

	// 位置と角度を適用
	CCamera::SetPosV(posV);
	CCamera::SetPosR(posR);
	CCamera::SetRotate(rot);
}

//==================================================================================
// --- カメラの設置処理 ---
//==================================================================================
void CRankingCamera::SetCamera(void)
{ // カメラを設置
	CCamera::SetCamera();
}