//==================================================================================
// 
// 建造物クラスのソースファイル [building.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/24
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "building.h"
#include "manager.h"
#include "game.h"
#include "planet.h"

//**********************************************************************************
// *** 静的メンバ変数 ***
//**********************************************************************************
const char *CBuilding::m_apBulidingPath[TYPE_MAX] =		// 建物モデルのパス
{
	"data/MODEL/house000.x",		// 建物0
	"data/MODEL/house001.x",		// 建物1
	"data/MODEL/house002.x",		// 建物2
	"data/MODEL/house003.x",		// 建物3
};

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CBuilding *CBuilding::Create(const TYPE type,
	const D3DXVECTOR3 &position,
	const D3DXVECTOR3 &vecQua,
	const float fAngle)
{ // 建物の生成
	CBuilding *pBuilding = nullptr;		// 建物へのポインタ

	switch (type)
	{ // 建物の種類で場合分け
	default:
		break;
	}

	NULLPOINTER_ASSERT(pBuilding);
	if (pBuilding != nullptr)
	{ // 生成に成功している場合、初期化
		pBuilding->Init(position, vecQua, fAngle);
	}

	return pBuilding;
}

//==================================================================================
// --- 生成処理 (任意軸と角度を現在の惑星から自動設定) ---
//==================================================================================
CBuilding *CBuilding::Create(const TYPE type, const D3DXVECTOR3 &position)
{ // 建物の生成
	CBuilding *pBuilding = nullptr;		// 建物へのポインタ

	switch (type)
	{ // 建物の種類で場合分け
	default:
		break;
	}

	NULLPOINTER_ASSERT(pBuilding);
	if (pBuilding != nullptr)
	{ // 生成に成功している場合、初期化
		pBuilding->Init(position);
	}

	return pBuilding;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CBuilding::CBuilding(const TYPE type, const int nPriority)
{ // メンバ変数のクリア
	m_pNearPole = nullptr;
	m_bHitByPlayerCamRay = false;
	m_buildingType = type;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CBuilding::~CBuilding()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CBuilding::Init(const D3DXVECTOR3 &position,
	const D3DXVECTOR3 &vecQua,
	const float fAngle)
{
	HRESULT hr = S_OK;		// 結果
	CGame *pGame = CManager::GetSceneByInstance<CGame>();		// ゲームへのポインタ
	CPlanet *pPlanet = pGame->GetPlanet();		// 惑星へのポインタ

	// 親クラスの初期化
	hr = CObjectXQuaternion::Init(m_apBulidingPath[m_buildingType], position, vecQua, fAngle);

	// 親を惑星に設定
	SetParent(pPlanet->GetMatrix());

	return S_OK;
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CBuilding::Init(const D3DXVECTOR3 &position)
{
	HRESULT hr = S_OK;		// 結果
	CGame *pGame = CManager::GetSceneByInstance<CGame>();		// ゲームへのポインタ
	CPlanet *pPlanet = pGame->GetPlanet();		// 惑星へのポインタ

	// 惑星から現在の任意軸と角度を取得
	D3DXVECTOR3 vecQua = VECTOR3_NULL;
	float fAngle = 0.0f;

	// クォータニオンから軸と角度を求める
	D3DXQuaternionToAxisAngle(pPlanet->GetQuaternion(),
		&vecQua,
		&fAngle);

	// 角度反転
	fAngle *= -1;

	// 親クラスの初期化
	hr = CObjectXQuaternion::Init(m_apBulidingPath[m_buildingType], position, vecQua, fAngle);

	// 親を惑星に設定
	SetParent(pPlanet->GetMatrix());

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CBuilding::Uninit(void)
{ // 親クラスの終了
	CObjectXQuaternion::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CBuilding::Update(void) 
{ // TODO : ここに将来電柱に電気が通ったらずんずん動く処理を書く
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CBuilding::Draw(void) 
{ // 親クラスの描画
	CObjectXQuaternion::Draw();
}