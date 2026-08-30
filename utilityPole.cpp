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
#include "debugproc.h"
#include "texture.h"
#include "electricalCable.h"
#include "electricCurrent.h"
#include "objectBillboard3D.h"
#include "effect.h"
#include "player.h"
#include "planet.h"
#include "camera.h"
#include "thunderCamera.h"
#include "vec3math.h"
#include "color.h"
#include "map.h"
#include <algorithm>
#include <ranges>
#include <string_view>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define POLE_PATH			"data/MODEL/utilityPole.x"		// 電柱のパス
#define DEF_ICON_SIZE		Vector2(50.0f, 50.0f)			// アイコンサイズ
#define SELECT_ICON_SIZE	(DEF_ICON_SIZE * 2.0f)			// 最も近いアイコンのサイズ
#define FOCUS_ANGLE			(D3DXToRadian(45.0f))			// フォーカス可能な角度
#define CONNECT_HEIGHT		(150.0f)	// 繋げられる電柱との高さの差分の最大値
#define CONNECT_HEIGHT_EX	(30.0f)		// 発電所の場合の追加差分
#define AIMING_ROTATE_SPD	(0.02f)		// エイムアイコンの回転速度

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	std::string_view c_asIconPath[CUtilityPole::ICON_MAX] =
	{ // 各アイコンのテクスチャパス
		"data/TEXTURE/icon/can.png",		// 可能アイコン
		"data/TEXTURE/icon/cant.png",		// 不可能アイコン
		"data/TEXTURE/icon/aiming.png",		// ターゲットアイコン
	};
}

//==================================================================================
// --- オブジェクト3Dの生成処理 ---
//==================================================================================
CUtilityPole *CUtilityPole::Create(const Vector3 &pos,
	const Vector3 &vecQua,
	const float fAngle,
	const int nID)
{
	CUtilityPole *pPole = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pPole = new CUtilityPole;
	if (pPole == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pPole->Init(pos, vecQua, fAngle, nID);

	return pPole;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CUtilityPole::CUtilityPole() : CObjectXQuaternion(UTILITYPOLE_PRIORITY)
{ // メンバ変数をクリア
	ZeroMemory(m_apBillboard, sizeof(m_apBillboard));
	m_pConnect = nullptr;
	m_pCurrentCable = nullptr;
	m_nNumConnect = 0;
	m_bElectriced = false;
	m_enableType = ICON_CAN;
	m_nID = -1;

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
HRESULT CUtilityPole::Init(const Vector3 &pos,
	const Vector3 &vecQua,
	const float fAngle,
	const int nID)
{
	HRESULT hr;		// 初期化結果
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャ管理オブジェクトへのポインタ
	Vector3 posIcon;

	// 親クラスの初期化
	hr = CObjectXQuaternion::Init(POLE_PATH, pos, vecQua, fAngle);
	posIcon = Vector3(0.0f, GetVtxMax()->y, 0.0f);

	// ビルボード生成
	for (int nCntIcon = 0; nCntIcon < ICON_MAX; nCntIcon++)
	{
		m_apBillboard[nCntIcon] = CObjectBillboard3D::Create(posIcon, VECTOR3_NULL, DEF_ICON_SIZE);
		m_apBillboard[nCntIcon]->BindTexture(pTexture->Register(c_asIconPath[nCntIcon]));
		m_apBillboard[nCntIcon]->SetDisp(false);
		m_apBillboard[nCntIcon]->SetAlpha(true);
	}

	// タイプを初期化
	m_enableType = ICON_CAN;

	// 引数を保存
	m_nID = nID;

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
	if (pGame == nullptr) return;					// ゲームシーンの取得に失敗した場合、以下の処理を無視

	CPlayer *pPlayer = pGame->GetPlayer();			// プレイヤーへのポインタ
	CCamera *pPlayerCam = CCamera::GetCamera(CCamera::TYPE_PLAYER);		// プレイヤーカメラへのポインタ
	Vector3 posPlayer = VECTOR3_NULL;			// プレイヤーの絶対座標
	Vector3 posPole = *GetPosition();			// 電柱の絶対座標
	auto pObj = pPlayer->GetRidingObject();		// プレイヤーの乗っているオブジェクトが保存されている変数
	int nIndexVariant = pObj->index();			// 現在入っている型のインデックス
	CObjectXQuaternion *pConnected = nullptr;	// 繋げてきたオブジェクト
	const CObjectXQuaternion *pRidingObject = nullptr;	// プレイヤーの乗っているオブジェクト

	// ポインタを取得
	std::visit([&](auto &x) { pConnected = x; }, m_pConnected);
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
				pBill->SetSize(SELECT_ICON_SIZE);
			}
			else
			{ // 選ばれていない場合、サイズダウン
				pBill->SetSize(DEF_ICON_SIZE);
			}
		}

		// プレイヤーの絶対座標を求める
		D3DXVec3TransformCoord(&posPlayer, &posPlayer, pPlayer->GetMatrix());

		// 絶対座標同士で距離を測る
		if (posPlayer.y - posPole.y <= CONNECT_HEIGHT + (CONNECT_HEIGHT_EX * (1 - nIndexVariant)))
		{ // プレイヤーと電柱の距離が一定以下の場合
			if (std::holds_alternative<CPowerPlant *>(*pObj))
			{ // プレイヤーの乗っているオブジェクトが発電所の場合
				auto pPowerPlant = std::get<CPowerPlant *>(*pObj);		// 発電所のポインタを取得
				auto vpPole = pPowerPlant->GetConnectPole();			// 発電所とつながっている電柱のポインタ

				// 電柱のポインタから自身を検索
				if (std::ranges::find(vpPole, this) == vpPole.end()
					&& pConnected == nullptr
					&& m_pConnect == nullptr)
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
				if (m_pConnect == nullptr
					&& pConnected == nullptr)
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
	else
	{ // プレイヤーが電柱に載っていない場合
		m_enableType = ICON_CANT;
	}

	// 回転
	float fRotate = m_apBillboard[ICON_AIMING]->GetRotation()->y;		// 現在のポリゴンの角度
	float fScale = 1.25f + (sinf(fRotate) * 0.5f);		// 拡縮倍率

	// 回転
	m_apBillboard[ICON_AIMING]->SetRotation(Vector3(0.0f, fRotate + AIMING_ROTATE_SPD, 0.0f));

	// サイズ拡縮
	m_apBillboard[ICON_AIMING]->SetSize(SELECT_ICON_SIZE * fScale);
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CUtilityPole::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// モデルの描画
	CObjectXQuaternion::Draw();

	CGame *pGame = pManager->GetScene<CGame>();			// ゲームシーンへのポインタ
	if (pGame == nullptr) return;						// ゲームシーンの取得に失敗した場合、以下の処理を無視

	CPlayer *pPlayer = pGame->GetPlayer();				// プレイヤーへのポインタ
	auto pObj = pPlayer->GetRidingObject();		// プレイヤーの乗っているオブジェクトが保存されている変数
	const CObjectXQuaternion *pRidingObject;	// プレイヤーの乗っているオブジェクト

	// ポインタを取得
	std::visit([&](auto &x) { pRidingObject = x; }, *pObj);

	for (auto &pBill : m_apBillboard)
	{ // 全ビルボードを非表示に設定
		pBill->SetDisp(false);
	}

	if (pRidingObject != nullptr)
	{ // プレイヤーが電柱に乗っていれば
		// 現在のアイコンタイプのビルボードのみ表示
		m_apBillboard[m_enableType]->SetDisp(true);

		if (m_bSelected == true)
		{ // 選択されていれば、エイムビルボードを表示
			m_apBillboard[ICON_AIMING]->SetDisp(true);
		}
	}
}

//==================================================================================
// --- 他の電柱との接続処理 ---
//==================================================================================
bool CUtilityPole::Connect(CUtilityPole *pPole)
{ // ポインタを確認
	if (pPole == nullptr) return false;					// ポインタがnullだった場合、失敗
	else if (pPole == this) return false;				// ポインタが自分だった場合、失敗
	else if (m_pConnect != nullptr) return false;		// 既に他の電柱と接続している場合、失敗
	else if (m_pConnect == pPole) return false;			// 既にその電柱と接続している場合、失敗
	
	// 発電所へのポインタを取得
	CPowerPlant **ppPlant = std::get_if<CPowerPlant*>(&pPole->m_pConnected);
	if (ppPlant)
	{ // 発電所へのポインタだった場合
		if (*ppPlant != nullptr) return false;			// 発電所へのポインタが代入されていれば、失敗
	}

	// ポインタを保存
	m_pConnect = pPole;

	// 接続処理を呼び出し
	pPole->Connected(this);

	// 電柱同士を電線で接続
	m_pCurrentCable = CElectricalCable::Create(this, 
		pPole,
		CMap::GetInstance()->GetCurrentScenePlanet());
	m_pCurrentCable->SetParent(GetParent());
	m_nNumConnect++;

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
	m_pConnected = pPowerPlant;
	return true;
}

//==================================================================================
// --- 接続済みの電柱へ電流を送信する処理 ---
//==================================================================================
void CUtilityPole::GenerateElectricity(void)
{ // 既に流れている場合は流さない
	CGame *pGame = CManager::GetInstance()->GetScene(&pGame);	// ゲームシーンへのポインタ

	if (m_bElectriced == true || m_pConnect == nullptr)
	{ // カメラへnullを登録
		if(pGame != nullptr) pGame->GetThunderCamera()->ChangeTarget(nullptr);
		return;
	}

	// 自身とつながっている電柱に電気を流す + カメラに登録
	CElectricCurrent *pCurrent = CElectricCurrent::Create(this, m_pConnect);
	pCurrent->SetParent(CMap::GetInstance()->GetCurrentScenePlanet()->GetMatrix());
	
	if (pGame != nullptr)
	{ // カメラに電柱を登録
		pGame->GetThunderCamera()->ChangeTarget(pCurrent);
	}

	// 電線の色を変更
	m_pCurrentCable->SetColor(Colors::GetColor(Colors::C_YELLOW));

	// 電流が流れたためフラグを立てる
	m_bElectriced = true;
}

//==================================================================================
// --- 接続の取り消し処理 ---
//==================================================================================
void CUtilityPole::RemoveConnected(void)
{ // nullptrか確認
	if (std::visit([](auto &x) { return (x != nullptr); }, m_pConnected))
	{ // 自分に繋げてきた電柱にも通知
		std::visit([](auto &x) { x->RemoveConnected(); }, m_pConnected);
	}

	if (m_pCurrentCable != nullptr)
	{ // 繋げた電線があれば破棄
		m_pCurrentCable->Uninit();
		m_pCurrentCable = nullptr;
	}

	// 接続先ポインタを手放す
	m_pConnect = nullptr;

	// ポインタを手放す
	m_pConnected = static_cast<CPowerPlant*>(nullptr);
}

//==================================================================================
// --- プレイヤーがフォーカス可能か判定する処理 ---
//==================================================================================
bool CUtilityPole::CanFocus(const CPlayer *pPlayer)
{ // 計算無しで判定できるものを先に判定
	if (pPlayer == nullptr) return false;				// プレイヤーがnullの場合、false
	else if (m_enableType != ICON_CAN) return false;	// タイプが接続不可の場合、false

	CCamera *pPlayerCam = CCamera::GetCamera(CCamera::TYPE_PLAYER);		// プレイヤーのカメラ
	Vector3 posWorld = VECTOR3_NULL;			// 自身の絶対座標
	Vector3 posPlayerWorld = VECTOR3_NULL;		// プレイヤーの絶対座標
	Vector3 rayCam = VECTOR3_NULL;				// カメラの視線ベクトル
	Vector3 rayPlayerToPole = VECTOR3_NULL;		// プレイヤーと電柱を繋いだベクトル
	float fDot = 0.0f;			// 視線ベクトルとプレイヤーと電柱を繋いだベクトルの内積結果
	float fRadian = 0.0f;		// 二つのベクトル間の角度

	// 自身の絶対座標を取得
	posWorld.y = GetVtxMax()->y;
	D3DXVec3TransformCoord(&posWorld, &posWorld, GetMatrix());

	// プレイヤーの絶対座標を取得
	D3DXVec3TransformCoord(&posPlayerWorld, &posPlayerWorld, pPlayer->GetMatrix());

	// カメラのレイを取得 + Y軸のベクトルを0に戻す
	rayCam = Vec3::Direction(posPlayerWorld, *pPlayerCam->GetPosV());
	rayCam.y = 0.0f;

	// ベクトルの長さが0だった場合、失敗
	if (Vec3::Length(rayCam) == 0.0f) return false;

	// プレイヤーと電柱を繋いだベクトルを求める
	rayPlayerToPole = Vec3::Direction(posWorld, posPlayerWorld);
	rayPlayerToPole.y = 0.0f;

	// ベクトルの長さが0だった場合、失敗
	if (Vec3::Length(rayPlayerToPole) == 0.0f) return false;

	// 二つのベクトルから内積を求める
	fDot = Vec3::Dot(rayCam, rayPlayerToPole);

	// -1.0f～1.0fの間にクランプ
	fDot = std::clamp(fDot, -1.0f, 1.0f);

	// 角度を求める
	fRadian = acosf(fDot);

	// 角度が+-15°の範囲にいるならtrue
	return (fRadian < FOCUS_ANGLE);
}

//==================================================================================
// --- 選択処理 ---
//==================================================================================
void CUtilityPole::SetEnableSelect(const bool bEnable)
{
	m_bSelected = bEnable;
}