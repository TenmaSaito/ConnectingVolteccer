//==================================================================================
// 
// 電流追従カメラクラスのソースファイル [thunderCamera.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/29
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "thunderCamera.h"
#include "manager.h"
#include "game.h"
#include "planet.h"
#include "electricCurrent.h"
#include "ray.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CThunderCamera *CThunderCamera::Create(const float fLength)
{
	CThunderCamera *pThunderCam = nullptr;		// 生成したカメラへのポインタ

	// 自身のタイプのカメラが既に存在しているか確認
	pThunderCam = static_cast<CThunderCamera*>(CCamera::GetCamera(TYPE_THUNDER));
	if (pThunderCam == nullptr)
	{ // 存在していなかった場合は、新規作成
		pThunderCam = new CThunderCamera;
	}

	// 初期化処理
	pThunderCam->Init(fLength);

	return pThunderCam;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CThunderCamera::CThunderCamera() : CCamera(TYPE_THUNDER)
{ // メンバ変数のクリア
	m_pTarget = nullptr;
	m_fLength = 0.0f;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CThunderCamera::~CThunderCamera()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CThunderCamera::Init(const float fLength)
{ // 引数を保存
	m_fLength = fLength;

	// カメラを初期化
	CCamera::Init(VECTOR3_NULL, VECTOR3_NULL);
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CThunderCamera::Uninit(void)
{ // カメラの終了
	CCamera::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CThunderCamera::Update(void)
{ // nullの場合スキップ
	if (m_pTarget == nullptr) return;
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CGame *pGame = pManager->GetScene(&pGame);			// ゲームシーンへのポインタ
	CPlanet *pPlanet = pGame->GetPlanet();			// 惑星へのポインタ
	Vector3 posPlanet = *pPlanet->GetPosition();	// 惑星の座標
	Vector3 posV = VECTOR3_NULL;		// 視点
	Vector3 posR = VECTOR3_NULL;		// 注視点

	// 注視点を求める
	D3DXVec3TransformCoord(&posR, &posR, m_pTarget->GetMatrix());

	// 惑星から注視点へのレイを求める
	CRay ray(posPlanet, posR);		// 惑星から注視点へのレイ

	// 長さを指定し、終点を求める
	ray.SetLength(m_fLength + ray.GetLength());
	posV = *ray.GetEnd();

	// 視点・注視点
	CCamera::SetPosV(posV);
	CCamera::SetPosR(posR);
}

//==================================================================================
// --- カメラの設置処理 ---
//==================================================================================
void CThunderCamera::SetCamera(void)
{ // カメラを設置
	CCamera::SetCamera();
}

//==================================================================================
// --- ターゲットの変更処理 ---
//==================================================================================
void CThunderCamera::ChangeTarget(const CElectricCurrent *pTarget)
{ // 引数を保存
	m_pTarget = pTarget;
	if (pTarget == nullptr)
	{ // カメラのフォーカスを自動的にプレイヤーカメラへ変更
		CCamera::SetFocus(TYPE_PLAYER);
	}
	else
	{ // カメラのフォーカスを自動的に電流用カメラへ変更
		CCamera::SetFocus(TYPE_THUNDER);
	}
}