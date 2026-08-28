//==================================================================================
// 
// 電柱クラスのヘッダーファイル [utilityPole.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/9
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "utilityPole.h"
#include "powerPlant.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "electricalCable.h"
#include "electricCurrent.h"
#include "objectBillboard3D.h"
#include "effect.h"
#include "player.h"
#include "planet.h"
#include "camera.h"
#include "vec3math.h"
#include <algorithm>
#include <ranges>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define POLE_PATH			"data/MODEL/utilityPole.x"		// 電柱のパス
#define DEF_ICON_SIZE		Vector2(50.0f, 50.0f)		// アイコンサイズ
#define SELECT_ICON_SIZE	(DEF_ICON_SIZE * 2.0f)			// 最も近いアイコンのサイズ
#define ICON_CAN_PATH		"data/TEXTURE/LOGO_CAN.png"		// 可能アイコンへのパス
#define ICON_CANT_PATH		"data/TEXTURE/LOGO_CANT.png"	// 不可能アイコンへのパス
#define CONNECT_LENGTH		(700.0f)			// 繋げられる電柱との長さの限界

//==================================================================================
// --- オブジェクト3Dの生成処理 ---
//==================================================================================
CUtilityPole *CUtilityPole::Create(const Vector3 &pos, const Vector3 &vecQua, const float fAngle)
{
	CUtilityPole *pPole = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pPole = new CUtilityPole;
	if (pPole == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pPole->Init(pos, vecQua, fAngle);

	return pPole;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CUtilityPole::CUtilityPole() : CObjectXQuaternion(UTILITYPOLE_PRIORITY)
{ // メンバ変数をクリア
	ZeroMemory(m_apBillboard, sizeof(m_apBillboard));
	m_pConnect = nullptr;
	m_pConnected = nullptr;
	m_pConnectedPowerPlant = nullptr;
	m_nNumConnect = 0;
	m_bElectriced = false;
	m_enableType = ICON_CAN;

	// タイプの指定
	CObject::SetType(TYPE_POLE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CUtilityPole::~CUtilityPole()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CUtilityPole::Init(const Vector3 &pos, const Vector3 &vecQua, const float fAngle)
{
	HRESULT hr;		// 初期化結果
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャ管理オブジェクトへのポインタ
	Vector3 posIcon;

	// 親クラスの初期化
	hr = CObjectXQuaternion::Init(POLE_PATH, pos, vecQua, fAngle);
	posIcon = Vector3(0.0f, GetVtxMax()->y, 0.0f);

	// ビルボード生成
	m_apBillboard[ICON_CAN] = CObjectBillboard3D::Create(posIcon, VECTOR3_NULL, DEF_ICON_SIZE);
	m_apBillboard[ICON_CAN]->BindTexture(pTexture->Register(ICON_CAN_PATH));
	m_apBillboard[ICON_CAN]->SetDisp(false);
	m_apBillboard[ICON_CAN]->SetAlpha(true);

	m_apBillboard[ICON_CANT] = CObjectBillboard3D::Create(posIcon, VECTOR3_NULL, DEF_ICON_SIZE);
	m_apBillboard[ICON_CANT]->BindTexture(pTexture->Register(ICON_CANT_PATH));
	m_apBillboard[ICON_CANT]->SetDisp(false);
	m_apBillboard[ICON_CANT]->SetAlpha(true);

	// タイプを初期化
	m_enableType = ICON_CAN;

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CUtilityPole::Uninit(void)
{ // 親クラスの終了
	CObjectXQuaternion::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CUtilityPole::Update(void)
{
	CManager *pManager = CManager::GetInstance();	// マネージャーへのポインタ
	CGame *pGame = pManager->GetScene<CGame>();		// ゲームシーンへのポインタ
	CPlayer *pPlayer = pGame->GetPlayer();			// プレイヤーへのポインタ
	CCamera *pPlayerCam = CCamera::GetCamera(CCamera::TYPE_PLAYER);		// プレイヤーカメラへのポインタ
	Vector3 posPlayer = VECTOR3_NULL;			// プレイヤーの絶対座標
	Vector3 posPole = *GetPosition();			// 電柱の絶対座標
	auto pObj = pPlayer->GetRidingObject();		// プレイヤーの乗っているオブジェクトが保存されている変数
	int nIndexVariant = pObj->index();			// 現在入っている型のインデックス
	const CObjectXQuaternion *pRidingObject;	// プレイヤーの乗っているオブジェクト

	// ポインタを取得
	std::visit([&](auto &x) { pRidingObject = x; }, *pObj);

	// 電柱の上の絶対座標を求める
	posPole.y = GetVtxMax()->y;
	D3DXVec3TransformCoord(&posPole, &posPole, GetMatrix());

	if (pRidingObject != nullptr)
	{ // プレイヤーが電柱に乗っている場合
		for (auto &pBill : m_apBillboard)
		{ // ビルボードの位置を修正
			pBill->SetPosition(posPole);

			if (m_bSelected == true)
			{ // 選ばれている場合
				if (nIndexVariant == CPlayer::CPOWERPLANT_PTR)
				{ // プレイヤーの乗っているオブジェクトが発電所の場合
					auto pPowerPlant = std::get<CPowerPlant *>(*pObj);		// 発電所のポインタを取得
					auto vpPole = pPowerPlant->GetConnectPoles();			// 発電所とつながっている電柱のポインタ

					// 電柱のポインタから自身を検索
					auto iter = std::find(vpPole->cbegin(), vpPole->cend(), this);
					if (iter == vpPole->cend())
					{ // 既に繋げている電柱では無い場合、サイズアップ
						pBill->SetSize(SELECT_ICON_SIZE);
					}
				}
				else if (nIndexVariant == CPlayer::CUTILITYPOLE_PTR)
				{ // プレイヤーの乗っているオブジェクトが電柱なら
					auto pPole = std::get<CUtilityPole*>(*pObj);		// 電柱のポインタを取得
					if (m_pConnect == nullptr)
					{ // プレイヤーのカメラの中心に最も近い場合(かつ既につながっていない場合)、サイズアップ
						pBill->SetSize(SELECT_ICON_SIZE);
					}
				}
			}
			else
			{ // プレイヤーのカメラの中心に最も近くない場合、サイズダウン
				pBill->SetSize(DEF_ICON_SIZE);
			}
		}

		// プレイヤーの絶対座標を求める
		D3DXVec3TransformCoord(&posPlayer, pPlayer->GetPosition(), pPlayer->GetMatrix());

		// 絶対座標同士で距離を測る
		if (Vec3::Length(posPlayer, posPole) <= CONNECT_LENGTH)
		{ // プレイヤーと電柱の距離が一定以下の場合
			if (nIndexVariant == CPlayer::CPOWERPLANT_PTR)
			{ // プレイヤーの乗っているオブジェクトが発電所の場合
				auto pPowerPlant = std::get<CPowerPlant*>(*pObj);		// 発電所のポインタを取得
				auto vpPole = pPowerPlant->GetConnectPoles();			// 発電所とつながっている電柱のポインタ

				// 電柱のポインタから自身を検索
				auto iter = std::find(vpPole->cbegin(), vpPole->cend(), this);
				if (iter == vpPole->cend())
				{ // 既に繋げている電柱では無い場合
					m_enableType = ICON_CAN;		// 可能アイコンに設定
				}
				else
				{ // 上記の条件を一つでも満たしていなかった場合
					m_enableType = ICON_CANT;		// 不可能アイコンに設定
				}
			}
			else
			{ // プレイヤーの乗っているオブジェクトが電柱なら
				if (m_pConnect == nullptr && m_pConnected == nullptr)
				{ // 自身が何処にもつながっていない場合
					m_enableType = ICON_CAN;		// 可能アイコンに設定
				}
				else
				{ // 上記の条件を一つでも満たしていなかった場合
					m_enableType = ICON_CANT;		// 不可能アイコンに設定
				}
			}
		}
		else
		{ // 一定距離以上の場合
			m_enableType = ICON_CANT;		// 不可能アイコンに設定
		}
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CUtilityPole::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CGame *pGame = pManager->GetScene<CGame>();			// ゲームシーンへのポインタ
	CPlayer *pPlayer = pGame->GetPlayer();				// プレイヤーへのポインタ
	auto pObj = pPlayer->GetRidingObject();		// プレイヤーの乗っているオブジェクトが保存されている変数
	const CObjectXQuaternion *pRidingObject;	// プレイヤーの乗っているオブジェクト

	// ポインタを取得
	std::visit([&](auto &x) { pRidingObject = x; }, *pObj);

	// モデルの描画
	CObjectXQuaternion::Draw();

	if (pRidingObject != nullptr)
	{ // プレイヤーが電柱に乗っていれば
		// 現在のアイコンタイプのビルボードのみ表示
		m_apBillboard[m_enableType]->SetDisp(true);
		m_apBillboard[(m_enableType + 1) % ICON_MAX]->SetDisp(false);
	}
	else
	{ // 全ビルボードを非表示に設定
		m_apBillboard[ICON_CAN]->SetDisp(false);
		m_apBillboard[ICON_CANT]->SetDisp(false);
	}
}

//==================================================================================
// --- 他の電柱との接続処理 ---
//==================================================================================
bool CUtilityPole::Connect(CUtilityPole *pPole)
{
	int nIdxNull = -1;		// nullがあったインデックス

	// ポインタを確認
	if (pPole == nullptr		// ポインタがnullだった場合
		|| pPole == this		// ポインタが自分だった場合
		|| pPole->m_pConnectedPowerPlant != nullptr		// 接続先が発電所とつながっていた場合
		|| m_pConnect != nullptr	// 既に他の電柱と接続している場合
		|| m_pConnect == pPole)
	{ // 処理をスキップ
		return false;
	}

	// ポインタを保存
	m_pConnect = pPole;

	// 接続処理を呼び出し
	pPole->Connected(this);

	// 電柱同士を電線で接続
	auto pCable = CElectricalCable::Create(this, pPole);
	pCable->SetParent(GetParent());
	m_nNumConnect++;

	// 電流エフェクト生成
	CElectricCurrent *pCurrect = CElectricCurrent::Create(this, pPole);
	pCurrect->SetParent(CManager::GetInstance()->GetScene<CGame>()->GetPlanet()->GetMatrix());

	return true;
}

//==================================================================================
// --- 他の電柱からの接続処理 ---
//==================================================================================
bool CUtilityPole::Connected(CUtilityPole *pPole)
{ // 接続先が自身もしくはnullならスキップ
	if (pPole == nullptr || pPole == this) return false;

	// 電柱へのポインタを保存
	m_pConnected = pPole;
	return true;
}

//==================================================================================
// --- 発電所からの接続処理 ---
//==================================================================================
bool CUtilityPole::Connected(CPowerPlant *pPowerPlant)
{ // nullの場合スキップ
	if (pPowerPlant == nullptr) return false;

	// 発電所へのポインタを保存
	m_pConnectedPowerPlant = pPowerPlant;
	return true;
}

//==================================================================================
// --- 接続済みの電柱へ電流を送信する処理 ---
//==================================================================================
void CUtilityPole::GenerateElectricity(void)
{ // 既に流れている場合は流さない
	if (m_bElectriced == true) return;

	// 自身とつながっている電柱に電気を流す
	CElectricCurrent::Create(this, m_pConnect);

	// 電流が流れたためフラグを立てる
	m_bElectriced = true;
}

//==================================================================================
// --- 選択処理 ---
//==================================================================================
void CUtilityPole::SetEnableSelect(const bool bEnable)
{
	m_bSelected = bEnable;
}