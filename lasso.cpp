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
#include "electricalCable.h"
#include "powerPlant.h"
#include "vec2math.h"
#include "vec3math.h"
#include "matrix.h"
#include "manager.h"
#include "game.h"
#include "player.h"
#include "planet.h"
#include "combo.h"
#include "debugproc.h"
#include "effect.h"
#include "camera.h"
#include "ray.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MODEL_PATH			"data/MODEL/lasso.x"		// 投げ縄モデルへのパス
#define DEF_SPEED			(-0.005f)		// 回転速度
#define COLLISION_LENGTH	(120.0f)		// 当たり判定をする長さ
#define DEF_LIFE			(30)			// 移動する時間
#define CABLE_COLLISION_EPSILON		(0.01f)	// 電線との当たり判定を行うまでの猶予

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
	CheckCollision();
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
{ // 死んでいたら無効
	if (IsDeath() == true) return;

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
		std::visit([&](auto &x) { return x->Connect(m_pEnd); }, m_pStart);

		CGame *pGame = CManager::GetInstance()->GetScene(&pGame);
		CPlayer *pPlayer = pGame->GetPlayer();

		// 次に乗るべき電柱を設定
		pPlayer->ChangeRidingPole(m_pEnd);

		// 投げ縄を破棄
		Uninit();
	}
}

//==================================================================================
// --- 電線との当たり判定処理 ---
//==================================================================================
void CLasso::CheckCollision(void)
{ // 死んでいたら無効
	if (IsDeath() == true) return;

	CObject *pObject = CObject::GetTop(CABLE_PRIORITY);		// 最初のオブジェクト
	Vector3 pos = VECTOR3_NULL;			// 自身の絶対座標

	// 自身の絶対座標を求める
	D3DXVec3TransformCoord(&pos, &pos, GetMatrix());

	CEffect::Create(pos, 2, 20);

	while (pObject != nullptr)
	{ // オブジェクトを走査
		CObject *pObjectNext = pObject->GetNext();			// 次のオブジェクトへのポインタ
		if (pObject->GetType() != CObject::TYPE_CABLE)
		{ // もしオブジェクトが電線で無ければスキップ
			pObject = pObjectNext;		// ポインタ更新
			continue;
		}

		CElectricalCable *pCable = nullptr;		// 電線へのポインタ

		// ポインタを電線のポインタにキャスト
		pCable = static_cast<CElectricalCable*>(pObject);

		// 各頂点の座標を取得
		std::array<Vector3, DEFAULT_VERTEX_NUM> aVtxPos = pCable->GetVtxPosition();
		Vector3 posLocal = VECTOR3_NULL;		// ローカル座標
		Vector3 posOldLocal = VECTOR3_NULL;		// ローカル座標
		Matrix mtxInv;							// 逆行列

		// 電線のマトリックスの逆行列を求める
		Mtx::Inverse(&mtxInv, *pCable->GetMatrix());

		// 自身の各絶対座標を、電線のローカル座標に変換
		D3DXVec3TransformCoord(&posLocal, &pos, &mtxInv);
		D3DXVec3TransformCoord(&posOldLocal, &m_posOld, &mtxInv);

		if (m_posOld.y <= aVtxPos.at(0).y && m_posOld.y <= aVtxPos.at(2).y)
		{ // 前フレームのローカル座標の高さが電線以下ならスキップ
			pObject = pObjectNext;		// ポインタ更新
			continue;
		}

		Vector3 vecLine = VECTOR3_NULL;			// 境界線ベクトル
		Vector3 vecMove = VECTOR3_NULL;			// 移動ベクトル
		Vector3 vecToPos = VECTOR3_NULL;		// 始点から現在座標へのベクトル
		Vector3 vecToPosOld = VECTOR3_NULL;		// 始点から過去座標へのベクトル
		float fVecPos = 0.0f;
		float fVecPosOld = 0.0f;
		float fPosToMove = 0.0f;				// vecToPosとの外積
		float fLineToMove = 0.0f;				// vecLineとの外積
		float fRate = 0.0f;						// 面積比率

		// 境界線ベクトルを求める
		vecLine = aVtxPos.at(2) - aVtxPos.at(0);

		// 移動ベクトルを求める
		vecMove = posLocal - posOldLocal;

		// 始点からのベクトルをそれぞれ求める
		vecToPos = posLocal - aVtxPos.at(0);
		vecToPosOld = posOldLocal - aVtxPos.at(0);

		// 各ベクトルから外積を求める
		fVecPos = (vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z);
		fVecPosOld = (vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z);

		// 現在位置との外積を計算
		fPosToMove = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);

		// 最大値との外積を計算
		fLineToMove = (vecLine.z * vecMove.x) - (vecLine.x * vecMove.z);

		if (((fVecPos <= 0 && fVecPosOld >= 0)
			|| (fVecPos >= 0 && fVecPosOld <= 0)) != true)
		{ // 無限長のベクトルを横切っていないなら、スキップ
			pObject = pObjectNext;		// ポインタ更新
			continue;
		}

		if (fabsf(fLineToMove) <= FLT_EPSILON)
		{ // 外積の値が誤差なら、スキップ
			pObject = pObjectNext;		// ポインタ更新
			continue;
		}

		// 面積比率を計算
		fRate = fPosToMove / fLineToMove;
		if (fRate < CABLE_COLLISION_EPSILON || fRate > 1.0f)
		{ // 面積比率が当たり判定の猶予範囲もしくは範囲外なら、スキップ
			pObject = pObjectNext;		// ポインタ更新
			continue;
		}

		CGame *pGame = CManager::GetInstance()->GetScene(&pGame);		// ゲームシーンへのポインタ
		CPlayer *pPlayer = pGame->GetPlayer();		// プレイヤーへのポインタ

		// プレイヤーに失敗を伝える
		pPlayer->FailedShot();

		// 投げ縄を破棄
		Uninit();

		// プレイヤーがおろされるため判定は不要
		break;
	}

	// 今回の絶対座標を保存
	m_posOld = pos;
}