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
#include "model.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "input.h"
#include "joypad.h"
#include "playerCamera.h"
#include "debugproc.h"
#include "meshField.h"
#include "matrix.h"
#include "filestream.h"
#include "motion.h"
#include "utilityPole.h"
#include "map.h"
#include "building.h"
#include "planet.h"
#include "ray.h"
#include "util.h"
#include "lasso.h"
#include "vec2math.h"
#include "vec3math.h"
#include <string>
#include <vector>
#include <algorithm>
#include <regex>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define PLAYER_SPD			(1.75f)		// モデルの移動スピード
#define PLAYER_ROTSPD		(0.1f)		// モデルの回転スピード
#define STICK_DEADZONE		(0.05f)		// 動いたと感知するデッドゾーン
#define RESIST_POW			(0.25f)		// 摩擦
#define RIDE_LENGTH			(120.0f)	// 電柱に乗れる距離
#define PLAYERCAM_DEFROT	Vector3(0.0f, 0.0f, -0.4f)		// デフォルトのカメラ角度
#define PLAYERCAM_RIDINGROT	Vector3(0.0f, 0.0f, -1.16f)		// 電柱に乗っているときのカメラ角度
#define PLAYERCAM_LEN			(1000.0f)		// プレイヤーのカメラの距離
#define PLAYERCAM_RIDING_LEN	(250.0f)		// 電柱に乗っているときのカメラの距離
#define ENABLE_RAY_PLAYER_TO_POLE		// プレイヤーから投げ縄を投げられる電柱へのレイの表示

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPlayer *CPlayer::Create(const char *pXFileName, const Vector3 &pos, const Vector3 &rot)
{
	CPlayer *pPlayer = new CPlayer;		// 生成したオブジェクトへのポインタ
	NULLPOINTER_ASSERT(pPlayer);

	if (pPlayer != nullptr)
	{ // 初期化処理
		pPlayer->Init(pXFileName, pos, rot);
	}

#pragma region std::regex test
#if 0
	constexpr const char *floatRegex = R"([-+]?[0-9]*\.?[0-9]+)";
	char aStr[] = "1.0, 3.14, 1.00";
	std::regex rotRegex(floatRegex);
	std::cregex_iterator regIter(std::cbegin(aStr), std::cend(aStr), rotRegex);
	std::cregex_iterator end;
	std::cmatch result;
	Vector3 rotReg;
	int n = 0;

	while (regIter != end)
	{
		auto &&result = *regIter;
		float fValue = static_cast<float>(std::atof(result.str().c_str()));

		Vec3::InAxis(rotReg, static_cast<Vec3::Axis>(n), fValue);
		n++;
		regIter++;
	}
#endif
#pragma endregion

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
	m_pMotion = nullptr;
	m_pThunderEffect = nullptr;
	m_pRidingPole = nullptr;
	m_pPoleNext = nullptr;
	m_bShotLasso = false;

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
{
	// 引数の値を保存
	m_pos = pos;
	m_offset = pos;
	m_rot = rot;

	// モーションを生成
	m_pMotion = std::make_unique<CMotion>();
	NULLPOINTER_ASSERT(m_pMotion);

	if (m_pMotion == nullptr) return E_FAIL;		// 生成失敗

	// モーションの初期化
	m_pMotion->Init();

	// ファイル読み込み
	LoadFile(pFileName);

	// モーションの設定
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
	for (int nCntModel = 0; nCntModel < MAX_PLAYER_MODEL_NUM; nCntModel++)
	{ // 各モデルを破棄
		// nullptrならスキップ
		if (m_apModel[nCntModel] == nullptr) continue;

		// モデルの破棄 + 終了処理
		m_apModel[nCntModel]->Uninit();
		m_apModel[nCntModel].reset();
	}
	
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

#ifndef	ENABLE_PLANET
	// 重力
	m_pos.y -= 0.05f;
#endif
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
	const Matrix *pMtxParent = (m_pRidingPole == nullptr) ? nullptr : m_pRidingPole->GetMatrix();
	Mtx::CalcWorld(&m_mtxWorld, pMtxParent, m_pos, m_rot);

	//  ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{ // 各モデルの描画
		m_apModel[nCntModel]->Draw();
	}
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

	// マップエディタ関連の入力
	InputMap();
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
	MOTIONTYPE type = MOTIONTYPE_NEUTRAL;			// モーションタイプ
	int nFrameBleand = 0;							// ブレンド時間
	bool bMove = true;			// 動いたか

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

	if (m_pRidingPole == nullptr)
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
	if ((pKeyboard->GetTrigger(DIK_BACK) || pJoypad->GetTrigger(CJoypad::KEY_A)) && !m_bShotLasso)
	{ // 押された場合
		if (m_pRidingPole == nullptr)
		{ // 電柱に載っていない場合
			CObject *pObject = CObject::GetTop(UTILITYPOLE_PRIORITY);		// 最初のオブジェクト
			float fLengthMin = RIDE_LENGTH;			// 現状最も近い電柱との距離
			CUtilityPole *pPoleNear = nullptr;		// 最も近い電柱へのポインタ

			while (pObject != nullptr)
			{ // オブジェクトを走査
				CObject *pObjectNext = pObject->GetNext();			// 次のオブジェクトへのポインタ

				if (pObject->GetType() == CObject::TYPE_POLE)
				{ // もしオブジェクトが電柱であれば、ポインタをキャスト
					CUtilityPole *pPole = static_cast<CUtilityPole *>(pObject);

					Vector3 posPole;			// マトリックスのキャスト用
					Vector3 pos = m_pos;		// マトリックスのキャスト用

					// 各座標をマトリックスでワールド座標に変換
					D3DXVec3TransformCoord(&pos, &pos, &m_mtxWorld);
					D3DXVec3TransformCoord(&posPole, pPole->GetPosition(), pPole->GetMatrix());

					// 距離を計算
					float fLength = Vec3::Length(posPole, pos);
					if (fLength < fLengthMin)
					{ // もし前回の距離よりも近いなら、ポインタ保存 + 距離更新
						fLengthMin = fLength;
						m_pRidingPole = pPole;
					}
				}

				pObject = pObjectNext;		// ポインタ更新
			}

			if (m_pRidingPole != nullptr)
			{ // もし乗れるポールが存在した場合
				// オフセットを電柱に設定し、マトリックスを設定
				m_pos.y = m_pRidingPole->GetVtxMax()->y;

				// カメラを電柱上フォーカスに変更
				pPlayerCam->SetState(CPlayerCamera::STATE_RIDING);
			}

			// モーションを終了
			m_pMotion->Set(MOTIONTYPE_NEUTRAL, 10);
		}
		else
		{ // 既に乗っている場合
			// オフセットを元に戻し、親マトリックスを破棄
			m_pos.y = m_offset.y;
			m_pRidingPole = nullptr;

			// カメラをプレイヤーフォーカスに変更
			pPlayerCam->SetState(CPlayerCamera::STATE_PLAYER);

			// モーションを終了
			m_pMotion->Set(MOTIONTYPE_NEUTRAL, 10);
		}
	}

	if ((pKeyboard->GetTrigger(DIK_RETURN)
		|| pJoypad->GetTrigger(CJoypad::KEY_B)) 
		&& m_pRidingPole != nullptr
		&& m_bShotLasso == false)
	{ // 投げ縄を飛ばす
		CObject *pObject = CObject::GetTop(UTILITYPOLE_PRIORITY);		// 最初のオブジェクト
		CUtilityPole *pPoleSelected = nullptr;		// 最も近い電柱へのポインタ

		while (pObject != nullptr)
		{ // オブジェクトを走査
			CObject *pObjectNext = pObject->GetNext();			// 次のオブジェクトへのポインタ

			if (pObject->GetType() == CObject::TYPE_POLE)
			{ // もしオブジェクトが電柱であれば、ポインタをキャスト
				CUtilityPole *pPole = static_cast<CUtilityPole *>(pObject);

				if (pPole->IsSelected() == true && pPole != m_pRidingPole)
				{ // もし前回の距離よりも近いなら、ポインタ保存 + 距離更新
					pPoleSelected = pPole;
					break;
				}
			}

			pObject = pObjectNext;		// ポインタ更新
		}

		CGame *pGame = pManager->GetScene<CGame>();		// ゲームシーンへのポインタ
		auto pPlanet = pGame->GetPlanet();				// 惑星へのポインタ
		Vector3 vecQua;		// 任意軸
		float fAngle;			// 角度

		// クォータニオンから軸と角度を求める
		D3DXQuaternionToAxisAngle(pPlanet->GetQuaternion(),
			&vecQua,
			&fAngle);

		// 角度反転
		fAngle *= -1;

		// 選ばれた電柱が存在すれば
		if (pPoleSelected)
		{ // 投げ縄生成
			CLasso *pLasso = CLasso::Create(Vector3(0.0f, m_offset.y + m_pRidingPole->GetVtxMax()->y, 0.0f),
				m_pRidingPole, 
				pPoleSelected);
			pLasso->SetParent(pPlanet->GetMatrix());

			// カメラの角度に合わせて、モデルの目標角度を求める！
			m_rotDest.y = (pPlayerCam->GetRotate()->y + D3DX_PI);

			// 投げ縄を投げた為フラグを立てる
			m_bShotLasso = true;
		}
	}
}

//==================================================================================
// --- マップ関連の入力処理 ---
//==================================================================================
void CPlayer::InputMap(void)
{
	CManager *pManager = CManager::GetInstance();					// マネージャーへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CMap *pMap = CMap::GetInstance();						// マップへのポインタ
	Vector3 pos = Vector3(0.0f, m_pos.y, 0.0f);		// 設置位置

	if (pKeyboard->GetTrigger(DIK_1))
	{ // 建物0生成
		pMap->AddBulding(CBuilding::TYPE_0, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_2))
	{ // 建物1生成
		pMap->AddBulding(CBuilding::TYPE_1, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_3))
	{ // 建物2生成
		pMap->AddBulding(CBuilding::TYPE_2, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_4))
	{ // 建物3生成
		pMap->AddBulding(CBuilding::TYPE_3, pos);
	}
	else if (pKeyboard->GetTrigger(DIK_5))
	{ // 電柱生成
		pMap->AddUtilityPole(pos);
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
	if (m_pRidingPole != nullptr)
	{ // 電柱に乗っている場合
		if (m_pPoleNext != nullptr)
		{ // 次に移動するべき電柱がある場合
			CPlanet *pPlanet = CManager::GetSceneByInstance<CGame>()->GetPlanet();
			
			// 惑星から各電柱の座標を求め、内積を求める
			// 内積の角度分、次の電柱への角度へ回転させる
			
			// ※ 絶対座標
			Vector3 planetToRiding = VECTOR3_NULL;		// 惑星からプレイヤーの乗っている電柱へのベクトル
			Vector3 planetToNext = VECTOR3_NULL;		// 惑星から次に乗る予定の電柱へのベクトル
			Vector3 vecQua = VECTOR3_NULL;				// 任意軸ベクトル
			Vector3 ridingWorldPos = m_pRidingPole->GetWorldPosition();		// 乗っている電柱の絶対座標
			Vector3 nextWorldPos = m_pPoleNext->GetWorldPosition();			// 次に乗る予定の電柱の絶対座標

			// ベクトルを求める
			planetToRiding = Vec3::Direction(ridingWorldPos, *pPlanet->GetPosition());
			CRay(*pPlanet->GetPosition(), planetToRiding, 10000.0f).Draw();

			planetToNext = Vec3::Direction(nextWorldPos, *pPlanet->GetPosition());
			CRay(*pPlanet->GetPosition(), planetToNext, 10000.0f).Draw();

			// ベクトルから内積計算
			float fDot = Vec3::Dot(planetToRiding, planetToNext);
			
			float fRadian = std::acosf(std::clamp(fDot, -1.0f, 1.0f));

			// 乗っている電柱から次に乗る電柱へのベクトルを計算して向きを求める
			float fAngle = Vec2::Direction(Vec3::ToVector2(nextWorldPos, Vec3::Axis::Y),
				Vec3::ToVector2(ridingWorldPos, Vec3::Axis::Y));

			fAngle += HALF_PI;
			fAngle = Util::FixedRotation(fAngle);

			vecQua = Vec2::ToVector3(Vec2::Direction(fAngle));
			vecQua.z = vecQua.y;
			vecQua.y = 0.0f;
			CRay(m_offset + Vector3(0.0f, 100.0f, 0.0f), vecQua, 1000.0f).Draw();

			Quaternion quaPlus;		// 加算するクォータニオン

			// クォータニオンを初期化
			D3DXQuaternionIdentity(&quaPlus);

			// クォータニオンを生成
			D3DXQuaternionRotationAxis(&quaPlus,
				&vecQua,
				-fRadian);

			// クォータニオンを加算
			pPlanet->AddQuaternion(quaPlus);

			// その電柱に乗り移る
			m_pRidingPole = m_pPoleNext;

			m_bShotLasso = false;
			m_pPoleNext = nullptr;
		}

		CCamera *pPlayerCam = CCamera::GetCamera(CCamera::TYPE_PLAYER);		// プレイヤーカメラへのポインタ
		CObject *pObject = CObject::GetTop(UTILITYPOLE_PRIORITY);		// 最初のオブジェクト
		Vector3 vecCam = VECTOR3_NULL;				// カメラの方向ベクトル
		Vector3 vecPlayerToPole = VECTOR3_NULL;		// プレイヤーから電柱への方向ベクトル
		float fLengthMin = RIDE_LENGTH;			// 現状最も近い電柱との距離
		CUtilityPole *pPoleNear = nullptr;		// 最も画面の中心に近い電柱へのポインタ
		float fDotMax = 0.0f;					// 内積の最小値

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

				if (m_pRidingPole != pPole && pPole->GetIconType() == CUtilityPole::ICON_CAN)
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

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{ // 各モデルの更新
		m_apModel[nCntModel]->Update();
	}

	// モーションの更新
	m_pMotion->Update();
}

//==================================================================================
// --- ファイルの読み込み処理 ---
//==================================================================================
HRESULT CPlayer::LoadFile(const char *pFilename)
{ // TODO : この処理はモーションクラス側に持たせるべき
	CFileStream *pFile = nullptr;		// ファイルの入出力クラス
	bool bResult = true;				// 処理結果
	std::string line;					// 読み取った一行
	char aStr[MAX_PATH] = {};			// パース用文字列
	int nNumModel = 0;					// 読み込むモデルパス数
	int nCntModel = 0;					// 読み込んだモデルパス数

	// ファイル入出力オブジェクト作成
	pFile = new CFileStream;
	if (pFile == nullptr) return E_FAIL;

	// ファイルを開く
	bResult = pFile->OpenFile(pFilename, false);
	if (bResult == false)
	{ // ファイルオープンに失敗
		return E_FAIL;
	}

	while (1)
	{ // SCRIPTの走査ループ
		// 一行読み取る
		pFile->Read(line);

		// その行をコピーして、コメントを消去
		strcpy(aStr, line.c_str());
		if (DeleteComment(aStr))
		{ // 読み取れる場合
			if (strcmp(aStr, "SCRIPT") == 0)
			{ // 見つかった場合、読み込み開始
				break;
			}
			else if (pFile->IsEoF() == true)
			{ // 見つからず終端に着いた場合、読み込み失敗
				return E_FAIL;
			}
		}
	}

	while (1)
	{ // 読み込みループ
		// 一行読み取る
		pFile->Read(line);

		// その行をコピーして、コメントを消去
		strcpy(aStr, line.c_str());
		if (DeleteComment(aStr))
		{ // 読み取れる場合
			if (strcmp(aStr, "END_SCRIPT") == 0)
			{ // モーションにモデルへのポインタと総数を設定
				m_pMotion->SetModel(m_apModel, m_nNumModel);
				break;
			}
			else if (strcmp(aStr, "CHARACTERSET") == 0)
			{ // キャラクターデータ読み込み
				LoadCharactorData(pFile);
			}
			else if (strstr(aStr, "NUM_MODEL") != nullptr)
			{ // モデルパス数読み込み
				LoadData(aStr, "%d", &nNumModel);
			}
			else if (strstr(aStr, "MODEL_FILENAME") != nullptr)
			{ // モデルパス読み込み
				if (nCntModel <= nNumModel)
				{ // パス数分を超えていなければ読み込み
					LoadData(aStr, "%s", &m_aModelPath[nCntModel][0]);
					nCntModel++;
				}
			}
			else if (strcmp(aStr, "MOTIONSET") == 0)
			{ // モーション情報の読み込み
				LoadMotionData(pFile);
			}
		}
	}

	// ファイル入出力オブジェクトを破棄
	pFile->CloseFile();
	delete pFile;
	pFile = nullptr;

	return S_OK;
}

//==================================================================================
// --- キャラクターデータの読み込み処理 ---
//==================================================================================
void CPlayer::LoadCharactorData(CFileStream *pFile)
{
	std::string line;					// 読み取った一行
	char aStr[MAX_PATH] = {};			// パース用文字列
	int nCntModel = 0;					// 生成したモデル数

	while (1)
	{ // 読み込みループ
		// 一行読み取る
		pFile->Read(line);

		// その行をコピーして、コメントを消去
		strcpy(aStr, line.c_str());
		if (DeleteComment(aStr))
		{ // 読み取れる場合
			if (strcmp(aStr, "END_CHARACTERSET") == 0)
			{ // 読み込み終了
				break;
			}
			else if (strcmp(aStr, "PARTSSET") == 0)
			{ // パーツデータの読み込み
				LoadPartsData(pFile, nCntModel);
				nCntModel++;		// モデル数増加
			}
			else if(strstr(aStr, "NUM_PARTS") != nullptr)
			{ // パーツ数の読み込み
				LoadData(aStr, "%d", &m_nNumModel);
			}
		}
	}
}

//==================================================================================
// --- パーツデータの読み込み処理 ---
//==================================================================================
void CPlayer::LoadPartsData(CFileStream *pFile, const int nCntModel)
{
	std::string line;					// 読み取った一行
	char aStr[MAX_PATH] = {};			// パース用文字列
	int nIdxParent = -1;				// 親モデルのインデックス
	int nIdxModel = -1;					// モデルのインデックス
	Vector3 pos = VECTOR3_NULL;		// オフセット位置
	Vector3 rot = VECTOR3_NULL;		// 角度

	while (1)
	{ // 読み込みループ
		// 一行読み取る
		pFile->Read(line);

		// その行をコピーして、コメントを消去
		strcpy(aStr, line.c_str());
		if (DeleteComment(aStr))
		{ // 読み取れる場合
			if (strcmp(aStr, "END_PARTSSET") == 0)
			{ // 読み込み終了 + モデルの作成
				if (nIdxModel >= 0 && nIdxModel < MAX_PLAYER_MODEL_PATH)
				{ // インデックスが範囲内の場合作成
					m_apModel[nCntModel].reset(CModel::Create(&m_aModelPath[nIdxModel][0],
						pos,
						rot));

					if (nIdxParent != -1)
					{ // 生成したモデルに親マトリックスを設定
						m_apModel[nCntModel]->SetParent(m_apModel[nIdxParent].get());
					}
				}

				break;
			}
			else if (strstr(aStr, "INDEX") != nullptr)
			{ // モデルのインデックス読み込み
				LoadData(aStr, "%d", &nIdxModel);
			}
			else if (strstr(aStr, "PARENT") != nullptr)
			{ // 親モデルのインデックス読み込み
				LoadData(aStr, "%d", &nIdxParent);
			}
			else if (strstr(aStr, "POS") != nullptr)
			{ // オフセット座標読み込み
				LoadData(aStr, "%f %f %f", &pos.x, &pos.y, &pos.z);
			}
			else if (strstr(aStr, "ROT") != nullptr)
			{ // 角度読み込み
				LoadData(aStr, "%f %f %f", &rot.x, &rot.y, &rot.z);
			}
		}
	}
}

//==================================================================================
// --- モーション情報の読み込み処理 ---
//==================================================================================
void CPlayer::LoadMotionData(CFileStream* pFile)
{
	std::string line;					// 読み取った一行
	char aStr[MAX_PATH] = {};			// パース用文字列
	CMotion::INFO info = {};			// モーション情報
	int nKeyInfo = 0;		// 現在設定しているキー情報の番号
	int nKey = 0;			// 現在設定しているキー要素の番号
	int nLoop = 0;			// 読み込んだループの有無

	while (1)
	{ // モーションの読み込み
		// 文字列初期化
		line.clear();

		// 一行読み取る
		pFile->Read(line);

		// その行をコピーして、コメントを消去
		strcpy(aStr, line.c_str());

		// 読み取れないならスキップ
		if (!DeleteComment(aStr)) continue;

		if (strcmp(aStr, "END_MOTIONSET") == 0)
		{ // モーション情報の読み込み終了
			// モーション情報の設定及び番号の初期化
			m_pMotion->SetInfo(info);
			nKeyInfo = 0;
			nKey = 0;
			break;
		}
		else if (strcmp(aStr, "KEYSET") == 0)
		{ // キー情報の設定
			while (1)
			{ // 読み込みループ
				// 文字列初期化
				line.clear();

				// 一行読み取る
				pFile->Read(line);

				// その行をコピーして、コメントを消去
				strcpy(aStr, line.c_str());

				// 読み取れないならスキップ
				if (!DeleteComment(aStr)) continue;

				if (strcmp(aStr, "END_KEYSET") == 0)
				{ // キー情報の読み込み終了
					// キー情報の番号を進め、キー要素の番号をリセット
					nKeyInfo++;
					nKey = 0;
					break;
				}
				else if (strcmp(aStr, "KEY") == 0)
				{ // キー情報の設定
					while (1)
					{ // 読み込みループ
						// 文字列初期化
						line.clear();

						// 一行読み取る
						pFile->Read(line);

						// その行をコピーして、コメントを消去
						strcpy(aStr, line.c_str());

						// 読み取れないならスキップ
						if (!DeleteComment(aStr)) continue;

						if (strcmp(aStr, "END_KEY") == 0)
						{ // キー要素の読み込み終了
							// キー要素の番号を進める
							nKey++;
							break;
						}
						else if (strstr(aStr, "POS") != nullptr)
						{ // 位置の読み込み
							LoadData(aStr, "%f %f %f",
								&info.aKeyInfo[nKeyInfo].aKey[nKey].pos.x,
								&info.aKeyInfo[nKeyInfo].aKey[nKey].pos.y,
								&info.aKeyInfo[nKeyInfo].aKey[nKey].pos.z);
						}
						else if (strstr(aStr, "ROT") != nullptr)
						{ // 角度の読み込み
							LoadData(aStr, "%f %f %f",
								&info.aKeyInfo[nKeyInfo].aKey[nKey].rot.x,
								&info.aKeyInfo[nKeyInfo].aKey[nKey].rot.y,
								&info.aKeyInfo[nKeyInfo].aKey[nKey].rot.z);
						}
					}
				}
				else if (strstr(aStr, "FRAME") != nullptr)
				{ // 再生フレーム数の読み込み
					LoadData(aStr, "%d", &info.aKeyInfo[nKeyInfo].nFrame);
				}
			}
		}
		else if (strstr(aStr, "LOOP") != nullptr)
		{ // ループの有無の読み込み
			LoadData(aStr, "%d", &nLoop);

			// 1以上ならループ有
			info.bLoop = nLoop > 0;
		}
		else if (strstr(aStr, "NUM_KEY") != nullptr)
		{ // キー情報の総数のの読み込み
			LoadData(aStr, "%d", &info.nNumKey);
		}
	}
}

//==================================================================================
// --- #のコメント消去処理 ---
//==================================================================================
bool CPlayer::DeleteComment(char *pStr)
{
	std::string line;		// パース用文字列

	// nullptrの場合、スキップ
	if (pStr == nullptr) return false;

	// 引数の文字列を代入
	line.append(pStr);

	while (1)
	{ // #がなくなるまで走査
		auto Iter = std::find(line.begin(), line.end(), '#');
		if (Iter == line.cbegin())
		{ // 一文字目なら、読み込みスキップ
			return false;
		}
		else if (Iter != line.cend())
		{ // 途中にあるなら、それ以降の文字列をスキップ
			line.erase(Iter, line.cend());
		}
		else
		{ // 見つからなかった場合、終了
			break;
		}
	}

	while (1)
	{ // タブスペースがなくなるまで走査
		auto Iter = std::find(line.begin(), line.end(), '\t');
		if (Iter == line.cend())
		{ // 見つからなかった場合、終了
			break;
		}
		else
		{ // 見つかった場合、消去
			line.erase(Iter);
		}
	}

	// パース後の文字列を引数にコピー
	strcpy(pStr, line.c_str());

	// 読み込み開始
	return true;
}