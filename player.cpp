//==================================================================================
// 
// プレイヤークラスのソースファイル [player.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/2
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "player.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "input.h"
#include "joypad.h"
#include "playerCamera.h"
#include "debugproc.h"
#include "meshField.h"
#include "matrix.h"
#include "motion.h"
#include "motionLoader.h"
#include "model.h"
#include "partsLoader.h"
#include "utilityPole.h"
#include "powerPlant.h"
#include "building.h"
#include "planet.h"
#include "map.h"
#include "effect.h"
#include "ray.h"
#include "util.h"
#include "lasso.h"
#include "combo.h"
#include "vec2math.h"
#include "vec3math.h"
#include "connectingEvaluate.h"
#include "shock.h"
#include "color.h"
#include <algorithm>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define PLAYER_SPD			(2.5f)		// モデルの移動スピード
#define PLAYER_ROTSPD		(0.1f)		// モデルの回転スピード
#define RESIST_POW			(0.25f)		// 摩擦
#define RIDE_LENGTH			(120.0f)	// 電柱に乗れる距離
#define POLE_MOVE_SPEED		(0.01f)		// 電柱を乗り移る際の角度
#define PLAYERCAM_DEFROT	Vector3(0.0f, 0.0f, -0.4f)		// デフォルトのカメラ角度
#define PLAYERCAM_RIDINGROT	Vector3(0.0f, 0.0f, -1.16f)		// 電柱に乗っているときのカメラ角度
#define PLAYERCAM_LEN			(1000.0f)		// プレイヤーのカメラの距離
#define PLAYERCAM_RIDING_LEN	(250.0f)		// 電柱に乗っているときのカメラの距離
//#define ENABLE_RAY_PLAYER_TO_POLE				// プレイヤーから投げ縄を投げられる電柱へのレイの表示
//#define ENABLE_CAN_FOCUS_POLE_VECTOR			// プレイヤーがフォーカス可能な電柱へのポインタの配列の一時保持

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPlayer *CPlayer::Create(const char *pXFileName, const Vector3 &pos, const Vector3 &rot)
{
	CPlayer *pPlayer = new CPlayer;		// 生成したオブジェクトへのポインタ
	if (pPlayer != nullptr)
	{ // 初期化処理
		pPlayer->Init(pXFileName, pos, rot);
	}

	return pPlayer;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPlayer::CPlayer(const int nPriority) : CObject(nPriority)
{ // メンバ変数をクリア
	m_pos = VECTOR3_NULL;
	m_offset = VECTOR3_NULL;
	m_move = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_rotDest = VECTOR3_NULL;
	m_vecQua = VECTOR3_NULL;
	m_pMotion = nullptr;
	m_pShock = nullptr;
	m_pThunderEffect = nullptr;
	m_pPoleNext = nullptr;
	m_bShotLasso = false;
	m_bShocked = false;
	m_fAngleRest = 0.0f;

	// タイプ設定
	SetType(TYPE_PLAYER);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPlayer::~CPlayer()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPlayer::Init(const char *pFileName, const Vector3 &pos, const Vector3 &rot)
{ // 引数の値を保存
	m_pos = pos;
	m_offset = pos;
	m_rot = rot;

	// 感電エフェクト用インスタンスを生成
	m_pShock = CShock::Create(&m_mtxWorld, Vector3(0.0f, 25.0f, 0.0f));

	// モーションを生成
	CMotionLoader *pMotionLoader = CMotionLoader::GetInstance();
	m_pMotion = pMotionLoader->CreateMotion(pMotionLoader->Register(pFileName));
	NULLPOINTER_ASSERT(m_pMotion);

	if (m_pMotion == nullptr) return E_FAIL;		// 生成失敗

	// パーツを生成
	CPartsLoader *pPartsLoader = CPartsLoader::GetInstance();
	m_vpModel = pPartsLoader->CreateParts(pPartsLoader->Register(pFileName));

	// モーションの設定
	m_pMotion->SetModel(m_vpModel);
	m_pMotion->Set(0);

	// プレイヤー用カメラの生成
	CPlayerCamera *pPlayerCam = CPlayerCamera::Create(PLAYERCAM_DEFROT, 
		PLAYERCAM_RIDINGROT, 
		PLAYERCAM_LEN,
		PLAYERCAM_RIDING_LEN);
	NULLPOINTER_ASSERT(pPlayerCam);

	if (pPlayerCam != nullptr)
	{ // プレイヤーを紐づけ
		pPlayerCam->BindPlayer(this);
		pPlayerCam->SetFocus();
	}

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPlayer::Uninit(void)
{
	for (auto &model : m_vpModel)
	{ // 各モデルを破棄
		// モデルの破棄 + 終了処理
		model->Uninit();
		model.reset();
	}

	// 配列リセット
	m_vpModel.clear();
	
	// モーションの破棄
	if (m_pMotion != nullptr)
	{ // 存在すれば破棄+終了処理
		m_pMotion->Uninit();
		m_pMotion.reset();
	}

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPlayer::Update(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャーへのポインタ
	auto pProc = pManager->GetDebugProc();				// デバッグ表示へのポインタ

	// 入力関連処理
	InputAction();

	// 位置と移動量の更新
	UpdatePotision();

	// 目標角度の更新
	UpdateRotateDest();

	// 電柱関連の更新
	UpdatePole();

	// 当たり判定関連処理
	CollisionAction();

	// 現在位置表示
	pProc->Print("[PPos : {:.2f} {:.2f} {:.2f}]\n", PRINT_VECTOR3(m_pos));

	// 各種更新処理
	OtherUpdate();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPlayer::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの設定
	const Matrix *pMtxParent = (GetRidingObjectX() == nullptr) ? nullptr : GetRidingObjectX()->GetMatrix();
	Mtx::CalcWorld(&m_mtxWorld, pMtxParent, m_pos, m_rot);

	//  ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (auto &model : m_vpModel)
	{ // 各モデルの描画
		if (m_bShocked)
		{ // 感電中は黒色に指定
			D3DMATERIAL9 matBlack = {};
			matBlack.Diffuse = Colors::GetColor(Colors::C_BLACK);
			model->SetCustomMat(matBlack);
		}
		else
		{ // デフォルトマテリアルを指定
			model->SetCustomMat();
		}

		model->Draw();
	}
}

//==================================================================================
// --- 投げ縄による接続の失敗時処理 ---
//==================================================================================
void CPlayer::FailedShot(void)
{ // 感電
	m_pShock->Set();
	m_bShocked = true;

	// コンボの時間を一時停止
	CCombo *pCombo = CManager::GetInstance()->GetScene<CGame>()->GetCombo();		// コンボ表示へのポインタ
	pCombo->SetEnablePause(true);

	// 投げ縄フラグを下ろす
	m_bShotLasso = false;
}

//==================================================================================
// --- 繋がった電柱の設定処理 ---
//==================================================================================
void CPlayer::ChangeRidingPole(CUtilityPole *pNext)
{ 
	CGame *pGame = CManager::GetInstance()->GetScene(&pGame);		// シーンの取得
	CPlanet *pPlanet = pGame->GetPlanet();			// 惑星の取得

	// 惑星から各電柱の座標を求め、内積を求める
	// 内積の角度分、次の電柱への角度へ回転させる
	Vector3 planetToRiding = VECTOR3_NULL;		// 惑星からプレイヤーの乗っている電柱へのベクトル
	Vector3 planetToNext = VECTOR3_NULL;		// 惑星から次に乗る予定の電柱へのベクトル
	Vector3 ridingWorldPos = GetRidingObjectX()->GetWorldPosition();	// 乗っているオブジェクトの絶対座標
	Vector3 nextWorldPos = pNext->GetWorldPosition();					// 次に乗る予定の電柱の絶対座標

	// ベクトルを求める
	planetToRiding = Vec3::Direction(ridingWorldPos, *pPlanet->GetPosition());
	CRay(*pPlanet->GetPosition(), planetToRiding, 10000.0f).Draw();

	planetToNext = Vec3::Direction(nextWorldPos, *pPlanet->GetPosition());
	CRay(*pPlanet->GetPosition(), planetToNext, 10000.0f).Draw();

	// ベクトルから内積計算
	float fDot = Vec3::Dot(planetToRiding, planetToNext);

	// 内積から二つのベクトルの角度差分を求める
	m_fAngleRest = std::acosf(std::clamp(fDot, -1.0f, 1.0f));

	// 乗っている電柱から次に乗る電柱へのベクトルを計算して向きを求める
	float fAngle = Vec2::Direction(Vec3::ToVector2(nextWorldPos, Vec3::Axis::Y),
		Vec3::ToVector2(ridingWorldPos, Vec3::Axis::Y));

	// 任意軸を右に回転
	fAngle += HALF_PI;
	fAngle = Util::FixedRotation(fAngle);

	m_vecQua = Vec2::ToVector3(Vec2::Direction(fAngle));
	m_vecQua.z = m_vecQua.y;
	m_vecQua.y = 0.0f;
	CRay(m_offset + Vector3(0.0f, 100.0f, 0.0f), m_vecQua, 1000.0f).Draw();

	// オフセットを電柱上に設定
	m_pos.y = m_offset.y + pNext->GetVtxMax()->y;
	
	// 電柱から降りる際の処理を実行
	DismountPole();

	m_pPoleNext = pNext;			// 次の電柱を保存

	// コンボ追加
	CCombo *pCombo = pGame->GetCombo();		// コンボ表示へのポインタ
	pCombo->AddCombo(1);
}

//==================================================================================
// --- Input時関連処理 ---
//==================================================================================
void CPlayer::InputAction(void)
{ // プレイヤーフォーカスのカメラではない場合、スキップ
	if (CCamera::GetFocus() != CCamera::TYPE_PLAYER) return;

	// 移動関連の入力
	InputMoving();

	// 電柱関連の入力
	InputPole();
}

//==================================================================================
// --- 移動関連の入力処理 ---
//==================================================================================
void CPlayer::InputMoving(void)
{
	CManager *pManager = CManager::GetInstance();					// マネージャーへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();						// ジョイパッドへのポインタ
	CCamera *pPlayerCam = CCamera::GetCamera(CCamera::TYPE_PLAYER);	// プレイヤーカメラへのポインタ
	const Vector3 *pCameraRot = pPlayerCam->GetRotate();		// カメラの角度
	Vector3 move = m_move;						// 代入予定の移動量
	Vector3 stick = VECTOR3_NULL;				// ジョイパッドのスティック入力
	MOTIONTYPE type = MOTIONTYPE_NEUTRAL;		// モーションタイプ
	int nFrameBleand = 0;						// ブレンド時間
	bool bMove = true;			// 動いたか

	if (m_bShotLasso == true)
	{ // 投げ縄を投げている最中もしくは電線を移動している最中なら、移動処理はスキップ
		return;
	}

	if (pKeyboard->GetPress(DIK_W))
	{ // Wを押したとき
		if (pKeyboard->GetPress(DIK_A))
		{ // Aを押したとき
			// カメラの角度に合わせて、平行移動！
			move.z += cosf(pCameraRot->y - (D3DX_PI * 0.25f)) * PLAYER_SPD;
			move.x += sinf(pCameraRot->y - (D3DX_PI * 0.25f)) * PLAYER_SPD;

			// カメラの角度に合わせて、モデルの目標角度を求める！
			m_rotDest.y = (pCameraRot->y - (D3DX_PI * 1.25f));
		}
		else if (pKeyboard->GetPress(DIK_D))
		{ // Dを押したとき
			// カメラの角度に合わせて、平行移動！
			move.z += cosf(pCameraRot->y + (D3DX_PI * 0.25f)) * PLAYER_SPD;
			move.x += sinf(pCameraRot->y + (D3DX_PI * 0.25f)) * PLAYER_SPD;

			// カメラの角度に合わせて、モデルの目標角度を求める！
			m_rotDest.y = (pCameraRot->y + (D3DX_PI * 1.25f));
		}
		else
		{ // 純粋なW入力時
			// カメラの角度に合わせて、平行移動！
			move.x += sinf(pCameraRot->y) * PLAYER_SPD;
			move.z += cosf(pCameraRot->y) * PLAYER_SPD;

			// カメラの角度に合わせて、モデルの目標角度を求める！
			m_rotDest.y = (pCameraRot->y + D3DX_PI);
		}

		if (m_pMotion->GetType() == MOTIONTYPE_NEUTRAL
			&& m_pMotion->GetBlendType() != MOTIONTYPE_MOVE)
		{ // 移動モーション
			type = MOTIONTYPE_MOVE;
			nFrameBleand = 10;
		}
	}
	else if (pKeyboard->GetPress(DIK_S))
	{ // Sを押したとき
		if (pKeyboard->GetPress(DIK_A))
		{ // Aを押したとき
			// カメラの角度に合わせて、平行移動！
			move.z += cosf(pCameraRot->y + (D3DX_PI * 1.25f)) * PLAYER_SPD;
			move.x += sinf(pCameraRot->y + (D3DX_PI * 1.25f)) * PLAYER_SPD;

			// カメラの角度に合わせて、モデルの目標角度を求める！
			m_rotDest.y = (pCameraRot->y + (D3DX_PI * 0.25f));
		}
		else if (pKeyboard->GetPress(DIK_D))
		{ // Dを押したとき
			// カメラの角度に合わせて、平行移動！
			move.z += cosf(pCameraRot->y - (D3DX_PI * 1.25f)) * PLAYER_SPD;
			move.x += sinf(pCameraRot->y - (D3DX_PI * 1.25f)) * PLAYER_SPD;

			// カメラの角度に合わせて、モデルの目標角度を求める！
			m_rotDest.y = (pCameraRot->y - (D3DX_PI * 0.25f));
		}
		else
		{ // 純粋なS入力時
			// カメラの角度に合わせて、平行移動！
			move.x += sinf(pCameraRot->y + D3DX_PI) * PLAYER_SPD;
			move.z += cosf(pCameraRot->y + D3DX_PI) * PLAYER_SPD;

			// カメラの角度に合わせて、モデルの目標角度を求める！
			m_rotDest.y = pCameraRot->y;
		}

		if (m_pMotion->GetType() == MOTIONTYPE_NEUTRAL
			&& m_pMotion->GetBlendType() != MOTIONTYPE_MOVE)
		{ // 移動モーション
			type = MOTIONTYPE_MOVE;
			nFrameBleand = 10;
		}
	}
	else if (pKeyboard->GetPress(DIK_A))
	{ // Aを押したとき
		// カメラの角度に合わせて、平行移動！
		move.z += cosf(pCameraRot->y + (D3DX_PI * 1.5f)) * PLAYER_SPD;
		move.x += sinf(pCameraRot->y + (D3DX_PI * 1.5f)) * PLAYER_SPD;

		// カメラの角度に合わせて、モデルの目標角度を求める！
		m_rotDest.y = (pCameraRot->y + (D3DX_PI * 0.5f));

		if (m_pMotion->GetType() == MOTIONTYPE_NEUTRAL
			&& m_pMotion->GetBlendType() != MOTIONTYPE_MOVE)
		{ // 移動モーション
			type = MOTIONTYPE_MOVE;
			nFrameBleand = 10;
		}
	}
	else if (pKeyboard->GetPress(DIK_D))
	{ // Dを押したとき
		// カメラの角度に合わせて、平行移動！
		move.z += cosf(pCameraRot->y + (D3DX_PI * 0.5f)) * PLAYER_SPD;
		move.x += sinf(pCameraRot->y + (D3DX_PI * 0.5f)) * PLAYER_SPD;

		// カメラの角度に合わせて、モデルの目標角度を求める！
		m_rotDest.y = (pCameraRot->y + (D3DX_PI * 1.5f));

		if (m_pMotion->GetType() == MOTIONTYPE_NEUTRAL
			&& m_pMotion->GetBlendType() != MOTIONTYPE_MOVE)
		{ // 移動モーション
			type = MOTIONTYPE_MOVE;
			nFrameBleand = 10;
		}
	}
	else if (pJoypad->GetStick(CJoypad::STICK_LEFT, &stick) && Vec3::Length(stick) > STICK_DEADZONE)
	{ // ジョイパッドからの入力で一定以上倒されていれば
		float fAngle = atan2f(stick.x, stick.y);		// スティックの倒された角度
		float fSpeed = Vec3::Length(stick);				// 倒された強さ

		// カメラの角度に合わせて、平行移動！
		move.z += cosf(pCameraRot->y + fAngle) * (PLAYER_SPD * fSpeed);
		move.x += sinf(pCameraRot->y + fAngle) * (PLAYER_SPD * fSpeed);

		// カメラの角度に合わせて、モデルの目標角度を求める！
		m_rotDest.y = (pCameraRot->y + (fAngle + D3DX_PI));

		if (m_pMotion->GetType() == MOTIONTYPE_NEUTRAL
			&& m_pMotion->GetBlendType() != MOTIONTYPE_MOVE)
		{ // 移動モーション
			type = MOTIONTYPE_MOVE;
			nFrameBleand = 10;
		}
	}
	else
	{
		if (m_pMotion->GetType() == MOTIONTYPE_MOVE
			&& m_pMotion->GetBlendType() != MOTIONTYPE_NEUTRAL)
		{ // 移動モーション
			type = MOTIONTYPE_NEUTRAL;
			nFrameBleand = 10;
		}

		bMove = false;			// 操作されていないためfalse
	}

	if (GetRidingObjectX() == nullptr)
	{ // 親が存在しない場合、移動 + モーションを遷移
		m_move = move;

		if (type == MOTIONTYPE_NEUTRAL && bMove == false)
		{ // 待機モーション
			if (m_pMotion->GetType() == MOTIONTYPE_MOVE
				&& m_pMotion->GetBlendType() != MOTIONTYPE_NEUTRAL)
			{ // モーション遷移
				m_pMotion->Set(type, nFrameBleand);
			}
		}
		else
		{ // その他のモーション
			if (m_pMotion->GetType() == MOTIONTYPE_NEUTRAL
				&& m_pMotion->GetBlendType() != MOTIONTYPE_MOVE)
			{ // モーション遷移
				m_pMotion->Set(type, nFrameBleand);
			}
		}
	}
}

//==================================================================================
// --- 電柱関連の入力処理 ---
//==================================================================================
void CPlayer::InputPole(void)
{
	CManager *pManager = CManager::GetInstance();					// マネージャーへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CJoypad *pJoypad = pManager->GetJoypad();						// ジョイパッドへのポインタ
	CPlayerCamera *pPlayerCam = static_cast<CPlayerCamera*>(CCamera::GetCamera(CCamera::TYPE_PLAYER));

	// プレイヤーの行動
	if ((pKeyboard->GetTrigger(DIK_SPACE) || pJoypad->GetTrigger(CJoypad::KEY_A)) && !m_bShotLasso)
	{ // 押された場合
		if (GetRidingObjectX() == nullptr)
		{ // 電柱に載っていない場合
			CObject *pObject = CObject::GetTop(POWERPLANT_PRIORITY);		// 最初のオブジェクト
			float fLengthMin = RIDE_LENGTH;			// 現状最も近い発電所との距離
			CPowerPlant *pPlantNear = nullptr;		// 最も近い発電所へのポインタ

			while (pObject != nullptr)
			{ // オブジェクトを走査
				CObject *pObjectNext = pObject->GetNext();			// 次のオブジェクトへのポインタ

				if (pObject->GetType() == CObject::TYPE_POWERPLANT)
				{ // もしオブジェクトが電柱であれば、ポインタをキャスト
					CPowerPlant *pPlant = static_cast<CPowerPlant*>(pObject);

					Vector3 posPole;			// マトリックスのキャスト用
					Vector3 pos = m_pos;		// マトリックスのキャスト用

					// 各座標をマトリックスでワールド座標に変換
					D3DXVec3TransformCoord(&pos, &pos, &m_mtxWorld);
					D3DXVec3TransformCoord(&posPole, pPlant->GetPosition(), pPlant->GetMatrix());

					// 距離を計算
					float fLength = Vec3::Length(posPole, pos);
					if (fLength < fLengthMin)
					{ // もし前回の距離よりも近いなら、ポインタ保存 + 距離更新
						fLengthMin = fLength;
						pPlantNear = pPlant;
					}
				}

				pObject = pObjectNext;		// ポインタ更新
			}

			if (pPlantNear != nullptr)
			{ // もし乗れる発電所が存在した場合
				// ポインタを保存
				m_pRidingObject = pPlantNear;
				m_pStartPlant = pPlantNear;

				// オフセットを電柱に設定し、マトリックスを設定
				m_pos.y = pPlantNear->GetVtxMax()->y;

				// カメラを電柱上フォーカスに変更
				pPlayerCam->SetState(CPlayerCamera::STATE_RIDING);
			}

			// モーションを終了
			m_pMotion->Set(MOTIONTYPE_NEUTRAL, 10);
		}
		else
		{ // 既に乗っている場合
			// オフセットを元に戻す
			m_pos.y = m_offset.y;

			// 電柱から降りる処理を実行
			DismountPole();

			// コンボリセット
			CCombo *pCombo(pManager->GetScene<CGame>()->GetCombo());		// コンボ表示へのポインタ
			pCombo->Finish();

			// カメラをプレイヤーフォーカスに変更
			pPlayerCam->SetState(CPlayerCamera::STATE_PLAYER);

			// モーションを終了
			m_pMotion->Set(MOTIONTYPE_NEUTRAL, 10);
		}
	}

	if ((pKeyboard->GetTrigger(DIK_RETURN)
		|| pJoypad->GetTrigger(CJoypad::KEY_B)) 
		&& GetRidingObjectX() != nullptr
		&& m_bShotLasso == false)
	{ // 投げ縄を飛ばす
		CObject *pObject = CObject::GetTop(UTILITYPOLE_PRIORITY);		// 最初のオブジェクト
		CUtilityPole *pPoleSelected = nullptr;		// 最も近い電柱へのポインタ

		while (pObject != nullptr)
		{ // オブジェクトを走査
			CObject *pObjectNext = pObject->GetNext();			// 次のオブジェクトへのポインタ

			if (pObject->GetType() == CObject::TYPE_POLE)
			{ // もしオブジェクトが電柱であれば、ポインタをキャスト
				CUtilityPole *pPole = static_cast<CUtilityPole*>(pObject);

				if (pPole->IsSelected() == true)
				{ // もし前回の距離よりも近い場合
					if (m_pRidingObject.index() == 0)
					{ // 発電所に載っている場合、ポインタを保存
						pPoleSelected = pPole;
					}
					else if(m_pRidingObject.index() == 1)
					{ // 電柱に載っている場合、自身の乗っている電柱でなければポインタを保存
						if(std::get<CUtilityPole*>(m_pRidingObject) != pPole) pPoleSelected = pPole;
					}

					break;
				}
			}

			pObject = pObjectNext;		// ポインタ更新
		}

		CGame *pGame = pManager->GetScene(&pGame);		// ゲームシーンへのポインタ
		auto pPlanet = pGame->GetPlanet();				// 惑星へのポインタ

		// 選ばれた電柱が存在すれば
		if (pPoleSelected)
		{ // 投げ縄生成
			Vector3 pos = Vector3(0.0f, m_offset.y + GetRidingObjectX()->GetVtxMax()->y, 0.0f);		// 発生位置
			
			// 投げ縄生成
			CLasso *pLasso = std::visit([&](auto &x)
				{ // マップにどのインデックスのオブジェクトに繋げたのかを追加
					CMap::GetInstance()->AddID(x->GetID());	 

					// 生成した投げ縄へのポインタを返す
					return CLasso::Create(pos, x, pPoleSelected);	
				}, m_pRidingObject);

			// カメラの角度に合わせて、モデルの目標角度を求める！
			m_rotDest.y = (pPlayerCam->GetRotate()->y + D3DX_PI);

			// 投げ縄を投げた為フラグを立てる
			m_bShotLasso = true;
		}
	}
}

//==================================================================================
// --- 位置と移動量の更新処理 ---
//==================================================================================
void CPlayer::UpdatePotision(void)
{ // 移動量の更新
	m_move += (VECTOR3_NULL - m_move) * RESIST_POW;

	CGame *pGame = CManager::GetInstance()->GetScene(&pGame);		// ゲームシーンへのポインタ
	if (pGame != nullptr)
	{ // nullでなければ
		CPlanet *pPlanet = pGame->GetPlanet();		// 惑星へのポインタ

		// 惑星に移動量を設定
		pPlanet->Move(m_rotDest, Vec3::Length(m_move));
	}
}

//==================================================================================
// --- 目標角度への更新処理 ---
//==================================================================================
void CPlayer::UpdateRotateDest(void)
{
	float fAngle = 0.0f;		// 角度修正用変数

	// 角度の修正!
	m_rotDest = Vec3::FixedRotation(m_rotDest);

	// 角度の差を求める!
	fAngle = (m_rotDest.y - m_rot.y);
	if (fAngle < -D3DX_PI)
	{
		fAngle += D3DX_PI * 2.0f;
	}
	else if (fAngle >= D3DX_PI)
	{
		fAngle -= D3DX_PI * 2.0f;
	}

	// 角度の差分だけ角度を徐々に回転する！
	m_rot.y += fAngle * 0.05f;

	// 角度の修正!
	m_rot = Vec3::FixedRotation(m_rot);

	if ((m_rotDest.y - m_rot.y) < 0.001f && (m_rotDest.y - m_rot.y) > -0.001f)
	{ // 角度の差分一定の範囲よりも小さくなった場合、矯正！
		m_rot.y = m_rotDest.y;
	}
}

//==================================================================================
// --- 電柱関連の更新処理 ---
//==================================================================================
void CPlayer::UpdatePole(void)
{
	if (m_pShock->IsLightning() != true
		&& m_bShocked == true
		&& GetRidingObjectX() != nullptr)
	{
		CManager *pManager = CManager::GetInstance();					// マネージャーへのポインタ
		CPlayerCamera *pPlayerCam = static_cast<CPlayerCamera *>(CCamera::GetCamera(CCamera::TYPE_PLAYER));		// プレイヤーカメラへのポインタ

		// オフセットを元に戻す
		m_pos.y = m_offset.y;

		// コンボリセット
		CCombo *pCombo(pManager->GetScene<CGame>()->GetCombo());		// コンボ表示へのポインタ
		pCombo->SetEnablePause(false);
		pCombo->Withdrawal();

		// 電柱から降りる処理を実行
		DismountPole();

		// カメラをプレイヤーフォーカスに変更
		pPlayerCam->SetState(CPlayerCamera::STATE_PLAYER);

		// モーションを終了
		m_pMotion->Set(MOTIONTYPE_NEUTRAL, 10);
		m_bShocked = false;
	}

	// 電柱間の移動処理
	MoveToNextPole();

	// 電柱に載っていられるかの確認処理
	CheckRidingRight();

	// 視点の中央に最も近い電柱の検索
	FindNearestPole();
}

//==================================================================================
// --- 次に乗る電柱に移動する処理 ---
//==================================================================================
void CPlayer::MoveToNextPole(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CGame *pGame = pManager->GetScene(&pGame);			// ゲームシーンへのポインタ

	if (m_pPoleNext != nullptr && pGame != nullptr)
	{ // 次に移動するべき電柱がある場合
		CPlanet *pPlanet = pGame->GetPlanet();		// 惑星へのポインタ
		Quaternion quaMultiply;				// 乗算するクォータニオン
		float fVolume = POLE_MOVE_SPEED;	// 回転する角度

		// フィードバックエフェクトを有効化
		pRenderer->SetEnableFeedBack(true);

		// 残りの角度をオーバーしないか確認
		if (m_fAngleRest - fVolume < 0.0f)
		{ // 角度がオーバーしないように調整
			fVolume -= m_fAngleRest - fVolume;
		}

		// クォータニオンを初期化
		D3DXQuaternionIdentity(&quaMultiply);

		// クォータニオンを生成
		D3DXQuaternionRotationAxis(&quaMultiply,
			&m_vecQua,
			-fVolume);

		// クォータニオンを加算
		pPlanet->MultiplyQuaternion(quaMultiply);

		// 残りの角度が0.0f以下になった場合
		if (m_fAngleRest <= 0.0f)
		{ // その電柱に乗り移る
			m_pRidingObject = m_pPoleNext;

			// オフセットを電柱に設定し、マトリックスを設定
			m_pos.y = GetRidingObjectX()->GetVtxMax()->y;

			// 投げ縄を投げたフラグを下し、乗り移る電柱へのポインタも破棄
			m_bShotLasso = false;
			m_pPoleNext = nullptr;
		}

		m_fAngleRest -= fVolume;
	}
	else
	{ // フィードバックエフェクトを無効化
		pRenderer->SetEnableFeedBack(false);
	}
}

//==================================================================================
// --- 電柱に未だ乗っていられるかの確認処理 ---
//==================================================================================
void CPlayer::CheckRidingRight(void)
{
	if (GetRidingObjectX() == nullptr)
	{ // オブジェクトに乗っていない場合、処理スキップ
		return;
	}

	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CGame *pGame = pManager->GetScene(&pGame);			// ゲームシーンへのポインタ
	CPlayerCamera *pPlayerCam = static_cast<CPlayerCamera *>(CCamera::GetCamera(CCamera::TYPE_PLAYER));		// プレイヤーカメラへのポインタ

	// ゲームシーンの取得
	if (pGame != nullptr)
	{ // 取得成功時
		CCombo *pCombo = pGame->GetCombo();		// コンボ表示へのポインタ
		if (pCombo->GetContinuing() == false
			&& std::holds_alternative<CUtilityPole*>(m_pRidingObject)
			&& m_bShotLasso == false)
		{ // コンボ表示が消えた且つ投げ縄を投げていないなら、電柱から強制的におろす
			// オフセットを元に戻す
			m_pos.y = m_offset.y;

			// 電柱から降りる処理を実行
			DismountPole();

			// コンボリセット
			CCombo *pCombo(pManager->GetScene<CGame>()->GetCombo());		// コンボ表示へのポインタ
			pCombo->Finish();

			// カメラをプレイヤーフォーカスに変更
			pPlayerCam->SetState(CPlayerCamera::STATE_PLAYER);

			// モーションを終了
			m_pMotion->Set(MOTIONTYPE_NEUTRAL, 10);
		}
	}
}

//==================================================================================
// --- 視点の中央に最も近い電柱の検索処理 ---
//==================================================================================
void CPlayer::FindNearestPole(void)
{
	if (GetRidingObjectX() == nullptr)
	{ // オブジェクトに乗っていない場合、処理スキップ
		return;
	}

	CObject *pObject = CObject::GetTop(UTILITYPOLE_PRIORITY);		// 最初のオブジェクト
	Vector3 vecCam = VECTOR3_NULL;			// カメラの方向ベクトル
	Vector3 pos = VECTOR3_NULL;				// プレイヤーの絶対座標
	CUtilityPole *pPoleNear = nullptr;		// 最も画面の中心に近い電柱へのポインタ
	float fLengthMin = 10000.0f;			// 各絶対座標を結んで最小だった値

#ifdef ENABLE_CAN_FOCUS_POLE_VECTOR
	std::vector<CUtilityPole*> vPole;		// プレイヤーがフォーカス可能な電柱へのポインタ

	// 事前にサイズを確保
	vPole.reserve(10);
#endif

	// プレイヤーの絶対座標を求める
	D3DXVec3TransformCoord(&pos, &pos, &m_mtxWorld);

	while (pObject != nullptr)
	{ // オブジェクトを走査
		CObject *pObjectNext = pObject->GetNext();			// 次のオブジェクトへのポインタ
		if (pObject->GetType() != CObject::TYPE_POLE)
		{ // もしオブジェクトが電柱で無ければスキップ
			pObject = pObjectNext;		// ポインタ更新
			continue;
		}

		// ポインタを電柱のポインタにキャスト
		CUtilityPole *pPole = static_cast<CUtilityPole *>(pObject);

		// ポールの選択フラグをおろす
		pPole->SetEnableSelect(false);

		if (GetRidingObjectX() == pPole)
		{ // プレイヤーの乗っている電柱なら、スキップ
			pObject = pObjectNext;		// ポインタ更新
			continue;
		}

		if (pPole->CanFocus(this))
		{ // フォーカス可能なら
			Vector3 posPole = VECTOR3_NULL;	// 電柱のの上の絶対座標
			float fLength = 0.0f;	// 二点の距離
			CRay ray;				// プレイヤーから電柱への光線

			// 電柱の上の絶対座標を求める
			posPole.y = pPole->GetVtxMax()->y;
			D3DXVec3TransformCoord(&posPole, &posPole, pPole->GetMatrix());

			// 二点間の長さを求める
			fLength = Vec3::Length(pos, posPole);
			if (fLength <= fLengthMin)
			{ // 今までで一番小さいなら、距離とポインタを保存
				fLengthMin = fLength;
				pPoleNear = pPole;
			}

#ifdef ENABLE_CAN_FOCUS_POLE_VECTOR
			// 配列に追加
			vPole.push_back(pPole);
#endif
#ifdef ENABLE_RAY_PLAYER_TO_POLE
			// レイを作成して描画
			pos.y = m_offset.y + pPole->GetVtxMax()->y;
			ray = CRay(pos, posPole);
			ray.Draw();
#endif
		}

		pObject = pObjectNext;		// ポインタ更新
	}

	if (pPoleNear != nullptr)
	{ // 中心に近い電柱が存在するなら、選択
		pPoleNear->SetEnableSelect(true);
	}
}

//==================================================================================
// --- 乗っている電柱から降りる際の惑星角度修正処理 ---
//==================================================================================
void CPlayer::DismountPole(void)
{
	CGame *pGame = CManager::GetInstance()->GetScene(&pGame);		// シーンの取得
	CPlanet *pPlanet = pGame->GetPlanet();			// 惑星の取得

	// 惑星から各電柱の座標を求め、内積を求める
	// 内積の角度分、次の電柱への角度へ回転させる
	Vector3 planetToPlayer = VECTOR3_NULL;		// 惑星からプレイヤーへのベクトル
	Vector3 planetToRiding = VECTOR3_NULL;		// 惑星からプレイヤーの乗っていた電柱へのベクトル
	Vector3 playerWorldPos = m_pos;				// 降りた後の自身の絶対座標(プレイヤーのXZ座標は動かない為posを代入)
	Vector3 ridingWorldPos = GetRidingObjectX()->GetWorldPosition();		// 乗っていた電柱の絶対座標
	Vector3 vecQua;			// 任意軸
	Quaternion quaMultiply;	// 乗算するクォータニオン

	// 各ベクトルを求める
	planetToRiding = Vec3::Direction(ridingWorldPos, *pPlanet->GetPosition());
	planetToPlayer = Vec3::Direction(m_pos, *pPlanet->GetPosition());

	// ベクトルから内積計算
	float fDot = Vec3::Dot(planetToRiding, planetToPlayer);

	// 内積から二つのベクトルの角度差分を求める
	float fValue = std::acosf(std::clamp(fDot, -1.0f, 1.0f));

	// 自身から乗っていた電柱へのベクトルを計算して向きを求める
	float fAngle = Vec2::Direction(Vec3::ToVector2(ridingWorldPos, Vec3::Axis::Y),
		Vec3::ToVector2(playerWorldPos, Vec3::Axis::Y));

	// 任意軸を右に回転
	fAngle += HALF_PI;
	fAngle = Util::FixedRotation(fAngle);

	vecQua = Vec2::ToVector3(Vec2::Direction(fAngle));
	vecQua.z = vecQua.y;
	vecQua.y = 0.0f;

	// クォータニオンを初期化
	D3DXQuaternionIdentity(&quaMultiply);

	// クォータニオンを生成
	D3DXQuaternionRotationAxis(&quaMultiply,
		&vecQua,
		-fValue);

	// クォータニオンを加算
	pPlanet->MultiplyQuaternion(quaMultiply);

	// 電柱をnullに変更
	m_pRidingObject = static_cast<CUtilityPole*>(nullptr);
}

//==================================================================================
// --- 当たり判定関連処理 ---
//==================================================================================
void CPlayer::CollisionAction(void)
{ // TODO : 使わない場合はこの関数を削除すること

}

//==================================================================================
// --- 自身以外の更新処理 ---
//==================================================================================
void CPlayer::OtherUpdate(void)
{
	for (auto &model : m_vpModel)
	{ // 各モデルの更新
		model->Update();
	}

	// モーションの更新
	m_pMotion->Update();
}

//==================================================================================
// --- variantの持つポインタの親クラスのポインタへの変換処理 ---
//==================================================================================
CObjectXQuaternion *CPlayer::GetRidingObjectX(void)
{
	return std::visit([&](auto &x) { return static_cast<CObjectXQuaternion*>(x); }, m_pRidingObject);
}