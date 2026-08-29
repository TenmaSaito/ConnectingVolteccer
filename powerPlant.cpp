//==================================================================================
// 
// 発電所クラスのソースファイル [powerPlant.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/22
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "powerPlant.h"
#include "manager.h"
#include "game.h"
#include "planet.h"
#include "utilityPole.h"
#include "electricalCable.h"
#include "electricCurrent.h"
#include "color.h"
#include <ranges>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MODEL_PATH		"data/MODEL/powerPlant.x"		// 発電所のモデルパス

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPowerPlant *CPowerPlant::Create(const Vector3 &pos,
	const Vector3 &vecQua,
	const float fAngle)
{
	CPowerPlant *pPlant = new CPowerPlant;		// 生成した発電所へのポインタ
	if (pPlant != nullptr)
	{ // 生成できている場合、初期化処理
		pPlant->Init(pos, vecQua, fAngle);
	}

	return pPlant;
}

//==================================================================================
// --- 生成処理 (任意軸と角度を現在の惑星から自動設定) ---
//==================================================================================
CPowerPlant *CPowerPlant::Create(const Vector3 &pos)
{
	CPowerPlant *pPlant = new CPowerPlant;		// 生成した発電所へのポインタ
	if (pPlant != nullptr)
	{ // 生成できている場合、初期化処理
		pPlant->Init(pos);
	}

	return pPlant;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPowerPlant::CPowerPlant(const int nPriority) : CObjectXQuaternion(nPriority)
{ // メンバ変数のクリア
	m_pCurrentPole = nullptr;
	m_pCurrentCable = nullptr;

	// タイプ設定
	SetType(CObject::TYPE_POWERPLANT);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPowerPlant::~CPowerPlant()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPowerPlant::Init(const Vector3 &position,
	const Vector3 &vecQua,
	const float fAngle)
{
	HRESULT hr = S_OK;		// 結果
	CGame *pGame = CManager::GetInstance()->GetScene(&pGame);		// ゲームへのポインタ
	CPlanet *pPlanet = pGame->GetPlanet();		// 惑星へのポインタ

	// 親クラスの初期化
	hr = CObjectXQuaternion::Init(MODEL_PATH, position, vecQua, fAngle);

	// 親を惑星に設定
	SetParent(pPlanet->GetMatrix());
	return hr;
}

//==================================================================================
// --- 初期化処理 (任意軸と角度を現在の惑星から自動設定) ---
//==================================================================================
HRESULT CPowerPlant::Init(const Vector3 &position)
{
	HRESULT hr = S_OK;		// 結果
	CGame *pGame = CManager::GetInstance()->GetScene(&pGame);		// ゲームへのポインタ
	CPlanet *pPlanet = pGame->GetPlanet();		// 惑星へのポインタ

	// 惑星から現在の任意軸と角度を取得
	Vector3 vecQua = VECTOR3_NULL;
	float fAngle = 0.0f;

	// クォータニオンから軸と角度を求める
	D3DXQuaternionToAxisAngle(pPlanet->GetQuaternion(),
		&vecQua,
		&fAngle);

	// 角度反転
	fAngle *= -1;

	// 親クラスの初期化
	hr = CObjectXQuaternion::Init(MODEL_PATH, position, vecQua, fAngle);

	// 親を惑星に設定
	SetParent(pPlanet->GetMatrix());
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPowerPlant::Uninit(void)
{ // 親クラスの終了
	CObjectXQuaternion::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPowerPlant::Update(void)
{ // 親クラスの更新
	CObjectXQuaternion::Update();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPowerPlant::Draw(void)
{ // 親クラスの描画
	CObjectXQuaternion::Draw();
}

//==================================================================================
// --- 電柱との接続処理 ---
//==================================================================================
bool CPowerPlant::Connect(CUtilityPole *pPole)
{ // nullの場合スキップ
	if (pPole == nullptr) return false;

	// 配列内に既に含まれていないか確認
	const auto result= std::ranges::find(m_vpPole, pPole);
	if (result != m_vpPole.cend())
	{ // 見つかった場合、接続済みの為スキップ
		return false;
	}

	// 電柱の接続処理呼び出し
	pPole->Connected(this);

	// 接続済みとしてポインタを保存
	m_vpPole.push_back(pPole);

	// 今回繋げた電柱としてポインタを保存
	m_pCurrentPole = pPole;

	// 電柱同士を電線で接続
	m_pCurrentCable = CElectricalCable::Create(this, pPole);
	m_pCurrentCable->SetParent(GetParent());
	return true;
}

//==================================================================================
// --- 今回繋げた電柱へ電流を流す処理 ---
//==================================================================================
void CPowerPlant::InvokeElectric(void)
{ // 今回繋げた電柱から電流を生成
	// 自身とつながっている電柱に電気を流す
	CElectricCurrent *pCurrect = CElectricCurrent::Create(this, m_pCurrentPole);
	pCurrect->SetParent(CManager::GetInstance()->GetScene<CGame>()->GetPlanet()->GetMatrix());

	// 電線の色を黄色に変更
	m_pCurrentCable->SetColor(Colors::GetColor(Colors::C_YELLOW));
}

//==================================================================================
// --- 今回繋げたのを取り消す処理 ---
//==================================================================================
void CPowerPlant::RemoveConnected(void)
{
	if (m_pCurrentCable != nullptr)
	{ // 繋げた電線があれば破棄
		m_pCurrentCable->Uninit();
		m_pCurrentCable = nullptr;
	}

	// 接続先のポインタが登録済みなら消す
	auto iter = std::ranges::find(m_vpPole, m_pCurrentPole);
	if(iter != m_vpPole.cend()) m_vpPole.erase(iter);

	// 接続先のポインタを手放す
	m_pCurrentPole = nullptr;
}