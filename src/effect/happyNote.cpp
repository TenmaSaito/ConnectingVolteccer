//==================================================================================
// 
// 喜び音符出現クラスのソースファイル [happyNote.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/31
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "effect/happyNote.h"
#include "effect/particleQuaternion.h"
#include "effect/particleQuaternionBuilder.h"
#include "render/texture.h"
#include "math/color.h"
#include "render/camera.h"
#include "game/stage/mapManager.h"
#include "game/stage/planet.h"
#include "game/stage/building.h"
#include "math/vec3math.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define NOTE_MOVABLE_MIN	(Vector3(0.2f, 0.0f, 0.2))
#define NOTE_MOVABLE_MAX	(Vector3)

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	const Vector3 NOTE_MOVE = Vector3(0.1f, 6.0f, 0.1f);		// 音符パーティクルの移動量
	const Vector2 NOTE_SCALE = Vector2(125.0f, 125.0f);		// 音符パーティクルのサイズ
	const Vector3 NOTE_MOVE_VARIATION = Vector3(10.0f, 3.0f, 10.0f);		// 音符パーティクルの移動量のばらつき
	constexpr int NOTE_PERCENT = 2;		// 音符パーティクルの発生確率 (%)
	constexpr int NOTE_EFFECT_LIFE = 45;		// 音符エフェクトの最大寿命 (フレーム)
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CHappyNote *CHappyNote::Create(const Vector3 &offset,
	const Vector3 &vecQua,
	const float fAngle,
	const CBuilding *pBuilding)
{
	CHappyNote *pHappyNote = new CHappyNote;		// 生成したオブジェクトへのポインタ
	if (pHappyNote != nullptr)
	{ // 生成出来ていたら、初期化
		pHappyNote->Init(offset, vecQua, fAngle, pBuilding);
	}
	return pHappyNote;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CHappyNote::CHappyNote() : CObject(DEFAULT_OBJ_PRIORITY)
{ // タイプを指定
	SetType(TYPE_HAPPYNOTE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CHappyNote::~CHappyNote()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CHappyNote::Init(const Vector3 &offset, 
	const Vector3 &vecQua,
	const float fAngle, 
	const CBuilding *pBuilding)
{ // 引数を保存
	m_pBuilding = pBuilding;
	
	// パーティクルの設定を決定し、生成
	m_pParticle = CParticleQuaternionBuilder().SetPosition(offset)
		.SetColor(Colors::Random(false))
		.SetVecQua(vecQua)
		.SetAngle(fAngle)
		.SetLife(INT_MAX)
		.SetMove(NOTE_MOVE)
		.SetScale(NOTE_SCALE)
		.SetNumEffectFrame(1)
		.SetMoveVariation(NOTE_MOVE_VARIATION)
		.SetPercent(NOTE_PERCENT)
		.SetEffectLifeMax(NOTE_EFFECT_LIFE)
		.Build();

	// 親マトリックスとテクスチャを設定
	m_pParticle->SetParent(pBuilding->GetMatrix());
	m_pParticle->BindTexture(CTexture::TYPE_NOTE);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CHappyNote::Uninit(void)
{ // 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CHappyNote::Update(void)
{ // 建物が見えなくなった時、パーティクル生成を停止
	m_pParticle->SetEnable(m_pBuilding->IsDisp());
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CHappyNote::Draw(void)
{

}