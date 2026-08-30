//==================================================================================
// 
// 感電エフェクトクラスのソースファイル [shock.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/30
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "shock.h"
#include "object2D.h"
#include "objectBillboard3D.h"
#include "sparkEffect.h"
#include "manager.h"
#include "texture.h"
#include "matrix.h"
#include "vec3math.h"
#include <string_view>
#include <span>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define LIGHTNING_TIME		(CManager::SecToFrame(1.5f))	// 感電の持続時間
#define PLAYER_EFFECT_SIZE	(Vector2(150.0f, 150.0f))		// プレイヤーのエフェクトのサイズ
#define SCREEN_EFFECT_SIZE	(SCREEN_SIZE)					// スクリーンエフェクトのサイズ
#define WARNING_EFFECT_SIZE	(Vector2(1000.0f, 256.0f))		// 警告エフェクトのサイズ
#define WARNING_PATH		"data/TEXTURE/effect/shock.png"	// 警告用テクスチャのパス
#define WARNING_BLINK_NUM	(3)			// 警告の点滅する回数

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	std::string_view c_asPlayerEffectPath[] =
	{ // プレイヤーに出るエフェクト用テクスチャパス
		"data/TEXTURE/effect/shock000.png",
		"data/TEXTURE/effect/shock001.png",
		"data/TEXTURE/effect/shock002.png",
		"data/TEXTURE/effect/shock003.png"
	};

	std::string_view c_asScreenEffectPath[] =
	{ // スクリーンに出るエフェクト用テクスチャパス
		"data/TEXTURE/effect/screenLightning000.png",
		"data/TEXTURE/effect/screenLightning001.png",
	};
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CShock *CShock::Create(const Matrix *pMtxParent, const Vector3 &offset)
{
	CShock *pShock = new CShock;		// 感電エフェクトへのポインタ
	if (pShock != nullptr)
	{ // 生成出来ていれば、初期化
		pShock->Init(pMtxParent, offset);
	}
	
	return pShock;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CShock::CShock()
{ // メンバ変数をクリア
	m_pWarningEffect = nullptr;
	m_pScreenEffect = nullptr;
	m_pPlayerEffect = nullptr;
	m_pMtxParent = nullptr;
	m_offset = VECTOR3_NULL;
	m_nTime = 0;
	m_nCounterFrame = 0;

	// タイプ指定
	SetType(TYPE_SHOCK);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CShock::~CShock()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CShock::Init(const Matrix *pMtxParent, const Vector3 &offset)
{ // 引数を保存
	m_pMtxParent = pMtxParent;
	m_offset = offset;

	CTexture *pTexture = CTexture::GetInstance();		// テクスチャインスタンスへのポインタ

	// 警告用ポリゴンを生成
	m_pWarningEffect = CObject2D::Create(SCREEN_MIDDLE, WARNING_EFFECT_SIZE);
	m_pWarningEffect->BindTexture(pTexture->Register(WARNING_PATH));
	m_pWarningEffect->SetDisp(false);

	// スクリーン用ポリゴンを生成
	m_pScreenEffect = CObject2D::Create(SCREEN_MIDDLE, SCREEN_EFFECT_SIZE);
	m_pScreenEffect->BindTexture(pTexture->Register(c_asScreenEffectPath[0]));
	m_pScreenEffect->SetDisp(false);

	// プレイヤー用ビルボードを生成
	m_pPlayerEffect = CObjectBillboard3D::Create(offset,
		VECTOR3_NULL, 
		PLAYER_EFFECT_SIZE,
		INT_MAX,
		2);
	m_pPlayerEffect->BindTexture(pTexture->Register(c_asPlayerEffectPath[0]));
	m_pPlayerEffect->SetParent(m_pMtxParent);
	m_pPlayerEffect->SetDisp(false);
	m_pPlayerEffect->SetStateFunctionBeforeDraw([](LPDIRECT3DDEVICE9 pDevice)
		{ // Zバッファへの書き込み無し
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		});
	m_pPlayerEffect->SetStateFunctionAfterDraw([](LPDIRECT3DDEVICE9 pDevice)
		{ // Zバッファへの書き込み無し
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		});

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CShock::Uninit(void)
{ // 各オブジェクトの破棄
	if (m_pWarningEffect != nullptr)
	{ // 警告用ポリゴンを終了
		m_pWarningEffect->Uninit();
		m_pWarningEffect = nullptr;
	}

	if (m_pScreenEffect != nullptr)
	{ // スクリーン用ポリゴンを終了
		m_pScreenEffect->Uninit();
		m_pScreenEffect = nullptr;
	}

	if (m_pPlayerEffect != nullptr)
	{ // プレイヤー用ビルボードを終了
		m_pPlayerEffect->Uninit();
		m_pPlayerEffect = nullptr;
	}

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CShock::Update(void)
{ // 感電していない場合スキップ
	if (m_nCounterFrame > m_nTime) return;

	// テクスチャのパス
	std::span asPlayerEffectPath = c_asPlayerEffectPath;
	std::span asScreenEffectPath = c_asScreenEffectPath;
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャインスタンスへのポインタ

	constexpr int nPlayerEffectChangeLine = LIGHTNING_TIME / asPlayerEffectPath.size();
	constexpr int nScreenEffectChangeLine = LIGHTNING_TIME / (asScreenEffectPath.size() * 2);
	constexpr float fRadianIncrease = (DOUBLE_PI * WARNING_BLINK_NUM) / LIGHTNING_TIME;
	int nPlayerCurrentIdx = m_nCounterFrame / nPlayerEffectChangeLine;		// テクスチャパスのインデックス
	int nScreenCurrentIdx = m_nCounterFrame / nScreenEffectChangeLine;		// テクスチャパスのインデックス
	float fAlpha = (cosf(fRadianIncrease * static_cast<float>(m_nCounterFrame)) * 0.5f) + 0.5f;		// α値
	Color col = m_pWarningEffect->GetColor();		// 今の色

	// α値を変更
	col.a = fAlpha;
	
	// 各ポリゴンを変更
	m_pWarningEffect->SetColor(col);
	m_pPlayerEffect->BindTexture(pTexture->Register(asPlayerEffectPath[nPlayerCurrentIdx % asPlayerEffectPath.size()]));
	m_pScreenEffect->BindTexture(pTexture->Register(asScreenEffectPath[nScreenCurrentIdx % asScreenEffectPath.size()]));

	m_nCounterFrame++;		// フレームカウント増加
	if (m_nCounterFrame >= m_nTime)
	{ // 各ポリゴンを描画しないようにする
		m_pWarningEffect->SetDisp(false);
		m_pScreenEffect->SetDisp(false);
		m_pPlayerEffect->SetDisp(false);
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CShock::Draw(void)
{
}

//==================================================================================
// --- 感電設定処理 ---
//==================================================================================
void CShock::Set(void)
{ // 各変数を再設定
	m_nTime = LIGHTNING_TIME;		// 持続時間を設定
	m_nCounterFrame = 0;			// フレームカウントリセット

	// 各ポリゴンを描画するようにする
	m_pWarningEffect->SetDisp(true);
	m_pScreenEffect->SetDisp(true);
	m_pPlayerEffect->SetDisp(true);
}