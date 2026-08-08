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
#include "electricCurrent.h"
#include "thunderEffect.h"
#include "utilityPole.h"
#include "planet.h"
#include "effect.h"
#include "vec3math.h"
#include "vec2math.h"
#include "matrix.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define THUNDER_MAX		(Vector3(40.0f, 0.0f, 0.0f))		// 雷エフェクトの最大値
#define THUNDER_MIN		(Vector3(-40.0f, 0.0f, 0.0f))		// 雷エフェクトの最小値

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CElectricCurrent *CElectricCurrent::Create(CUtilityPole *pStart, 
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
CElectricCurrent::CElectricCurrent()
{ // メンバ変数をクリア
	m_pStart = nullptr;
	m_pEnd = nullptr;
	m_pThunder = nullptr;
	m_pMtxParent = nullptr;
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
HRESULT CElectricCurrent::Init(CUtilityPole *pStart, 
	CUtilityPole *pEnd,
	const float fTotalTime)
{
	Vector3 posStart = *pStart->GetPosition();		// 始点の座標
	Vector3 posEnd = *pEnd->GetPosition();			// 終点の座標
	
	const Quaternion *pQuaStart = pStart->GetQuaternion();	// 始点のクォータニオン
	const Quaternion *pQuaEnd = pEnd->GetQuaternion();		// 終点のクォータニオン

	// 最初の電柱と最後の電柱を保存
	m_pStart = pStart;
	m_pEnd = pEnd;

	// 時間を初期化
	m_fTime = 0.0f;

	// 合計時間を保存
	m_fTotalTime = fTotalTime;

	// 位置を上にずらす
	m_pos.y = pStart->GetVtxMax()->y + 1125.0f;

	// マトリックスを生成
	Mtx::CalcWorld(&m_mtxWorld, m_pos, *pQuaStart);

	// 雷エフェクトを生成
	m_pThunder = CThunderEffect::Create(THUNDER_MIN, THUNDER_MAX, &m_mtxWorld);

	// 電流が流れたため電柱のフラグを立てる
	pStart->SetEnableElectriced(true);
	pEnd->SetEnableElectriced(true);

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
{
	if (m_fTime < 1.0f)
	{ // 線形補間を行う
		D3DXQuaternionSlerp(&m_qua,
			m_pStart->GetQuaternion(),
			m_pEnd->GetQuaternion(),
			m_fTime);

		// 1 / 合計時間分加算
		m_fTime += 1.0f / m_fTotalTime;
	}

	if (m_fTime >= 1.0f && m_pThunder->IsEndAnim(5.0f))
	{ // 線形補間が終わった時
		// 終了地点の電柱から電流を新規で生成
		m_pEnd->GenerateElectricity();

		// 終了
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