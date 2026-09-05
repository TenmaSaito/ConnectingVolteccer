//==================================================================================
// 
// パーセント表示クラスのソースファイル [percent.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/31
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "percent.h"
#include "number.h"
#include "polygon2D.h"
#include <algorithm>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define UPPER_NUM		(3)			// 小数点以上の桁数
#define LOWER_NUM		(2)			// 小数点以下の桁数
static_assert(MAX_PERCENT_NUM >= (UPPER_NUM + LOWER_NUM));		// 桁数が超えていないかチェック

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPercent *CPercent::Create(const Setting &setting)
{
	CPercent *pPercent = new CPercent;		// 生成したオブジェクトへのポインタ
	if (pPercent != nullptr)
	{ // 生成出来ていれば、初期化
		pPercent->Init(setting);
	}
	return pPercent;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPercent::CPercent() : CObject(DEFAULT_UI_PRIORITY)
{ // メンバ変数をクリア
	m_setting = {};

	// タイプを指定
	SetType(TYPE_PERCENT);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPercent::~CPercent()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPercent::Init(const Setting &setting)
{ // 引数を保存
	m_setting = setting;
	std::vector<int> vNumValue;		// 各桁の数値
	const Vector3 &pos = m_setting.start;				// 開始位置
	const Vector2 &numScale = m_setting.numScale;		// 数値のサイズ
	const Vector2 &dotScale = m_setting.dotScale;		// 小数点のサイズ
	const Vector2 &percentScale = m_setting.percentScale;		// パーセントのサイズ
	const int &nNumberType = m_setting.nNumberType;		// 数値のテクスチャタイプ
	const int &nValue = static_cast<int>(m_setting.fValue * 10000.0f);		// int用パーセント

	// 桁数分だけメモリを確保
	vNumValue.reserve(MAX_PERCENT_NUM);

	for (int nCntNumber = 0; nCntNumber < MAX_PERCENT_NUM; nCntNumber++)
	{ // 各桁の数値を求める
		vNumValue.push_back(nValue % (int)powf(10.0f, (float)(MAX_PERCENT_NUM - nCntNumber)) / (int)powf(10.0f, (float)(MAX_PERCENT_NUM - nCntNumber) - 1.0f));
	}

	// 数値オブジェクトを3桁分作成
	for (int nCntNumber = 0; nCntNumber < UPPER_NUM; nCntNumber++)
	{
		Vector3 posNumber;		// 数値オブジェクトの中心座標

		// 中心座標を計算
		posNumber.x = pos.x + (numScale.x * nCntNumber);
		posNumber.y = pos.y;
		posNumber.z = 0.0f;

		// 生成したオブジェクトを保存 (描画はしない)
		m_apNumber.at(nCntNumber).reset(CNumber::Create(static_cast<CNumber::TYPE>(nNumberType),
			posNumber,
			numScale, vNumValue[nCntNumber]));
	}

	// 小数点ポリゴンを作成
	Vector3 posDot;

	// 中心座標を計算
	posDot.x = pos.x + (numScale.x * UPPER_NUM);
	posDot.y = pos.y;
	posDot.z = 0.0f;

	// 小数点を生成
	m_pDot.reset(CPolygon2D::Create(posDot, VECTOR3_NULL, dotScale));
	m_pDot->BindTexture(m_setting.nIdxDotTexture);

	// 数値オブジェクトを2桁分作成
	for (int nCntNumber = 0; nCntNumber < LOWER_NUM; nCntNumber++)
	{
		Vector3 posNumber;		// 数値オブジェクトの中心座標

		// 中心座標を計算
		posNumber.x = pos.x + (numScale.x * (nCntNumber + UPPER_NUM)) + dotScale.x;
		posNumber.y = pos.y;
		posNumber.z = 0.0f;

		// 生成したオブジェクトを保存 (描画はしない)
		m_apNumber.at(nCntNumber + UPPER_NUM).reset(CNumber::Create(static_cast<CNumber::TYPE>(nNumberType),
			posNumber,
			numScale, vNumValue[nCntNumber + UPPER_NUM]));
	}

	// パーセントポリゴンを作成
	Vector3 posPercent;

	// 中心座標を計算
	posPercent.x = pos.x + (numScale.x * (UPPER_NUM + LOWER_NUM)) + dotScale.x;
	posPercent.y = pos.y;
	posPercent.z = 0.0f;

	// パーセントを生成
	m_pPercent.reset(CPolygon2D::Create(posPercent, VECTOR3_NULL, percentScale));
	m_pPercent->BindTexture(m_setting.nIdxPercentTexture);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPercent::Uninit(void)
{
	for (auto &pNumber : m_apNumber)
	{ // 各数値オブジェクトを破棄
		if (pNumber == nullptr) continue;
		pNumber->Uninit();
		pNumber.reset();
	}

	if (m_pDot != nullptr)
	{ // 小数点ポリゴンの破棄
		m_pDot->Uninit();
		m_pDot.reset();
	}

	if (m_pPercent != nullptr)
	{ // パーセントポリゴンの破棄
		m_pPercent->Uninit();
		m_pPercent.reset();
	}

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPercent::Update(void)
{

}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPercent::Draw(void)
{
	for (auto &pNumber : m_apNumber)
	{ // 各数値オブジェクトを描画
		if (pNumber == nullptr) continue;
		pNumber->Draw();
	}

	if (m_pDot != nullptr)
	{ // 小数点ポリゴンの描画
		m_pDot->Draw();
	}

	if (m_pPercent != nullptr)
	{ // パーセントポリゴンの描画
		m_pPercent->Draw();
	}
}

//==================================================================================
// --- パーセントの設定処理 ---
//==================================================================================
void CPercent::SetValue(const float fValue)
{ // 引数を保存
	m_setting.fValue = fValue;

	const int &nValue = static_cast<int>(m_setting.fValue * 10000.0f);		// int用パーセント

	for (int nCntNumber = 0; nCntNumber < MAX_PERCENT_NUM; nCntNumber++)
	{ // 各桁の数値を求める
		int nNumber = nValue % (int)powf(10.0f, (float)(MAX_PERCENT_NUM - nCntNumber)) / (int)powf(10.0f, (float)(MAX_PERCENT_NUM - nCntNumber) - 1.0f);

		// 値を設定
		m_apNumber[nCntNumber]->SetNumber(nNumber);
	}
}