//==================================================================================
// 
// ランキング描画クラスのソースファイル [rankingDrawer.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/10
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "rankingDrawer.h"
#include "polygon2D.h"
#include "percent.h"
#include "number.h"
#include "texture.h"
#include "rankingManager.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DOT_PATH			"data/TEXTURE/dot.png"			// 小数点テクスチャのパス
#define PERCENT_PATH		"data/TEXTURE/percent.png"		// パーセントテクスチャのパス

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asRankPath[MAX_RANKING_NUM] =		// 各順位ポリゴンのテクスチャパス
	{
		"",
		"",
		"",
		"",
		"",
	};

	const CPercent::Setting c_defSetting =							// パーセント表示の基本設定
	{
		Vector3(675.0f, 250.0f, 0.0f),		// 開始位置
		Vector2(64.0f, 128.0f),				// 数字一桁のサイズ
		Vector2(64.0f, 128.0f),				// 小数点のサイズ
		Vector2(72.0f, 146.0f),				// パーセントのサイズ
		0.0f,								// パーセントの初期値
		CNumber::TYPE_VOLTNUM_003,			// 数値テクスチャの種類 
		INVALID_TEX_ID,						// テクスチャインデックス (静的に読み込めないため無効値)
		INVALID_TEX_ID,						// テクスチャインデックス (静的に読み込めないため無効値)
	};
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CRankingDrawer *CRankingDrawer::Create(const Vector3 &pos,
	const Vector2 &scale,
	const int nRank,
	const float fScore,
	const bool bBlink)
{
	CRankingDrawer *pDrawer = new CRankingDrawer;		// 生成したオブジェクトへのポインタ
	if (pDrawer != nullptr)
	{ // 生成出来ていれば初期化
		pDrawer->Init(pos, scale, nRank, fScore, bBlink);
	}

	return pDrawer;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CRankingDrawer::CRankingDrawer()
{ // メンバ変数のクリア
	m_nCountFrame = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CRankingDrawer::~CRankingDrawer()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CRankingDrawer::Init(const Vector3 &pos,
	const Vector2 &scale,
	const int nRank,
	const float fScore,
	const bool bBlink)
{ // 順位ポリゴン作成
	CTexture *pTexture = CTexture::GetInstance();

	m_pRank.reset(CPolygon2D::Create(pos, VECTOR3_NULL, scale));
	m_pRank->BindTexture(pTexture->Register(c_asRankPath[nRank]));

	// パーセント表示の設定
	auto setting = c_defSetting;		// 基本設定をコピー
	setting.start = Vector3(pos.x + (scale.x * 0.5f), pos.y, 0.0f);		// 開始座標を変更
	setting.fValue = fScore;			// 値を変更

	// テクスチャインデックスを変更
	setting.nIdxDotTexture = pTexture->Register(DOT_PATH);
	setting.nIdxPercentTexture = pTexture->Register(PERCENT_PATH);

	// スコア作成
	m_pScore = CPercent::Create(setting);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CRankingDrawer::Uninit(void)
{
	if (m_pRank != nullptr)
	{ // 生成されていれば破棄
		m_pRank->Uninit();
		m_pRank.reset();
	}

	if (m_pScore != nullptr)
	{ // 生成されていれば破棄
		m_pScore->Uninit();
		m_pScore = nullptr;
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CRankingDrawer::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CRankingDrawer::Draw(void)
{
	if (m_pRank != nullptr) m_pRank->Draw();
}