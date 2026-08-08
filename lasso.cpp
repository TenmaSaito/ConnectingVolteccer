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
#define DEF_LIFE			(60)			// 移動する時間

//==================================================================================
// --- 生成処理 (クォータニオン指定) ---
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
// --- コンストラクタ ---
//==================================================================================
CLasso::CLasso()
{ // メンバ変数のクリア
	m_pStart = nullptr;
	m_pEnd = nullptr;
	m_fSlerpTime = 0.0f;
	m_pRidingPole = nullptr;
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

	// 乗っている電柱を保存
	m_pRidingPole = CManager::GetInstance()->GetScene<CGame>()->GetPlayer()->GetRidingPole();

	// 親クラスの初期化
	return CObjectXQuaternion::Init(MODEL_PATH, pos, *m_pStart->GetQuaternion());
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
{ 
	// 位置と角度の更新
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
		m_pStart->GetQuaternion(),
		m_pEnd->GetQuaternion(),
		m_fSlerpTime);

	// 球面線形補間の補間係数を進める
	m_fSlerpTime += (1.0f / static_cast<float>(DEF_LIFE));
	if (m_fSlerpTime > 1.0f)
	{ // 一定時間進んだら死亡
		m_pStart->Connect(m_pEnd);

		CPlayer *pPlayer = CManager::GetSceneByInstance<CGame>()->GetPlayer();

		// 次に乗るべき電柱を設定
		pPlayer->ChangeRidingPole(m_pEnd);

		Uninit();
	}
}

//==================================================================================
// --- 当たり判定処理 ---
//==================================================================================
void CLasso::Collision(void)
{
	float fLengthMin = COLLISION_LENGTH;							// 現状最も近い電柱との距離
	CObject *pObject = CObject::GetTop(UTILITYPOLE_PRIORITY);		// 先頭ポインタ
	CUtilityPole *pPoleNear = nullptr;		// 最も近い電柱へのポインタ

	// 投げ縄の現在位置表示
	Vector3 pos;						// マトリックスのキャスト用
	D3DXVec3TransformCoord(&pos, GetPosition(), GetMatrix());

	while (pObject != nullptr)
	{ // 電柱走査ループ
		CObject *pObjectNext = pObject->GetNext();		// 次のオブジェクトへのポインタ

		if (pObject->GetType() == CObject::TYPE_POLE)
		{ // もしオブジェクトが電柱であれば、ポインタをキャスト
			CUtilityPole* pPole = static_cast<CUtilityPole*>(pObject);

			Vector3 posPole;			// マトリックスのキャスト用

			// 各座標をマトリックスでワールド座標に変換
			D3DXVec3TransformCoord(&posPole, pPole->GetPosition(), pPole->GetMatrix());

			float fLength = Vec3::Length(posPole, pos);
			if (fLength < fLengthMin && pPole != m_pRidingPole)
			{ // もし前回の距離よりも近いなら、ポインタ保存 + 距離更新
				fLengthMin = fLength;
				pPoleNear = pPole;
			}
		}

		pObject = pObjectNext;		// ポインタ更新
	}

	if (pPoleNear != nullptr)
	{ // 電柱と当たったら、プレイヤーの乗っている電柱と繋げる
		m_pRidingPole->Connect(pPoleNear);

		// 投げ縄は破棄する
		Uninit();
	}
}