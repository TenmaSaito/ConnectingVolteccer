//==================================================================================
// 
// 接続時評価表示クラスのソースファイル [connectingEvaluate.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/27
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "connectingEvaluate.h"
#include "polygon2D.h"
#include "manager.h"
#include "texture.h"
#include <string_view>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_RESERVE		(30)		// 配列の初期サイズ
#define DEFAULT_SPD			(1.0f)		// デフォルトの上に上昇する係数
#define POLYGON_LIFE		(CManager::SecToFrame(1.0f))	// ポリゴンの寿命
#define POLYGON_ALPHA_DECREASE		(1.0f / POLYGON_LIFE)	// ポリゴンのα値の減少係数

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr std::string_view c_asEvaluatePath[CConnectingEvaluate::EVALUATE_MAX] =	
	{ // 各評価のテクスチャパス
		"none",		// 評価無し
		"data/TEXTURE/evaluate/GOOD.png",		// GOOD判定
		"data/TEXTURE/evaluate/GREAT.png",		// GREAT判定
		"data/TEXTURE/evaluate/EXCELLENT.png",	// EXCELLENT判定
	};

	constexpr int c_aEvaluateLine[CConnectingEvaluate::EVALUATE_MAX] =
	{ // 各評価のライン
		0,		// 評価無し (0軒)
		1,		// GOOD判定	(1～4軒)
		5,		// GREAT判定 (5～9軒)
		10		// EXCELLENT判定 (10軒以上)
	};
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CConnectingEvaluate *CConnectingEvaluate::Create(const Vector3 &pos, const Vector2 &scale)
{
	CConnectingEvaluate *pEval = new CConnectingEvaluate;		// 生成したインスタンスへのポインタ
	if (pEval != nullptr)
	{ // 生成出来ていれば、初期化
		pEval->Init(pos, scale);
	}

	return pEval;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CConnectingEvaluate::CConnectingEvaluate() : CObject(DEFAULT_UI_PRIORITY)
{ // メンバ変数を初期化
	m_pos = VECTOR3_NULL;
	m_scale = VECTOR2_NULL;
	m_vPolyInfo.reserve(DEFAULT_RESERVE);

	// タイプを指定
	SetType(TYPE_EVALUATE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CConnectingEvaluate::~CConnectingEvaluate()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CConnectingEvaluate::Init(const Vector3 &pos, const Vector2 &scale)
{ // 引数を保存
	m_pos = pos;
	m_scale = scale;
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CConnectingEvaluate::Uninit(void)
{
	for (auto &pPolyInfo : m_vPolyInfo)
	{ // 今出現しているポリゴンを破棄
		if (pPolyInfo.pPoly == nullptr) continue;		// 存在しない場合スキップ

		// ポリゴンの破棄
		pPolyInfo.pPoly->Uninit();
		pPolyInfo.pPoly.reset();
	}
	
	// 配列をクリア
	m_vPolyInfo.clear();

	// オブジェクトを解放
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CConnectingEvaluate::Update(void)
{ // ポリゴンの更新
	PolygonsUpdate();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CConnectingEvaluate::Draw(void)
{
	for (auto &pPolyInfo : m_vPolyInfo)
	{ // 今出現しているポリゴンを描画
		if (pPolyInfo.pPoly == nullptr) continue;		// 存在しない場合スキップ

		// ポリゴンを描画
		pPolyInfo.pPoly->Draw();
	}
}

//==================================================================================
// --- 評価追加処理 ---
//==================================================================================
void CConnectingEvaluate::AddEvaluate(const int nNumLightingHouse)
{ 
	EVALUATE eval = EVALUATE_NONE;		// 評価結果

	for (int nCntEvaluate = 0; nCntEvaluate < EVALUATE_MAX; nCntEvaluate++)
	{ // どれかの評価に当たるまで繰り返し
		if (nNumLightingHouse >= c_aEvaluateLine[nCntEvaluate])
		{ // 基準値以下だった場合、そのタイプを保存
			eval = static_cast<EVALUATE>(nCntEvaluate);
		}
	}

	// 評価無し(もしくは、範囲外)だった場合、処理スキップ
	if (eval == EVALUATE_NONE || eval == EVALUATE_MAX) return;

	// 評価に合わせたポリゴンを生成
	CreatePolygon(eval);
}

//==================================================================================
// --- 評価ポリゴンの生成処理 ---
//==================================================================================
void CConnectingEvaluate::CreatePolygon(const EVALUATE eval)
{
	PolyInfo info = {};		// ポリゴン情報

	// 体力を設定
	info.nLife = POLYGON_LIFE;

	// ポリゴンを生成 + テクスチャを指定
	info.pPoly.reset(CPolygon2D::Create(m_pos, VECTOR3_NULL, m_scale));
	info.pPoly->BindTexture(CTexture::GetInstance()->Register(c_asEvaluatePath[eval]));

	// ポリゴン情報を登録
	m_vPolyInfo.push_back(std::move(info));
}

//==================================================================================
// --- 各ポリゴンの更新処理 ---
//==================================================================================
void CConnectingEvaluate::PolygonsUpdate(void)
{
	for (auto itr = m_vPolyInfo.begin(); itr != m_vPolyInfo.end();)
	{ // 各ポリゴンの更新
		auto &pPolyInfo = *itr;				// ポリゴン情報
		auto &pPoly = pPolyInfo.pPoly;		// ポリゴンへのポインタ
		if (pPoly == nullptr) continue;		// 存在しない場合スキップ

		// Y座標を上にずらす
		pPoly->SetPosition(*pPoly->GetPosition() - Vector3(0.0f, DEFAULT_SPD, 0.0f));

		// α値を減らす
		pPoly->SetAlpha(pPoly->GetColor()->a - POLYGON_ALPHA_DECREASE);

		// 寿命を減らす
		pPolyInfo.nLife--;
		if (pPolyInfo.nLife <= 0)
		{ // 寿命が尽きた場合
			// ポリゴンの破棄
			pPolyInfo.pPoly->Uninit();
			pPolyInfo.pPoly.reset();

			// 配列から削除後、次のイテレータを受け取る
			itr = m_vPolyInfo.erase(itr);
		}
		else
		{ // まだ生きている場合、イテレーターを進める
			++itr;
		}
	}
}