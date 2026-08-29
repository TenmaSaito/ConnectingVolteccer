//==================================================================================
// 
// プレイヤーカメラクラスのソースファイル [playerCamera.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "playerCamera.h"
#include "manager.h"
#include "input.h"
#include "joypad.h"
#include "player.h"
#include "vec3math.h"
#include "debugproc.h"
#include "util.h"
#include "planet.h"
#include "effect.h"
#include "ray.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_FOVY		(45.0f)		// 視野角
#define DEFAULT_ZN			(1.0f)		// 最短距離
#define DEFAULT_ZF			(10000.0f)	// 最遠距離
#define DEFAULT_SPD			(1.0f)		// 移動速度
#define DEFAULT_ROTSPD		(0.03f)		// 回転速度
#define DEFAULT_LENGTH		(1000.0f)	// 注視点との距離
#define LENGTH_MINLIMIT		(10.0f)		// 注視点との最小距離
#define LENGTH_MAXLIMIT		(100000.0f)	// 注視点との最大距離
#define DEFAULT_LENGTHSPD	(10.0f)		// 距離変更スピード
#define DEFAULT_PLANET_Z	(-0.08f)	// 惑星フォーカス時のカメラのZ角度
#define ENABLE_RAY_PLAYER_CAM			// プレイヤーのカメラの光線ベクトルの表示
#define RAY_LENGTH			(1000.0f)	// レイの長さ
#define LERP_SPD			(CManager::SecToRatio(0.5f))		// 線形補間の増加値

//==================================================================================
// --- カメラの生成 ---
//==================================================================================
CPlayerCamera *CPlayerCamera::Create(const Vector3 &rot,
	const Vector3 &rotRiding,
	const float fLengthPlayer, 
	const float fLengthRiding)
{
	CPlayerCamera *pPlayerCam = nullptr;		// 生成したカメラへのポインタ

	// 自身のタイプのカメラが既に存在しているか確認
	pPlayerCam = static_cast<CPlayerCamera*>(CCamera::GetCamera(TYPE_PLAYER));
	if (pPlayerCam == nullptr)
	{ // 存在していなかった場合は、新規作成
		pPlayerCam = new CPlayerCamera;
	}

	// 初期化処理
	pPlayerCam->Init(rot, rotRiding, fLengthPlayer, fLengthRiding);

	return pPlayerCam;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPlayerCamera::CPlayerCamera() : CCamera(TYPE_PLAYER)
{ // メンバ変数をクリア
	m_pPlayer = nullptr;
	m_rotDefault = VECTOR3_NULL;
	m_posVBefore = VECTOR3_NULL;
	m_posRBefore = VECTOR3_NULL;
	m_fLengthPlayer = 0.0f;
	m_fLengthRiding = 0.0f;
	m_fTime = 0.0f;
	m_state = STATE_PLAYER;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPlayerCamera::~CPlayerCamera()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
void CPlayerCamera::Init(const Vector3 &rot, 
	const Vector3 &rotRiding, 
	const float fLengthPlayer,
	const float fLengthRiding)
{ // 親クラスの初期化
	CCamera::Init(Vec3::Arc(fLengthPlayer, rot.y, rot.z));

	// 角度の設定
	CCamera::SetRotate(rot);

	// 引数の保存
	m_rotDefault = rot;
	m_rotRidingDefault = rotRiding;
	m_fLengthPlayer = fLengthPlayer;
	m_fLengthRiding = fLengthRiding;
	m_fTime = 1.0f;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPlayerCamera::Uninit(void)
{ // カメラの解放
	CCamera::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPlayerCamera::Update(void)
{
	CManager *pManager = CManager::GetInstance();	// マネージャへのポインタ
	auto pKeyboard = pManager->GetInputKeyboard();	// キーボードへのポインタ
	auto pJoypad = pManager->GetJoypad();			// ジョイパッドへのポインタ
	auto pProc = pManager->GetDebugProc();			// デバッグ表示へのポインタ
	auto pPlayer = m_pPlayer;				// プレイヤーへのポインタ
	Vector3 posV = *CCamera::GetPosV();		// 視点座標
	Vector3 posR = *CCamera::GetPosR();		// 注視点座標
	Vector3 rot = *CCamera::GetRotate();	// 角度
	Vector3 stick;		// スティックの入力

	if (m_state == STATE_RIDING)
	{ // 電柱に載っている場合は視点移動を可能にする
		// カメラの回転！(注視点中心)
		if (pKeyboard->GetPress(DIK_D))
		{ // 角度をずらして修正
			rot.y += DEFAULT_ROTSPD;
			rot = Vec3::FixedRotation(rot);
		}
		else if (pKeyboard->GetPress(DIK_A))
		{ // 角度をずらして修正
			rot.y -= DEFAULT_ROTSPD;
			rot = Vec3::FixedRotation(rot);
		}
		else if (pJoypad->GetStick(CJoypad::STICK_LEFT, &stick) && Vec3::Length(stick) > STICK_DEADZONE)
		{ // スティックから角度を取得
			rot.y += stick.x * DEFAULT_ROTSPD;
		}
	}

	if (m_state == STATE_PLAYER)
	{ // プレイヤーの位置取得
		posR = *pPlayer->GetPosition();

		// 視点座標を注視点座標から求める
		posV.x = posR.x + (sinf(rot.z) * sinf(rot.y) * m_fLengthPlayer);
		posV.y = posR.y + (cosf(rot.z) * m_fLengthPlayer);
		posV.z = posR.z + (sinf(rot.z) * cosf(rot.y) * m_fLengthPlayer);

#ifdef ENABLE_RAY_PLAYER_CAM
		if (pManager->GetEnablePause() == false)
		{ // ポーズ中でなければプレイヤーカメラの視線ベクトルを表示
			CRay(posV, GetRay(), RAY_LENGTH).Draw();
		}
#endif
	}
	else if (m_state == STATE_RIDING)
	{ // 電柱の上のプレイヤーの位置を取得
		posR = VECTOR3_NULL;

		// マトリックス変換
		D3DXVec3TransformCoord(&posR, &posR, pPlayer->GetMatrix());

		// 視点座標を注視点座標から求める
		posV.x = posR.x + (sinf(rot.z) * sinf(rot.y) * m_fLengthRiding);
		posV.y = posR.y + (cosf(rot.z) * m_fLengthRiding);
		posV.z = posR.z + (sinf(rot.z) * cosf(rot.y) * m_fLengthRiding);

#ifdef ENABLE_RAY_PLAYER_CAM
		if (pManager->GetEnablePause() == false)
		{ // ポーズ中でなければプレイヤーカメラの視線ベクトルを表示
			CRay(posV, GetRay(), RAY_LENGTH).Draw();
		}
#endif
	}

	if (m_fTime < 1.0f)
	{ // 線形補間が終わっていなければ
		m_fTime += LERP_SPD;		// 時間を増加
		if (m_fTime >= 1.0f)
		{ // 1.0fを超えた場合、修正
			m_fTime = 1.0f;
		}

		// 座標を求める
		posV = Vec3::Lerp(m_posVBefore, posV, m_fTime);
		posR = Vec3::Lerp(m_posRBefore, posR, m_fTime);
	}

	// 位置と角度を適用
	CCamera::SetPosV(posV);
	CCamera::SetPosR(posR);
	CCamera::SetRotate(rot);

	// カメラの座標を表示
	pProc->Print("Cam : PosV[{:.2f} {:.2f} {:.2f}]\nPosR[{:.2f} {:.2f} {:.2f}]\n", PRINT_VECTOR3(posV), PRINT_VECTOR3(posR));
}

//==================================================================================
// --- カメラの設置処理 ---
//==================================================================================
void CPlayerCamera::SetCamera(void)
{ // カメラを設置
	CCamera::SetCamera();
}

//==================================================================================
// --- カメラの状態変更処理 ---
//==================================================================================
void CPlayerCamera::SetState(const STATE state)
{ // フォーカスを変更
	if (m_state != state)
	{ // 前回の状態と異なっていた場合、線形補間リセット + 座標保存
		m_fTime = 0.0f;
		m_posVBefore = *CCamera::GetPosV();
		m_posRBefore = *CCamera::GetPosR();
	}

	m_state = state;		// 状態保存

	if (state == STATE_PLAYER)
	{ // カメラの角度を通常の角度に戻す
		SetRotate(m_rotDefault);
	}
	else if (state == STATE_RIDING)
	{ // カメラの角度を電柱に乗った際の角度に戻す
		SetRotate(m_rotRidingDefault);
	}
}