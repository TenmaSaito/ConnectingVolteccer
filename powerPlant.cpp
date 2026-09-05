//==================================================================================
// 
// 発電所クラスのソースファイル [powerPlant.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/22
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "powerPlant.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "game.h"
#include "planet.h"
#include "player.h"
#include "utilityPole.h"
#include "electricalCable.h"
#include "electricCurrent.h"
#include "thunderCamera.h"
#include "objectBillboard3D.h"
#include "color.h"
#include "mapManager.h"
#include "vec3math.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MODEL_PATH		"data/MODEL/powerPlant.x"		// 発電所のモデルパス
#define HIT_ALPHA		(0.2f)				// プレイヤーカメラのレイと当たった時に設定するα値
#define BILLBOARD_POS	(Vector3(0.0f, GetVtxMax()->y, 0.0f))		// ガイド用ビルボードの位置
#define BILLBOARD_SIZE	(Vector2(225.0f, 50.0f))		// ガイド用ビルボードのサイズ
#define BILLBOARD_PATH	"data/TEXTURE/rideon.png"		// ガイド用ビルボードのテクスチャパス

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPowerPlant *CPowerPlant::Create(const Vector3 &pos,
	const Vector3 &vecQua,
	const float fAngle,
	const int nID)
{
	CPowerPlant *pPlant = new CPowerPlant;		// 生成した発電所へのポインタ
	if (pPlant != nullptr)
	{ // 生成できている場合、初期化処理
		pPlant->Init(pos, vecQua, fAngle, nID);
	}

	return pPlant;
}

//==================================================================================
// --- 生成処理 (任意軸と角度を現在の惑星から自動設定) ---
//==================================================================================
CPowerPlant *CPowerPlant::Create(const Vector3 &pos, const int nID)
{
	CPowerPlant *pPlant = new CPowerPlant;		// 生成した発電所へのポインタ
	if (pPlant != nullptr)
	{ // 生成できている場合、初期化処理
		pPlant->Init(pos, nID);
	}

	return pPlant;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPowerPlant::CPowerPlant(const int nPriority) : CObjectXQuaternion(nPriority)
{ // メンバ変数のクリア
	m_pCurrentPole = nullptr;
	m_pCurrentCable = nullptr;
	m_pPlayer = nullptr;
	m_nID = -1;
	m_bHitByPlayerCamRay = false;
	m_bDisp = true;

	// タイプ設定
	SetType(CObject::TYPE_POWERPLANT);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPowerPlant::~CPowerPlant()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPowerPlant::Init(const Vector3 &position,
	const Vector3 &vecQua,
	const float fAngle,
	const int nID)
{ // 引数を保存
	m_nID = nID;

	// 親クラスの初期化
	CObjectXQuaternion::Init(MODEL_PATH, position, vecQua, fAngle);
	return S_OK;
}

//==================================================================================
// --- 初期化処理 (任意軸と角度を現在の惑星から自動設定) ---
//==================================================================================
HRESULT CPowerPlant::Init(const Vector3 &position, const int nID)
{
	HRESULT hr = S_OK;		// 結果
	CMapManager *pMap = CMapManager::GetInstance();	// マップへのポインタ
	CPlanet *pPlanet = pMap->GetPlanet();			// 惑星の取得

	// 引数を保存
	m_nID = nID;

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
	CObjectXQuaternion::Init(MODEL_PATH, position, vecQua, fAngle);
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPowerPlant::Uninit(void)
{ // 電線の破棄処理
	RemoveConnected();

	// 親クラスの終了
	CObjectXQuaternion::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPowerPlant::Update(void)
{
	CCamera *pFocusCam = CCamera::GetCamera(CCamera::GetFocus());		// フォーカスしているカメラへのポインタ
	CMapManager *pMap = CMapManager::GetInstance();		// マップへのポインタ
	CPlanet *pPlanet = pMap->GetPlanet();				// 惑星へのポインタ

	// nullならスキップ
	if (pFocusCam == nullptr || pPlanet == nullptr) return;

	Vector3 pos = VECTOR3_NULL;						// 自身の位置
	Vector3 posCam = *pFocusCam->GetPosV();			// 視点座標
	Vector3 posPlanet = *pPlanet->GetPosition();	// 惑星の座標
	Vector3 vecToPos = VECTOR3_NULL;		// 惑星から自身への方向ベクトル
	Vector3 vecToCam = VECTOR3_NULL;		// 惑星からカメラへの方向ベクトル
	Matrix mtxWorld;		// ワールドマトリックス
	float fDot = 0.0f;		// 内積結果

	// 自身の位置を計算
	D3DXMatrixIdentity(&mtxWorld);
	D3DXVec3TransformCoord(&pos, &pos, CalcMatrixUnaffect(&mtxWorld));

	// それぞれの方向ベクトルを求める
	vecToPos = Vec3::Direction(pos, posPlanet);
	vecToCam = Vec3::Direction(posCam, posPlanet);

	// カメラの指定位置と自身の位置を計算して、見えない場所であればパーティクル生成をストップ
	fDot = Vec3::Dot(vecToPos, vecToCam);
	if (fDot > 0 || CManager::GetInstance()->GetMode() == CScene::MODE_RESULT)
	{ // 内積が0より大きく映る可能性がある場合、もしくは結果画面なら有効化
		m_bDisp = true;

		// カメラに映っている場合のみ、判定
		m_bHitByPlayerCamRay = IsHitByPlayerCamRay() && !m_pPlayer->IsRiding();		// プレイヤーカメラとプレイヤの間にいるか
	}
	else
	{ // 内積が0より小さく映る可能性が低い且つゲームモード場合、無効化
		m_bDisp = false;
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPowerPlant::Draw(void)
{
	if (m_bDisp != true) return;

	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	if (m_bHitByPlayerCamRay)
	{ // αテストを有効にする + Zバッファへの書き込みを無効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 30);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		// α値を変更
		SetAlpha(HIT_ALPHA);
	}

	CObjectXQuaternion::Draw();

	if (m_bHitByPlayerCamRay)
	{ // αテストを無効にする + Zバッファへの書き込みを有効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		// α値を戻す
		SetAlpha(DEFAULT_ALPHA);
	}
}

//==================================================================================
// --- 電柱との接続処理 ---
//==================================================================================
bool CPowerPlant::Connect(CUtilityPole *pPole)
{ // nullの場合スキップ
	if (pPole == nullptr) return false;

	// 配列内に既に含まれていないか確認
	const auto result= std::ranges::find(m_vpPole, pPole);
	if (result != m_vpPole.cend())
	{ // 見つかった場合、接続済みの為スキップ
		return false;
	}

	// 電柱の接続処理呼び出し
	pPole->Connected(this);

	// 接続済みとしてポインタを保存
	m_vpPole.push_back(pPole);

	// 今回繋げた電柱としてポインタを保存
	m_pCurrentPole = pPole;

	// 電柱同士を電線で接続
	m_pCurrentCable = CElectricalCable::Create(this,
		pPole, 
		CMapManager::GetInstance()->GetPlanet());
	m_pCurrentCable->SetParent(GetParent());
	return true;
}

//==================================================================================
// --- 今回繋げた電柱へ電流を流す処理 ---
//==================================================================================
void CPowerPlant::InvokeElectric(void)
{ // 今回繋げた電柱から電流を生成
	// まだ繋げられていないか既に死んでいたならスキップ
	if (m_pCurrentPole == nullptr || IsDeath() == true) return;		

	// 電流用カメラを取得
	CThunderCamera *pThunderCam = static_cast<CThunderCamera*>(CCamera::GetCamera(CCamera::TYPE_THUNDER));

	// 自身とつながっている電柱に電気を流す
	CElectricCurrent *pCurrent = CElectricCurrent::Create(this, m_pCurrentPole);
	pCurrent->SetParent(CMapManager::GetInstance()->GetPlanet()->GetMatrix());

	// 電流用カメラを取得出来たなら、カメラのターゲットを変更
	if(pThunderCam != nullptr) pThunderCam->ChangeTarget(pCurrent);

	// 電線を通電させる
	m_pCurrentCable->Electric();
}

//==================================================================================
// --- 今回繋げたのを取り消す処理 ---
//==================================================================================
void CPowerPlant::RemoveConnected(void)
{
	if (m_pCurrentCable != nullptr)
	{ // 繋げた電線があれば破棄
		m_pCurrentCable->Uninit();
		m_pCurrentCable = nullptr;
	}

	// 接続先のポインタが登録済みなら消す
	auto iter = std::ranges::find(m_vpPole, m_pCurrentPole);
	if(iter != m_vpPole.cend()) m_vpPole.erase(iter);

	// 接続先のポインタを手放す
	m_pCurrentPole = nullptr;
}