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
#include "grassQuaternion.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "rand.h"
#include "player.h"
#include "planet.h"
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
CGrassLand *CGrassLand::Create(const Info &info)
{
	CGrassLand *pGrassLand = new CGrassLand;		// 生成した草原へのポインタ
	if (pGrassLand != nullptr)
	{ // 生成に成功していれば、初期化処理
		pGrassLand->Init(info);
	}

	return pGrassLand;
}

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
	m_info = {};
	m_nNumGrass = 0;

	// タイプを指定
	SetType(TYPE_GRASSLAND);
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
HRESULT CGrassLand::Init(const Info &info)
{
	CRand *pRand = CRand::GetInstance();				// 乱数デバイスへのポインタ
	CPlanet *pPlanet = nullptr;				// 惑星へのポインタ
	Vector3 posGrass = VECTOR3_NULL;		// 生成位置
	Vector2 sizeGrass = VECTOR2_NULL;		// 草のサイズ
	Quaternion qua;					// クォータニオン
	float fAngle = 0.0f;			// 回転度数
	float fSwaySpeed = 0.0f;		// 揺れる速度
	float fSwayWidth = 0.0f;		// 揺れる幅
	Quaternion quaDef;				// 発生させる草のクォータニオンの基準
	Vector3 posWorldParent;			// 親オブジェクトの絶対座標
	Vector3 vecToParent;			// 惑星から親オブジェクトへのベクトル

	// 惑星を検索
	CObject *pObject = CObject::FindAnyObjectByType(PLANET_PRIORITY, TYPE_PLANET);
	pPlanet = static_cast<CPlanet*>(pObject);		// ポインタをキャスト

	// 基準となるクォータニオンを生成
	D3DXQuaternionRotationAxis(&quaDef,
		&info.vecQua,
		info.fAngle);

	// 親オブジェクトの絶対座標を求める
	Matrix mtxParent;		// 親マトリックス
	D3DXVec3TransformCoord(&posWorldParent,
		&Vec3::Zero,
		info.pMtxParent);

	// 惑星から親オブジェクトへのベクトルを求める
	vecToParent = Vec3::Direction(posWorldParent, VECTOR3_NULL);

	// 草を生成
	for (int nCntGrass = 0; nCntGrass < info.nNumGrass; nCntGrass++)
	{ // クォータニオンの初期化
		D3DXQuaternionIdentity(&qua);
		
		// 任意軸から一定の角度がついたクォータニオンを生成
		D3DXQuaternionRotationAxis(&quaDef,
			&info.vecQua,
			info.fAngle + pRand->Generate(-(info.fAngleRange * 0.5f), (info.fAngleRange * 0.5f)));
#if 1
		// 家を中心としたランダムな回転がかかったクォータニオンを生成
		D3DXQuaternionRotationAxis(&qua,
			&vecToParent,
			pRand->Generate(-D3DX_PI, D3DX_PI));
#endif
		// クォータニオンを乗算
		D3DXQuaternionMultiply(&qua, &quaDef, &qua);

		// 草を生成
		CGrassQuaternion *pGrassQua = CGrassQuaternion::Create(info.pos,
			qua,
			info.size);

		// 親を設定
		pGrassQua->SetParent(pPlanet->GetMatrix());
		m_apGrass[nCntGrass] = pGrassQua;		// ポインタを保存
	}

	return S_OK;
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
	CPlayer *pPlayer = static_cast<CPlayer*>(CObject::FindAnyObjectByType(PLAYER_PRIORITY, TYPE_PLAYER));

	for (auto &pGrass : m_apGrass)
	{ // 草が存在するなら更新
		if (pGrass == nullptr) continue;

		// 草を更新
		pGrass->Update();
	}

	// プレイヤーと草むらの当たり判定
	if(pPlayer != nullptr) Collision(*pPlayer->GetPosition(), 50.0f);
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CGrassLand::Draw(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイスへのポインタ

	// カリングを無効に設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (auto &pGrass : m_apGrass)
	{ // 草が存在するなら描画
		if (pGrass == nullptr) continue;

		// 草を描画
		pGrass->Draw();
	}

	// カリングを有効に設定
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