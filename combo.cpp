//==================================================================================
// 
// コンボ表示クラスのソースファイル [combo.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/20
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "combo.h"
#include "number.h"
#include "polygon2D.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "player.h"
#include "powerPlant.h"
#include "utilityPole.h"
#include "util.h"
#include "texture.h"
#include "observer_pointer.h"
#include <algorithm>
#include <array>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define COMBO_TIME		(20)				// コンボ表示が持続する時間
#define COMBO_FRAME		(CManager::SecToFrame(COMBO_TIME))		// コンボ表示が持続するフレーム数
#define COMBO_SIZE		(Vector2(200.0f, 80.0f))		// コンボ表示のサイズ
#define NUMBER_SIZE		(Vector2(64.0f, 128.0f))		// 数値オブジェクトのサイズ
#define NUMBER_TEXTYPE	(CNumber::TYPE_VOLTNUM_003)		// タイマーの数字のテクスチャの種類
#define GAUGE_SIZE		Vector2(COMBO_SIZE.x + (NUMBER_SIZE.x * MAX_COMBO_NUM), 15.0f)		// ゲージのサイズ
#define COMBO_TEX_PATH	"data/TEXTURE/Combo.png"		// コンボテクスチャ
#define SIZE_MAGNI		(1.65f)			// コンボ増加時の拡大倍率
#define GAUGE_TIME		(static_cast<float>(COMBO_TIME))		// コンボ表示が完全に消えるまでの時間
#define GAUGE_VALUE		(CManager::SecToRatio(GAUGE_TIME))		// 1フレーム当たりのゲージの線形補間の増加係数

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CCombo *CCombo::Create(const Vector3 &pos, const Vector3 &rot)
{
	CCombo *pTimer = new CCombo;		// 生成したタイマーへのポインタ
	if (pTimer != nullptr)
	{ // タイマーを初期化
		pTimer->Init(pos, rot);
	}

	return pTimer;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CCombo::CCombo() : CObject(DEFAULT_UI_PRIORITY)
{ // タイプを指定
	SetType(TYPE_TIMER);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CCombo::~CCombo()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CCombo::Init(const Vector3 &pos, const Vector3 &rot)
{ // 引数を保存
	m_pos = pos;
	m_rot = rot;
	m_fTimeCatmullRom = 1.0f;
	m_nDispLife = COMBO_FRAME;

	float fWidthAll = COMBO_SIZE.x + (NUMBER_SIZE.x * MAX_COMBO_NUM);		// 総合幅

	// 数値オブジェクトを二桁分作成
	for (int nCntNumber = 0; nCntNumber < MAX_COMBO_NUM; nCntNumber++)
	{
		Vector3 posNumber;		// 数値オブジェクトの中心座標

		// 中心座標を計算
		posNumber.x = pos.x - (fWidthAll * 0.5f) + (NUMBER_SIZE.x * nCntNumber);
		posNumber.y = pos.y;
		posNumber.z = 0.0f;

		// 生成したオブジェクトを保存 (描画はしない)
		m_apNumber.at(nCntNumber).reset(CNumber::Create(NUMBER_TEXTYPE, posNumber, NUMBER_SIZE, 0));
		m_apNumber.at(nCntNumber)->SetDisp(false);
	}

	Vector3 posCombo;		// コンボ表示用ポリゴンの座標

	// ポリゴンの位置を計算
	posCombo.x = pos.x + (fWidthAll * 0.5f) - (COMBO_SIZE.x * 0.6f);
	posCombo.y = pos.y + (COMBO_SIZE.y * 0.4f);
	posCombo.z = 0.0f;

	// コンボ表示用ポリゴンを作成 (描画はしない)
	m_pCombo.reset(CPolygon2D::Create(posCombo, VECTOR3_NULL, COMBO_SIZE));
	m_pCombo->BindTexture(CTexture::GetInstance()->Register(COMBO_TEX_PATH));
	m_pCombo->SetDisp(false);

	Vector3 posGauge;		// ゲージ用ポリゴンの座標

	// ポリゴンの位置を計算
	posGauge.x = pos.x;
	posGauge.y = pos.y + (NUMBER_SIZE.y * 0.6f);
	posGauge.z = 0.0f;

	// ゲージ用ポリゴンを作成 (描画はしない)
	m_pGauge.reset(CPolygon2D::Create(posGauge, VECTOR3_NULL, GAUGE_SIZE));
	m_pGauge->SetDisp(false);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CCombo::Uninit(void)
{
	for (auto &pNumber : m_apNumber)
	{ // コンボが生成した数値オブジェクトを破棄
		if (pNumber != nullptr)
		{ // NULLではなかった場合、破棄 + 終了処理
			pNumber->Uninit();
			pNumber.reset();
		}
	}

	// ポリゴンを破棄
	if (m_pCombo != nullptr)
	{ // NULLではなかった場合、破棄 + 終了処理
		m_pCombo->Uninit();
		m_pCombo.reset();
	}

	// ポリゴンを破棄
	if (m_pGauge != nullptr)
	{ // NULLではなかった場合、破棄 + 終了処理
		m_pGauge->Uninit();
		m_pGauge.reset();
	}

	// 親クラスの終了処理
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CCombo::Update(void)
{
	if (m_bDisp == true)
	{ // 描画している場合のみ
		m_nDispLife--;
		if (m_nDispLife < 0)
		{ // 描画時間が過ぎた場合、描画しないように設定し体力を元に戻す
			m_bDisp = false;
			m_nDispLife = COMBO_FRAME;
		}
	}
	
	// サイズ更新
	UpdateScale();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CCombo::Draw(void)
{ // 描画フラグが立っていない場合スキップ
	if (m_bDisp != true) return;

	own::ObserverPtr pManager(CManager::GetInstance());		// マネージャへのポインタ
	own::ObserverPtr pRenderer(pManager->GetRenderer());	// レンダラーへのポインタ
	own::ObserverPtr pDevice(pRenderer->GetDevice());		// デバイスへのポインタ

	// αテストを有効化
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 30);

	for (auto &pNumber : m_apNumber)
	{ // タイマーが生成した数値オブジェクトを描画
		if (pNumber != nullptr)
		{ // NULLではなかった場合、描画処理
			pNumber->Draw();
		}
	}

	if (m_pCombo != nullptr)
	{ // NULLではなかった場合、描画処理
		m_pCombo->Draw();
	}
	
	if (m_pGauge != nullptr)
	{ // NULLではなかった場合、描画処理
		m_pGauge->Draw();
	}

	// αテストを無効化
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==================================================================================
// --- コンボ設定処理 ---
//==================================================================================
void CCombo::SetCombo(const int nValue)
{
	int nNumValue = Util::GetNumInt(nValue);		// 引数の桁数
	std::array<int, MAX_COMBO_NUM> aNumValue;		// 各桁の値

	m_nCombo = nValue;		// コンボ数を保存

	for (int nCntNumber = 0; nCntNumber < MAX_COMBO_NUM; nCntNumber++)
	{ // 各桁の数値を求める
		aNumValue.at(nCntNumber) = nValue % (int)powf(10.0f, (float)(MAX_COMBO_NUM - nCntNumber)) / (int)powf(10.0f, (float)(MAX_COMBO_NUM - nCntNumber) - 1.0f);
	}

	if (nNumValue < MAX_COMBO_NUM)
	{ // 1桁の場合
		for (int nCntNumber = 0; nCntNumber < MAX_COMBO_NUM; nCntNumber++)
		{ // 表示して、数値を代入 + 透明度リセット
			m_apNumber.at(nCntNumber)->SetNumber(aNumValue.at(nCntNumber));
			m_apNumber.at(nCntNumber)->SetAlpha(1.0f);
		}

		// 1桁目のみ表示
		m_apNumber.at(0U)->SetDisp(false);
		m_apNumber.at(1U)->SetDisp(true);
		m_pCombo->SetDisp(true);
		m_pCombo->SetAlpha(1.0f);
		m_pGauge->SetDisp(true);
	}
	else
	{ // 2桁の場合
		for (int nCntNumber = 0; nCntNumber < MAX_COMBO_NUM; nCntNumber++)
		{ // 表示して、数値を代入 + 透明度リセット
			m_apNumber.at(nCntNumber)->SetNumber(aNumValue.at(nCntNumber));
			m_apNumber.at(nCntNumber)->SetDisp(true);
			m_apNumber.at(nCntNumber)->SetAlpha(1.0f);
		}

		// コンボとゲージを表示
		m_pCombo->SetDisp(true);
		m_pCombo->SetAlpha(1.0f);
		m_pGauge->SetDisp(true);
	}

	// 描画時間と各補間用変数を再設定
	m_fTimeCatmullRom = 0.0f;
	m_fTimeAlphaLerp = 0.0f;
	m_bDisp = true;
}

//==================================================================================
// --- コンボ追加処理 ---
//==================================================================================
void CCombo::AddCombo(const int nValue)
{ // 現コンボ数に加算した値で再設定
	SetCombo(m_nCombo + nValue);
}

//==================================================================================
// --- コンボ確定処理 ---
//==================================================================================
void CCombo::Finish(void)
{ // 現在までのコンボで確定させ、電気を流した後コンボをリセットする
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CGame *pGame = pManager->GetScene(&pGame);			// ゲームシーンへのポインタ
	CPlayer *pPlayer = pGame->GetPlayer();				// プレイヤーへのポインタ
	CPowerPlant *pPlant = pPlayer->GetStartPlant();		// プレイヤーの乗っていた発電所へのポインタ
	
	// コンボが0以下の場合スキップ
	if (m_nCombo <= 0) return;

	// 電流を流し始める
	pPlant->InvokeElectric();

	// コンボをリセット
	ResetCombo();
}

//==================================================================================
// --- コンボ取り消し処理 ---
//==================================================================================
void CCombo::Withdrawal(void)
{ // 現在までのコンボを取り消し、電気を流さずにコンボをリセットする
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CGame *pGame = pManager->GetScene(&pGame);			// ゲームシーンへのポインタ
	CPlayer *pPlayer = pGame->GetPlayer();				// プレイヤーへのポインタ
	auto pCurrentObject = pPlayer->GetRidingObject();	// プレイヤーの乗っているオブジェクト

	// 発電所に乗っている場合、スキップ
	if (pCurrentObject->index() == CPlayer::CPOWERPLANT_PTR) return;

	// 電柱へのポインタへキャスト
	auto pPole = std::get_if<CUtilityPole*>(pCurrentObject);
	if (pPole)
	{ // 取得成功時、電線の破棄処理を呼び出し
		(*pPole)->RemoveConnected();
	}

	// コンボをリセット
	ResetCombo();
}

//==================================================================================
// --- コンボリセット処理 ---
//==================================================================================
void CCombo::ResetCombo(void)
{ // 非表示化
	for (auto &pNumber : m_apNumber)
	{ // 非表示化
		pNumber->SetDisp(false);
		pNumber->SetAlpha(1.0f);
	}

	m_pCombo->SetDisp(false);
	m_pCombo->SetAlpha(1.0f);
	m_pGauge->SetDisp(false);

	m_nDispLife = COMBO_FRAME;	// 体力を元に戻す
	m_fTimeGaugeLerp = 0.0f;	// 補間用変数をリセット
	m_nCombo = 0;			// コンボ数リセット
	m_bDisp = false;		// 描画フラグをおろす
}

//==================================================================================
// --- 透明度更新処理 ---
//==================================================================================
void CCombo::UpdateAlpha(void)
{
	
}

//==================================================================================
// --- サイズ更新処理 ---
//==================================================================================
void CCombo::UpdateScale(void)
{
	if (m_fTimeCatmullRom < 1.0f && m_bDisp == true)
	{ // 補間用変数が1.0f未満の場合
		m_fTimeCatmullRom += 0.05f;		// 時間を加算
		Vector2 value;		// 計算後のサイズ
		Vector2 defSize = NUMBER_SIZE;				// 基本のサイズ
		Vector2 magSize = defSize * SIZE_MAGNI;		// 拡大時の最大サイズ

		if (m_fTimeCatmullRom <= 0.5f)
		{ // サイズ拡大処理
			D3DXVec2CatmullRom(&value, &magSize, &defSize, &magSize, &defSize, m_fTimeCatmullRom);
		}
		else
		{ // サイズ縮小処理
			D3DXVec2CatmullRom(&value, &defSize, &magSize, &defSize, &magSize, m_fTimeCatmullRom);
		}

		for (auto &pNumber : m_apNumber)
		{ // サイズ変更
			pNumber->SetSize(value);
		}

		if (m_fTimeCatmullRom >= 1.0f)
		{ // 補間用変数が1.0fを超えた場合、1.0fに修正
			m_fTimeCatmullRom = 1.0f;
		}
	}

	if (m_fTimeGaugeLerp < 1.0f && m_bDisp == true)
	{ // 補間用変数が1.0f未満の場合
		m_fTimeGaugeLerp += GAUGE_VALUE;	// 時間を加算

		// 補間後の値を計算
		float fWidth = std::lerp(GAUGE_SIZE.x, 0.0f, m_fTimeGaugeLerp);
		Vector2 size = Vector2(fWidth, GAUGE_SIZE.y);			// 計算後のサイズ

		// サイズを適用
		m_pGauge->SetSize(size);

		if (m_fTimeGaugeLerp >= 1.0f)
		{ // 補間用変数が1.0fを超えた場合、1.0fに修正
			m_fTimeGaugeLerp = 1.0f;

			CGame *pGame = CManager::GetInstance()->GetScene(&pGame);		// ゲームシーンへのポインタ
			CPlayer *pPlayer = (pGame) ? pGame->GetPlayer() : nullptr;		// プレイヤーへのポインタ
			// 透明に変更
			for (auto &pNumber : m_apNumber)
			{ // 非表示化
				pNumber->SetDisp(false);
				pNumber->SetAlpha(1.0f);
			}

			// 非表示化
			m_pCombo->SetDisp(false);
			m_pCombo->SetAlpha(1.0f);
			m_pGauge->SetDisp(false);

			if (pPlayer)
			{ // プレイヤーがnullではなく、投げ縄を既に投げていなければ
				if (pPlayer->IsShotLasso() != true) m_nCombo = 0;			// コンボ数リセット
			}
			else
			{ // プレイヤーがnullの場合
				m_nCombo = 0;			// コンボ数リセット
			}
			m_bDisp = false;			// 描画フラグをおろす
		}
	}
}