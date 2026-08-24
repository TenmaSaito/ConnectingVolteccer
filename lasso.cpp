//==================================================================================
// 
// 投げ縄クラスのソースファイル [lasso.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/10
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "lasso.h"
#include "utilityPole.h"
#include "powerPlant.h"
#include "vec3math.h"
#include "matrix.h"
#include "manager.h"
#include "game.h"
#include "player.h"
#include "planet.h"
#include "debugproc.h"
#include "effect.h"
#include "camera.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MODEL_PATH			"data/MODEL/lasso.x"		// 投げ縄モデルへのパス
#define DEF_SPEED			(-0.005f)		// 回転速度
#define COLLISION_LENGTH	(120.0f)		// 当たり判定をする長さ
#define DEF_LIFE			(30)			// 移動する時間

//==================================================================================
// --- 生成処理 (電柱 -> 電柱) ---
//==================================================================================
CLasso *CLasso::Create(const Vector3 &pos, 
	CUtilityPole *pStart,
	CUtilityPole *pEnd)
{
	CLasso *pLasso = new CLasso;		// 生成したオブジェクトへのポインタ
	if (pLasso != nullptr)
	{ // 生成出来ていれば初期化
		pLasso->Init(pos, pStart, pEnd);
	}

	return pLasso;
}

//==================================================================================
// --- 生成処理 (発電所 -> 電柱) ---
//==================================================================================
CLasso *CLasso::Create(const Vector3 &pos,
	CPowerPlant *pStart,
	CUtilityPole *pEnd)
{
	CLasso *pLasso = new CLasso;		// 生成したオブジェクトへのポインタ
	if (pLasso != nullptr)
	{ // 生成出来ていれば初期化
		pLasso->Init(pos, pStart, pEnd);
	}

	return pLasso;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CLasso::CLasso()
{ // メンバ変数のクリア
	m_pEnd = nullptr;
	m_fSlerpTime = 0.0f;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CLasso::~CLasso()
{
}

//==================================================================================
// --- 初期化処理 (クォータニオン指定) ---
//==================================================================================
HRESULT CLasso::Init(const Vector3 &pos,
	CUtilityPole *pStart,
	CUtilityPole *pEnd)
{ // 体力を設定
	// 各クォータニオンを保存
	m_pStart = pStart;
	m_pEnd = pEnd;

	// 親クラスの初期化
	return CObjectXQuaternion::Init(MODEL_PATH, pos, *pStart->GetQuaternion());
}

//==================================================================================
// --- 初期化処理 (クォータニオン指定) ---
//==================================================================================
HRESULT CLasso::Init(const Vector3 &pos,
	CPowerPlant *pStart,
	CUtilityPole *pEnd)
{ // 体力を設定
	// 各クォータニオンを保存
	m_pStart = pStart;
	m_pEnd = pEnd;

	// 親クラスの初期化
	return CObjectXQuaternion::Init(MODEL_PATH, pos, *pStart->GetQuaternion());
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CLasso::Uninit(void)
{ // カメラのフォーカスをプレイヤーに戻す
	CCamera::SetFocus(CCamera::TYPE_PLAYER);
	
	// 親クラスの破棄
	CObjectXQuaternion::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CLasso::Update(void)
{ // 位置と角度の更新
	UpdateTransform();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CLasso::Draw(void)
{ // 親クラスの描画
	CObjectXQuaternion::Draw();
}

//==================================================================================
// --- 位置及び角度更新処理 ---
//==================================================================================
void CLasso::UpdateTransform(void)
{
	Quaternion *pQuaLasso = GetQuaternionPtr();		// 現在のクォータニオン

	// 球面線形補間を行う
	D3DXQuaternionSlerp(pQuaLasso,
		std::visit([](auto &x) { return x->GetQuaternion(); }, m_pStart),
		m_pEnd->GetQuaternion(),
		m_fSlerpTime);

	// 球面線形補間の補間係数を進める
	m_fSlerpTime += (1.0f / static_cast<float>(DEF_LIFE));
	if (m_fSlerpTime > 1.0f)
	{ // 一定時間進んだら死亡
		std::visit<>([&](auto &x) { return x->Connect(m_pEnd); }, m_pStart);

		CGame *pGame = CManager::GetInstance()->GetScene(&pGame);
		CPlayer *pPlayer = pGame->GetPlayer();

		// 次に乗るべき電柱を設定
		pPlayer->ChangeRidingPole(m_pEnd);

		// 投げ縄を破棄
		Uninit();
	}
}