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
#include "happyNote.h"
#include "particleQuaternion.h"
#include "particleQuaternionBuilder.h"
#include "texture.h"
#include "color.h"
#include "camera.h"
#include "mapManager.h"
#include "planet.h"
#include "building.h"
#include "vec3math.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define NOTE_MOVABLE_MIN	(Vector3(0.2f, 0.0f, 0.2))
#define NOTE_MOVABLE_MAX	(Vector3)

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
		.SetMove(Vector3(0.1f, 6.0f, 0.1f))
		.SetScale(Vector2(125.0f, 125.0f))
		.SetNumEffectFrame(1)
		.SetMoveVariation(Vector3(10.0f, 3.0f, 10.0f))
		.SetPercent(2)
		.SetEffectLifeMax(45)
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