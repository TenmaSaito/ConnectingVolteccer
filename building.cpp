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
#include "utilityPole.h"
#include "vec3math.h"
#include "effect.h"
#include "particle.h"
#include "color.h"
#include "texture.h"
#include <string_view>
#include <array>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define SCALE_VALUE		(1.0f / 15.0f)		// 線形補間の増減値
#define PARTICLE_PATH	"data/TEXTURE/effect000.jpg"	// エフェクトのパス

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asBuildingPath[CBuilding::TYPE_MAX] =	// 建物モデルのパス
	{
		"data/MODEL/house000.x",		// 建物0
		"data/MODEL/house001.x",		// 建物1
		"data/MODEL/house002.x",		// 建物2
		"data/MODEL/house003.x",		// 建物3
	};
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CBuilding *CBuilding::Create(const TYPE type,
	const Vector3 &position,
	const Vector3 &vecQua,
	const float fAngle)
{ // 建物の生成
	CBuilding *pBuilding = nullptr;		// 建物へのポインタ

	switch (type)
	{ // 建物の種類で場合分け
	default:		// 特段処理の無いただの建物
		pBuilding = new CBuilding(type);
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
CBuilding *CBuilding::Create(const TYPE type, const Vector3 &position)
{ // 建物の生成
	CBuilding *pBuilding = nullptr;		// 建物へのポインタ

	switch (type)
	{ // 建物の種類で場合分け
	default:		// 特段処理の無いただの建物
		pBuilding = new CBuilding(type);
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
CBuilding::CBuilding(const TYPE type, const int nPriority) : CObjectXQuaternion(nPriority)
{ // メンバ変数のクリア
	m_pNearPole = nullptr;
	m_bFound = false;
	m_bLighting = false;
	m_fLerp = 0.0f;
	m_fValue = 0.0f;
	m_buildingType = type;

	// タイプ設定
	SetType(CObject::TYPE_BUILDING);
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
HRESULT CBuilding::Init(const Vector3 &position,
	const Vector3 &vecQua,
	const float fAngle)
{
	HRESULT hr = S_OK;		// 結果
	CGame *pGame = CManager::GetInstance()->GetScene(&pGame);		// ゲームへのポインタ
	CPlanet *pPlanet = pGame->GetPlanet();		// 惑星へのポインタ

	// 親クラスの初期化
	hr = CObjectXQuaternion::Init(c_asBuildingPath[m_buildingType].data(), position, vecQua, fAngle);

	// 親を惑星に設定
	SetParent(pPlanet->GetMatrix());

	// 増減値を設定
	m_fValue = SCALE_VALUE;
	return hr;
}

//==================================================================================
// --- 初期化処理 (任意軸と角度を現在の惑星から自動設定) ---
//==================================================================================
HRESULT CBuilding::Init(const Vector3 &position)
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
	hr = CObjectXQuaternion::Init(c_asBuildingPath[m_buildingType].data(), position, vecQua, fAngle);

	// 親を惑星に設定
	SetParent(pPlanet->GetMatrix());

	// 増減値を設定
	m_fValue = SCALE_VALUE;
	return hr;
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
{ 
	// まだ電柱が登録されていなければ
	if (m_bFound == false)
	{ // 最も近い電柱を探す
		FindUtilityPole();
		m_bFound = true;		// 検索済みフラグを立てる
	}
	
	if (m_pNearPole != nullptr)
	{ // 電柱が存在するなら判定
		if (m_pNearPole->IsElectriced())
		{ // 既に電柱に電流が流れている場合
			Vector3 scale = *GetScale();		// スケールの値

			// スケール値を変化
			m_fLerp += m_fValue;
			if (m_fLerp >= 1.0f || m_fLerp <= 0.0f)
			{ // 補間用変数が0,1の範囲を超えたら、変化量を反転
				m_fValue *= -1;
			}

			scale.x *= (m_fValue > 0) ? 1.01f : 0.99f;
			scale.y *= (m_fValue > 0) ? 0.99f : 1.01f;

			SetScale(scale);

			if (m_bLighting != true)
			{ // 始めて電流が流れたとき
				auto pParticle = CParticleQuaternion::Create(Vector3(0.0f, 0.0, 0.0f),
					Vector2(15.0f, 15.0f),
					Colors::GetColor(Colors::C_ORANGE),
					*GetVecQua(),
					GetAngle(),
					3,
					100);

				// 親マトリックスとテクスチャを設定
				pParticle->SetParent(GetMatrix());
				pParticle->BindTexture(CTexture::GetInstance()->Register(PARTICLE_PATH));
				m_bLighting = true;
			}
		}
	}

	// 親クラスの更新
	CObjectXQuaternion::Update();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CBuilding::Draw(void) 
{ // 親クラスの描画
	CObjectXQuaternion::Draw();
}

//==================================================================================
// --- 最も近い電柱の検索処理 ---
//==================================================================================
void CBuilding::FindUtilityPole(void)
{
	CObject *pObject = CObject::GetTop(UTILITYPOLE_PRIORITY);		// 最初のオブジェクト
	float fLengthMin = 10000.0f;			// 現状最も近い電柱との距離
	Vector3 pos;		// 建物のワールド座標

	// 座標をマトリックスでワールド座標に変換
	pos = GetWorldPosition();

	while (pObject != nullptr)
	{ // オブジェクトを走査
		CObject *pObjectNext = pObject->GetNext();			// 次のオブジェクトへのポインタ

		if (pObject->GetType() == CObject::TYPE_POLE)
		{ // もしオブジェクトが電柱であれば、ポインタをキャスト
			CUtilityPole *pPole = static_cast<CUtilityPole*>(pObject);

			// 電柱のワールド座標を求める
			Vector3 posPole;	// 電柱のワールド座標
			posPole = pPole->GetWorldPosition();

			// 距離を計算
			float fLength = Vec3::Length(posPole, pos);
			if (fLength < fLengthMin)
			{ // もし前回の距離よりも近いなら、ポインタ保存 + 距離更新
				fLengthMin = fLength;
				m_pNearPole = pPole;
			}
		}

		pObject = pObjectNext;		// ポインタ更新
	}
}