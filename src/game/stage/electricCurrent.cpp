//==================================================================================
// 
// 電流クラスのヘッダーファイル [electricCurrent.h]
// Author : TENMA SAITO
// Date   : 2026/6/10
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/stage/electricCurrent.h"
#include "effect/thunderEffect.h"
#include "game/stage/utilityPole.h"
#include "game/stage/planet.h"
#include "effect/effect.h"
#include "core/manager.h"
#include "math/vec3math.h"
#include "math/vec2math.h"
#include "math/matrix.h"
#include "input/joypad.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define THUNDER_MAX		(Vector3(40.0f, 0.0f, 0.0f))		// 雷エフェクトの最大値
#define THUNDER_MIN		(Vector3(-40.0f, 0.0f, 0.0f))		// 雷エフェクトの最小値

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr float CURRENT_HEIGHT_OFFSET = 1125.0f;		// 電流の発生位置の高さのオフセット
	constexpr int VIBRATION_L_POWER = 15000;		// 電流発生時の左モーターの振動の強さ
	constexpr int VIBRATION_R_POWER = 20000;		// 電流発生時の右モーターの振動の強さ
	constexpr int VIBRATION_TIME_CUT = 5;		// 電流の合計時間から差し引く振動時間
	constexpr float THUNDER_END_EPSILON = 5.0f;		// 雷エフェクトが終了したとみなす許容誤差
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CElectricCurrent *CElectricCurrent::Create(CObjectXQuaternion *pStart,
	CUtilityPole *pEnd,
	const float fTotalTime)
{
	CElectricCurrent *pElectric = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pElectric = new CElectricCurrent;
	if (pElectric == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pElectric->Init(pStart, pEnd, fTotalTime);

	return pElectric;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CElectricCurrent::CElectricCurrent() : CObject(ELECTRIC_CURRENT_PRIORITY)
{ // メンバ変数をクリア
	m_pStart = nullptr;
	m_pEnd = nullptr;
	m_pThunder = nullptr;
	m_pMtxParent = nullptr;
	m_bGenerate = false;
	m_bCreateConnectEffect = false;
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_fTime = 0.0f;
	m_fTotalTime = 0.0f;

	// タイプの指定
	CObject::SetType(TYPE_ELECTRICCURRENT);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CElectricCurrent::~CElectricCurrent()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CElectricCurrent::Init(CObjectXQuaternion *pStart,
	CUtilityPole *pEnd,
	const float fTotalTime)
{
	Vector3 posStart = *pStart->GetPosition();		// 始点の座標
	Vector3 posEnd = *pEnd->GetPosition();			// 終点の座標
	
	const Quaternion *pQuaStart = pStart->GetQuaternion();	// 始点のクォータニオン

	// 最初の電柱と最後の電柱を保存
	m_pStart = pStart;
	m_pEnd = pEnd;

	// 時間を初期化
	m_fTime = 0.0f;

	// 合計時間を保存
	m_fTotalTime = fTotalTime;

	// 位置を上にずらす
	m_pos.y = pStart->GetVtxMax()->y + CURRENT_HEIGHT_OFFSET;

	// マトリックスを生成
	Mtx::CalcWorld(&m_mtxWorld, m_pos, *pQuaStart);

	// 雷エフェクトを生成
	m_pThunder = CThunderEffect::Create(THUNDER_MIN, THUNDER_MAX, &m_mtxWorld);

	CManager *pManager = CManager::GetInstance();	// マネージャへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();		// ジョイパッドへのポインタ

	if (pManager->GetMode() == CScene::MODE_GAME
		|| pManager->GetMode() == CScene::MODE_TUTORIAL)
	{// ゲームもしくはチュートリアルシーンならば、バイブレーション発生
		pJoypad->SetVibration(VIBRATION_L_POWER, VIBRATION_R_POWER, m_fTotalTime - VIBRATION_TIME_CUT);
	}

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CElectricCurrent::Uninit(void)
{
	if (m_pThunder != nullptr)
	{ // 生成出来ていれば、解放
		m_pThunder->Uninit();
		m_pThunder = nullptr;
	}

	// オブジェクトの破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CElectricCurrent::Update(void)
{ // 死んでいたらスキップ
	if (IsDeath() == true) return;

	if (m_fTime < 1.0f)
	{ // 線形補間を行う
		D3DXQuaternionSlerp(&m_qua,
			m_pStart->GetQuaternion(),
			m_pEnd->GetQuaternion(),
			m_fTime);

		// 1 / 合計時間分加算
		m_fTime += 1.0f / m_fTotalTime;
	}

	if (m_fTime >= 1.0f && m_bGenerate != true)
	{ // 線形補間が終わった時
		// 終了地点の電柱から電流を新規で生成
		m_pEnd->GenerateElectricity();
		m_bGenerate = true;
	}

	if ((m_pThunder->IsEndAnim(THUNDER_END_EPSILON) 
		|| CManager::GetInstance()->GetMode() != CScene::MODE_GAME)
		&& m_bGenerate == true)
	{ // 電流がある程度短くなったもしくはゲームシーン以外で、既に次の電線に電気を流したなら終了
		Uninit();
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CElectricCurrent::Draw(void)
{ // マトリックスの計算を行う
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの設定
	Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_pos, m_qua);
}