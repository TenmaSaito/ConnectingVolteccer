//==================================================================================
// 
// タイマークラスのソースファイル [timer.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/20
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "timer.h"
#include "number.h"
#include "manager.h"
#include "renderer.h"
#include "observer_pointer.h"
#include <vector>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define INIT_POS		VECTOR3_NULL		// 初期位置
#define INIT_SIZE		Vector2(50, 65)		// 初期サイズ
#define INIT_TIME		(0)					// 初期タイマー
#define INIT_NUM		(MAX_TIMERNUM)		// 初期数値数
#define TIMER_TEXTYPE	(CNumber::TYPE_VOLTNUM_000)		// タイマーの数字のテクスチャの種類

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CTimer *CTimer::Create(const Vector3 &pos, const Vector2 &size, const int nNumNumber)
{
	CTimer *pTimer = new CTimer;		// 生成したタイマーへのポインタ
	if (pTimer != nullptr)
	{ // タイマーを初期化
		pTimer->Init(pos, size, nNumNumber);
	}

	return pTimer;
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CTimer *CTimer::Create(const Vector3 &pos, const Vector2 &size, const int nNumNumber, const int nTime)
{
	CTimer *pTimer = new CTimer;		// 生成したタイマーへのポインタ
	if (pTimer != nullptr)
	{ // タイマーを初期化
		pTimer->Init(pos, size, nNumNumber, nTime);
	}

	return pTimer;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTimer::CTimer(const int nPriority) : CObject(nPriority)
{ // メンバ変数をクリア
	m_nTime = 0;
	m_nNumTime = 0;
	m_nCounter = 0;
	m_bUpdate = true;

	// タイプを指定
	SetType(TYPE_TIMER);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CTimer::~CTimer()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTimer::Init(void)
{
	Vector2 numSize;			// 数値1個当たりのサイズ
	Vector3 pos = INIT_POS;		// 初期位置

	// 変数を初期化
	m_nTime = INIT_TIME;
	m_nNumTime = INIT_NUM;

	if (m_nNumTime > MAX_TIMERNUM)
	{ // タイマーの最大桁数以上の場合
		// 桁数を最大値に変更
		m_nNumTime = MAX_TIMERNUM;
	}

	// 数値のサイズを計算
	numSize = INIT_SIZE / static_cast<float>(m_nNumTime);

	// 数値オブジェクトを作成
	for (UINT uCntNumber = 0; uCntNumber < m_nNumTime; uCntNumber++)
	{
		CNumber *pNumber = nullptr;			// 生成した数値オブジェクトへのポインタ
		Vector3 posCreate = pos;			// 生成位置

		// 生成位置(X座標)を修正
		posCreate.x = pos.x + (numSize.x * uCntNumber);

		pNumber = CNumber::Create(TIMER_TEXTYPE, posCreate, numSize, 0);
		if (pNumber != NULL)
		{ // 生成成功時
			// 生成したポインタを保存
			m_apNumber.at(uCntNumber).reset(pNumber);
		}
	}

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTimer::Init(const Vector3 &pos, const Vector2 &size, const int nNumNumber)
{
	Vector2 numSize;	// 数値1個当たりのサイズ

	// 引数を保存
	m_nTime = INIT_TIME;
	m_nNumTime = nNumNumber;

	if (m_nNumTime > MAX_TIMERNUM)
	{ // タイマーの最大桁数以上の場合
		// 桁数を最大値に変更
		m_nNumTime = MAX_TIMERNUM;
	}

	// 数値のサイズを計算
	numSize = size / static_cast<float>(nNumNumber);

	// 数値オブジェクトを作成
	for (int nCntNumber = 0; nCntNumber < m_nNumTime; nCntNumber++)
	{
		CNumber *pNumber = nullptr;		// 生成した数値オブジェクトへのポインタ
		Vector3 posCreate;			// 生成位置

		// 生成位置を計算
		posCreate.x = pos.x + (numSize.x * nCntNumber) + (numSize.x * 0.5f);
		posCreate.y = pos.y + (numSize.y * 0.5f);

		// 数値オブジェクトを生成
		pNumber = CNumber::Create(TIMER_TEXTYPE, posCreate, numSize, 0);
		if (pNumber != NULL)
		{ // 生成成功時
			// 生成したポインタを保存
			m_apNumber.at(nCntNumber).reset(pNumber);
		}
		else
		{ // 生成失敗
			return S_FALSE;
		}
	}

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTimer::Init(const Vector3 &pos, const Vector2 &size, const int nNumNumber, const int nTime)
{
	Vector2 numSize;	// 数値1個当たりのサイズ
	std::vector<int> vNumValue;		// 各桁の数値

	// 引数を保存
	m_nTime = nTime;
	m_nNumTime = nNumNumber;

	if (nNumNumber > MAX_TIMERNUM)
	{ // タイマーの最大桁数以上の場合
		// 桁数を最大値に変更
		m_nNumTime = MAX_TIMERNUM;
	}

	// 数値のサイズを計算
	numSize.x = size.x / m_nNumTime;
	numSize.y = size.y;

	// 桁数分だけメモリを確保
	vNumValue.reserve(m_nNumTime);

	for (UINT uCntScore = 0; uCntScore < static_cast<UINT>(m_nNumTime); uCntScore++)
	{ // 各桁の数値を求める
		vNumValue.push_back(nTime % (int)powf(10.0f, (float)(m_nNumTime - uCntScore)) / (int)powf(10.0f, (float)(m_nNumTime - uCntScore) - 1.0f));
	}

	// 数値オブジェクトを作成
	for (int nCntNumber = 0; nCntNumber < m_nNumTime; nCntNumber++)
	{
		CNumber *pNumber = nullptr;		// 生成した数値オブジェクトへのポインタ
		Vector3 posCreate;				// 生成位置

		// 生成位置(X座標)を修正
		posCreate.x = pos.x + (numSize.x * nCntNumber) + (numSize.x * 0.5f);
		posCreate.y = pos.y + (numSize.y * 0.5f);

		// 数値オブジェクトを生成
		pNumber = CNumber::Create(TIMER_TEXTYPE, posCreate, numSize, vNumValue.at(nCntNumber));
		if (pNumber != NULL)
		{ // 生成成功時
			// 生成したポインタを保存
			m_apNumber.at(nCntNumber).reset(pNumber);
		}
	}

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CTimer::Uninit(void)
{
	for (auto &pNumber : m_apNumber)
	{ // タイマーが生成した数値オブジェクトを破棄
		if (pNumber != nullptr)
		{ // NULLではなかった場合、破棄 + 終了処理
			pNumber->Uninit();
			pNumber.reset();
		}
	}

	// 親クラスの終了処理
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CTimer::Update(void)
{
	if (m_nTime > 0 && m_nCounter % 60 == 0 && m_bUpdate == true)
	{ // 0秒より大きい且つ60秒経ったらカウントダウン
		AddTimer(-1);
	}

	for (auto &pNumber : m_apNumber)
	{ // タイマーが生成した数値オブジェクトを更新
		if (pNumber != nullptr)
		{ // NULLではなかった場合、更新処理
			pNumber->Update();
		}
	}

	// フレームカウント増加
	m_nCounter++;
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CTimer::Draw(void)
{
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

	// αテストを無効化
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==================================================================================
// --- タイマーの自動更新フラグ設定処理 ---
//==================================================================================
void CTimer::SetUpdate(const bool bUpdate)
{
	m_bUpdate = bUpdate;
}

//==================================================================================
// --- タイマーの自動更新フラグ取得処理 ---
//==================================================================================
bool CTimer::GetUpdate(void) const
{
	return m_bUpdate;
}

//==================================================================================
// --- タイマー設定処理 ---
//==================================================================================
void CTimer::SetTimer(const int nTime)
{
	std::vector<int> vNumValue;		// 各桁の数値

	// タイマーを加算
	m_nTime = nTime;

	// 桁数分だけメモリを確保
	vNumValue.reserve(m_nNumTime);

	for (int nCntNumber = 0; nCntNumber < m_nNumTime; nCntNumber++) 
	{ // 各桁の数値を求める
		vNumValue.push_back(nTime % (int)powf(10.0f, (float)(m_nNumTime - nCntNumber)) / (int)powf(10.0f, (float)(m_nNumTime - nCntNumber) - 1.0f));
	}

	for (int nCntNumber = 0; nCntNumber < m_nNumTime; nCntNumber++)
	{ // 各桁の数値オブジェクトに適用
		if (m_apNumber.at(nCntNumber) != nullptr)
		{ // NULLじゃなければ
			// 値を変更
			m_apNumber.at(nCntNumber)->SetNumber(vNumValue.at(nCntNumber));
		}
	}
}

//==================================================================================
// --- タイマー追加処理 ---
//==================================================================================
void CTimer::AddTimer(const int nValue)
{
	std::vector<int> vNumValue;		// 各桁の数値

	// タイマーを加算
	m_nTime += nValue;

	// 桁数分だけメモリを確保
	vNumValue.reserve(m_nNumTime);

	for (int nCntNumber = 0; nCntNumber < m_nNumTime; nCntNumber++)
	{ // 各桁の数値を求める
		vNumValue.push_back(m_nTime % (int)powf(10.0f, (float)(m_nNumTime - nCntNumber)) / (int)powf(10.0f, (float)(m_nNumTime - nCntNumber) - 1.0f));
	}

	for (int nCntNumber = 0; nCntNumber < m_nNumTime; nCntNumber++)
	{ // 各桁の数値オブジェクトに適用
		if (m_apNumber.at(nCntNumber) != nullptr)
		{ // NULLじゃなければ
			// 値を変更
			m_apNumber.at(nCntNumber)->SetNumber(vNumValue.at(nCntNumber));
		}
	}
}

//==================================================================================
// --- タイマー取得処理 ---
//==================================================================================
int CTimer::GetTimer(void) const
{
	return m_nTime;
}