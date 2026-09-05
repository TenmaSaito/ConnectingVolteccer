//==================================================================================
// 
// 草原クラスのソースファイル [grassland.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/16
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "grassland.h"
#include "grass.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "rand.h"
#include "player.h"
#include "vec2math.h"
#include "vec3math.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEF_MAX_SIZE		Vector2(15.0f, 50.0f)		// 基本の最大サイズ
#define DEF_MIN_SIZE		Vector2(5.0f, 10.0f)		// 基本の最小サイズ
#define DEF_MAXSWAYSPEED	(0.07f)				// 基本の最大揺れ速度
#define DEF_MINSWAYSPEED	(0.01f)				// 基本の最小揺れ速度
#define DEF_MAXSWAYWIDTH	(HALF_PI * 0.75f)	// 基本の最大揺れ幅
#define DEF_MINSWAYWIDTH	(0.01f)				// 基本の最小揺れ幅

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CGrassLand *CGrassLand::Create(const Vector3 &pos, const Vector2 &size, const int nNumGrass, const Propaty &propaty)
{
	CGrassLand *pGrassLand = nullptr;

	// 草原を生成
	pGrassLand = new CGrassLand;
	if (pGrassLand == nullptr)
	{ // 生成失敗
		return nullptr;
	}

	// 草の初期化
	pGrassLand->Init(pos, size, nNumGrass, propaty);
	return pGrassLand;
}

//==================================================================================
// --- デフォルトのプロパティ取得処理 ---
//==================================================================================
CGrassLand::Propaty CGrassLand::GetDefaultPropaty(void)
{
	return Propaty{ DEF_MAX_SIZE,		// 最大サイズ
		DEF_MIN_SIZE,					// 最小サイズ
		DEF_MAXSWAYSPEED,				// 揺れる最大速度
		DEF_MINSWAYSPEED,				// 揺れる最小速度
		DEF_MAXSWAYWIDTH,				// 揺れる最大幅
		DEF_MINSWAYWIDTH				// 揺れる最小幅
	};
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CGrassLand::CGrassLand()
{ // メンバ変数のクリア
	ZeroMemory(m_apGrass, sizeof(m_apGrass));
	m_nNumGrass = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CGrassLand::~CGrassLand()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CGrassLand::Init(const Vector3 &pos, const Vector2 &size, const int nNumGrass, const Propaty &propaty)
{
	CRand *pRand = CRand::GetInstance();				// 乱数デバイスへのポインタ
	Vector3 posGrass = VECTOR3_NULL;		// 生成位置
	Vector2 sizeGrass = VECTOR2_NULL;		// 草のサイズ
	float fSwaySpeed = 0.0f;		// 揺れる速度
	float fSwayWidth = 0.0f;		// 揺れる幅

	// 草を生成
	for (int nCntGrass = 0; nCntGrass < nNumGrass; nCntGrass++)
	{
		if (m_apGrass[nCntGrass] == nullptr)
		{ // 空だったら生成
			// 位置をランダムで指定
			posGrass = Vec3::Random();
			posGrass.x *= size.x;
			posGrass.y = 0.0f;
			posGrass.z *= size.y;

			// 位置をpos中心に指定
			posGrass += pos;

			// 草のサイズをランダムで指定
			sizeGrass.x = pRand->Generate(propaty.minSize.x, propaty.maxSize.x);
			sizeGrass.y = pRand->Generate(propaty.minSize.y, propaty.maxSize.y);

			// 草の揺れスピードと幅をランダムで指定
			fSwaySpeed = pRand->Generate(propaty.fMinSwaySpeed, propaty.fMaxSwaySpeed);
			fSwayWidth = pRand->Generate(propaty.fMinSwayWidth, propaty.fMaxSwayWidth);

			// 草を生成
			m_apGrass[nCntGrass] = CGrass::Create(posGrass,
				sizeGrass,
				fSwaySpeed,
				fSwayWidth);
		}
	}

	// 総数を保存
	m_nNumGrass = nNumGrass;

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CGrassLand::Uninit(void)
{
	for (auto &pGrass : m_apGrass)
	{ // 草が存在するなら破棄
		if (pGrass == nullptr) continue;

		// 草を破棄
		pGrass->Uninit();
		delete pGrass;
		pGrass = nullptr;
	}

	// オブジェクトの破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CGrassLand::Update(void)
{
	//CPlayer *pPlayer = CManager::GetInstance()->GetScene<CGame>()->GetPlayer();		// プレイヤーへのポインタ
	
	for (auto &pGrass : m_apGrass)
	{ // 草が存在するなら更新
		if (pGrass == nullptr) continue;

		// 草を更新
		pGrass->Update();
	}

	// プレイヤーと草むらの当たり判定
	//Collision(*pPlayer->GetPosition(), 70.0f);
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CGrassLand::Draw(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイスへのポインタ

	// ライティングを無効に設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (auto &pGrass : m_apGrass)
	{ // 草が存在するなら描画
		if (pGrass == nullptr) continue;

		// 草を描画
		pGrass->Draw();
	}

	// ライティングを有効に設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//==================================================================================
// --- 当たり判定処理 ---
//==================================================================================
bool CGrassLand::Collision(const Vector3 &pos, const float fRadius)
{
	bool bCollision = false;		// 一度でも当たったか

	for (auto &pGrass : m_apGrass)
	{ // 草が存在するなら判定
		if (pGrass == nullptr) continue;

		// 当たり判定
		bCollision = (pGrass->Collision(pos, fRadius) == true && bCollision == false) ? true : bCollision;
	}

	return bCollision;
}