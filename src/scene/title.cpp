//==================================================================================
// 
// タイトルクラスのソースファイル [title.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene/title.h"
#include "object/base/object.h"
#include "core/manager.h"
#include "render/renderer.h"
#include "sound/sound.h"
#include "render/camera.h"
#include "input/input.h"
#include "input/joypad.h"
#include "render/texture.h"
#include "object/2d/object2D.h"
#include "render/titleCamera.h"
#include "ui/titlemenu.h"
#include "game/stage/mapManager.h"
#include "game/stage/planet.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr float PLANET_ROT_SPEED = 0.005f;		// 惑星の1フレーム当たりの回転角度
	constexpr float CAMERA_POS_X = 2100.0f;		// タイトル画面のカメラのX座標
	constexpr float CAMERA_POS_Y = 0.0f;		// タイトル画面のカメラのY座標
	constexpr float CAMERA_POS_Z = -2800.0f;		// タイトル画面のカメラの視点のZ座標
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTitle::CTitle()
{ // 親クラスのコンストラクタ呼び出し
	m_pMenu = nullptr;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CTitle::~CTitle()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTitle::Init(void)
{ // 開始処理
	Start();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CTitle::Uninit(void)
{
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CTitle::Update(void)
{
	// 惑星を回転
	CMapManager *pMap = CMapManager::GetInstance();		// マップマネージャへのポインタ
	Vector3 vecQua = Vector3(0.0f, D3DX_PI, 0.0f);		// 軸
	float fAngle = PLANET_ROT_SPEED;	// 回転度数
	Quaternion qua;			// かけ合わせるクォータニオン

	// クォータニオンを計算
	D3DXQuaternionIdentity(&qua);
	D3DXQuaternionRotationAxis(&qua,
		&vecQua,
		fAngle);

	// 惑星に掛ける
	pMap->GetPlanet()->MultiplyQuaternion(qua);

	// 全カメラの更新処理
	CCamera::UpdateAll();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CTitle::Draw(void)
{
}

//==================================================================================
// --- 初期化後呼び出し処理 ---
//==================================================================================
void CTitle::Start(void)
{
	// 仮置きでカメラを生成 (自動解放)
	// TODO : ここのカメラはタイトル用のカメラを作って置き換える事！
	float fX = CAMERA_POS_X;
	float fY = CAMERA_POS_Y;
	CTitleCamera *pCamera = CTitleCamera::Create(Vector3(fX, fY, CAMERA_POS_Z), Vector3(fX, fY, 0.0f));
	pCamera->SetFocus();

	// タイトル画面のメニューを生成
	m_pMenu = CTitleMenu::Create();

	CMapManager *pMap = CMapManager::GetInstance();		// マップマネージャへのポインタ
	CPlanet *pPlanet = nullptr;		// 惑星へのポインタ

	// マップを読み込み
	pMap->LoadLatest();

	Quaternion quaInv;				// 反転したクォータニオン

	// 惑星へのポインタを取得して、逆向きのクォータニオンをかけ合わせる
	pPlanet = pMap->GetPlanet();
	pPlanet->MultiplyQuaternion(*D3DXQuaternionInverse(&quaInv, pPlanet->GetQuaternion()));

	CSound *pSound = CManager::GetInstance()->GetSound();		// サウンドへのポインタ

	// タイトル画面のBGMを流す
	pSound->Play(CSound::LABEL_BGM_TITLE);
}