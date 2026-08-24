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
#include "ray.h"
#include "util.h"
#include "lasso.h"
#include "combo.h"
#include "vec2math.h"
#include "vec3math.h"
#include "observer_pointer.h"
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
//#define ENABLE_RAY_PLAYER_TO_POLE		// プレイヤーから投げ縄を投げられる電柱へのレイの表示

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
	m_pThunderEffect = nullptr;
	m_pPoleNext = nullptr;
	m_bShotLasso = false;
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

	// モーションを生成
	CMotionLoader *pMotionLoader = CMotionLoader::GetInstance();
	m_pMotion = pMotionLoader->CreateMotion(pMotionLoader->Register(pFileName));
	NULLPOINTER_ASSERT(m_pMotion);

	if (m_pMotion == nullptr) return E_FAIL;		// 生成失敗

	CPartsLoader *pPartsLoader = CPartsLoader::GetInstance();
	m_vpModel = pPartsLoader->CreateParts(pPartsLoader->Register(pFileName));

	// モーションの設定
	m_pMotion->SetModel(m_vpModel, m_vpModel.size());
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
		model->Draw();
	}
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
	own::ObserverPtr pCombo(pGame->GetCombo());		// コンボ表示へのポインタ
	pCombo->AddCombo(1);
}

//==================================================================================
// --- Input時関連処理 ---
//==================================================================================
void CPlayer::InputAction(void)
{
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
			pCombo->ResetCombo();

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

		CGame *pGame = pManager->GetScene<CGame>();		// ゲームシーンへのポインタ
		auto pPlanet = pGame->GetPlanet();				// 惑星へのポインタ

		// 選ばれた電柱が存在すれば
		if (pPoleSelected)
		{ // 投げ縄生成
			Vector3 pos = Vector3(0.0f, m_offset.y + GetRidingObjectX()->GetVtxMax()->y, 0.0f);		// 発生位置
			
			// 投げ縄生成
			CLasso *pLasso = std::visit([&](auto &x) { return CLasso::Create(pos, x, pPoleSelected); }, m_pRidingObject);
			pLasso->SetParent(pPlanet->GetMatrix());

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
			CPlayerCamera *pPlayerCam = static_cast<CPlayerCamera*>(CCamera::GetCamera(CCamera::TYPE_PLAYER));
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

	if (GetRidingObjectX() != nullptr)
	{ // オブジェクトに乗っている場合
		CPlayerCamera *pPlayerCam = static_cast<CPlayerCamera*>(CCamera::GetCamera(CCamera::TYPE_PLAYER));		// プレイヤーカメラへのポインタ
		CObject *pObject = CObject::GetTop(UTILITYPOLE_PRIORITY);		// 最初のオブジェクト
		Vector3 vecCam = VECTOR3_NULL;				// カメラの方向ベクトル
		Vector3 vecPlayerToPole = VECTOR3_NULL;		// プレイヤーから電柱への方向ベクトル
		float fLengthMin = RIDE_LENGTH;			// 現状最も近い電柱との距離
		CUtilityPole *pPoleNear = nullptr;		// 最も画面の中心に近い電柱へのポインタ
		float fDotMax = 0.0f;					// 内積の最小値

		// ゲームシーンの取得
		if (pGame != nullptr)
		{ // 取得成功時
			CCombo *pCombo = pGame->GetCombo();		// コンボ表示へのポインタ
			if (pCombo->GetDisp() == false
				&& m_pRidingObject.index() == CUTILITYPOLE_PTR
				&& m_bShotLasso == false)
			{ // コンボ表示が消えた且つ投げ縄を投げていないなら、電柱から強制的におろす
				// オフセットを元に戻す
				m_pos.y = m_offset.y;

				// 電柱から降りる処理を実行
				DismountPole();

				// コンボリセット
				CCombo *pCombo(pManager->GetScene<CGame>()->GetCombo());		// コンボ表示へのポインタ
				pCombo->ResetCombo();

				// カメラをプレイヤーフォーカスに変更
				pPlayerCam->SetState(CPlayerCamera::STATE_PLAYER);

				// モーションを終了
				m_pMotion->Set(MOTIONTYPE_NEUTRAL, 10);
				return;
			}
		}

		// カメラの方向ベクトルを求める
		vecCam = pPlayerCam->GetRay();

		// XZ平面で計算する為Y軸のベクトルは0に設定
		vecCam.y = 0.0f;

		while (pObject != nullptr)
		{ // オブジェクトを走査
			CObject *pObjectNext = pObject->GetNext();			// 次のオブジェクトへのポインタ

			if (pObject->GetType() == CObject::TYPE_POLE)
			{ // もしオブジェクトが電柱であれば、ポインタをキャスト
				CUtilityPole *pPole = static_cast<CUtilityPole *>(pObject);
				Vector3 posPole = VECTOR3_NULL;	// マトリックスのキャスト用
				Vector3 pos = VECTOR3_NULL;		// マトリックスのキャスト用
				CRay ray;				// プレイヤーから電柱への光線
				float fDot = 0.0f;		// 内積結果

				// ポールの選択フラグをおろす
				pPole->SetEnableSelect(false);

				if (GetRidingObjectX() != pPole && pPole->GetIconType() == CUtilityPole::ICON_CAN)
				{ // プレイヤーの乗っている電柱ではなく、選択可能なら計算開始
					// 各座標をマトリックスでワールド座標に変換
					posPole.y = pPole->GetVtxMax()->y;
					D3DXVec3TransformCoord(&pos, &m_pos, &m_mtxWorld);
					D3DXVec3TransformCoord(&posPole, &posPole, pPole->GetMatrix());

					// プレイヤーから電柱への方向ベクトルを求める
					vecPlayerToPole = Vec3::Direction(posPole, pos);

					// XZ平面で計算する為Y軸のベクトルは0に設定
					vecPlayerToPole.y = 0.0f;

#ifdef ENABLE_RAY_PLAYER_TO_POLE
					// レイを作成して描画
					pos.y = m_offset.y + m_pRidingPole->GetVtxMax()->y;
					ray = CRay(pos, posPole);
					ray.Draw();
#endif
					// 二つのベクトルから内積を求める
					fDot = Vec3::Dot(vecCam, vecPlayerToPole);
					if (fDot >= fDotMax)
					{ // 現在の最小内積結果より小さかった場合、ポインタと結果を保存
						pPoleNear = pPole;
						fDotMax = fDot;
					}
				}
			}

			pObject = pObjectNext;		// ポインタ更新
		}

		if (pPoleNear != nullptr)
		{ // 中心に近い電柱が存在するなら、選択
			pPoleNear->SetEnableSelect(true);
		}
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
	CCamera *pPlayerCam = CCamera::GetCamera(CCamera::TYPE_PLAYER);		// プレイヤーカメラへのポインタ

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