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

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define INIT_POS		VECTOR3_NULL		// 初期位置
#define INIT_SIZE		D3DXVECTOR2(50, 65)	// 初期サイズ
#define INIT_TIME		(0)					// 初期タイマー
#define INIT_NUM		(MAX_TIMERNUM)		// 初期数値数

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CTimer *CTimer::Create(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber)
{
	CTimer *pTimer;		// 生成したタイマーへのポインタ

	if (CObject::GetNumAll() >= MAX_OBJECT)
	{ // オブジェクトの総数が配列の最大数だった場合
		// 生成せずにNULLを返す
		return nullptr;
	}

	// タイマーを生成
	pTimer = new CTimer;
	if (pTimer == nullptr)
	{ // 生成失敗
		return nullptr;
	}

	// タイマーを初期化
	pTimer->Init(pos, size, nNumNumber);

	return pTimer;
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CTimer *CTimer::Create(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber, const int nTime)
{
	CTimer *pTimer;		// 生成したタイマーへのポインタ

	if (CObject::GetNumAll() >= MAX_OBJECT)
	{ // オブジェクトの総数が配列の最大数だった場合
		// 生成せずにNULLを返す
		return nullptr;
	}

	// タイマーを生成
	pTimer = new CTimer;
	if (pTimer == nullptr)
	{ // 生成失敗
		return nullptr;
	}

	// タイマーを初期化
	pTimer->Init(pos, size, nNumNumber, nTime);

	return pTimer;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTimer::CTimer(const int nPriority) : CObject(nPriority)
{
	// メンバ変数をクリア
	memset(m_apNumber, 0, sizeof(m_apNumber));
	m_nTime = 0;
	m_nNumTime = 0;
	m_nCounter = 0;
	m_bUpdate = true;
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
	D3DXVECTOR2 numSize;			// 数値1個当たりのサイズ
	D3DXVECTOR3 pos = INIT_POS;		// 初期位置

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
	for (int nCntNumber = 0; nCntNumber < m_nNumTime; nCntNumber++)
	{
		CNumber* pNumber = nullptr;			// 生成した数値オブジェクトへのポインタ
		D3DXVECTOR3 posCreate = pos;	// 生成位置

		// 生成位置(X座標)を修正
		posCreate.x = pos.x + (numSize.x * nCntNumber);

		pNumber = CNumber::Create(posCreate, numSize, 0);
		if (pNumber != NULL)
		{ // 生成成功時
			// 生成したポインタを保存 (CObjectによって解放されるためdeleteは禁止)
			m_apNumber[nCntNumber] = pNumber;
		}
	}

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTimer::Init(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber)
{
	D3DXVECTOR2 numSize;	// 数値1個当たりのサイズ

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
		D3DXVECTOR3 posCreate;			// 生成位置

		// 生成位置を計算
		posCreate.x = pos.x + (numSize.x * nCntNumber) + (numSize.x * 0.5f);
		posCreate.y = pos.y + (numSize.y * 0.5f);

		// 数値オブジェクトを生成
		pNumber = CNumber::Create(posCreate, numSize, 0);
		if (pNumber != NULL)
		{ // 生成成功時
			// 生成したポインタを保存 (CObjectによって解放されるためdeleteは禁止)
			m_apNumber[nCntNumber] = pNumber;
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
HRESULT CTimer::Init(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber, const int nTime)
{
	D3DXVECTOR2 numSize;	// 数値1個当たりのサイズ
	int *pNumValue;			// 各桁の数値

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
	pNumValue = new int[m_nNumTime];

	for (int nCntScore = 0; nCntScore < m_nNumTime; nCntScore++)
	{ // 各桁の数値を求める
		pNumValue[nCntScore] = nTime % (int)powf(10.0f, (float)(m_nNumTime - nCntScore)) / (int)powf(10.0f, (float)(m_nNumTime - nCntScore) - 1.0f);
	}

	// 数値オブジェクトを作成
	for (int nCntNumber = 0; nCntNumber < m_nNumTime; nCntNumber++)
	{
		CNumber *pNumber = nullptr;		// 生成した数値オブジェクトへのポインタ
		D3DXVECTOR3 posCreate;			// 生成位置

		// 生成位置(X座標)を修正
		posCreate.x = pos.x + (numSize.x * nCntNumber) + (numSize.x * 0.5f);
		posCreate.y = pos.y + (numSize.y * 0.5f);

		// 数値オブジェクトを生成
		pNumber = CNumber::Create(posCreate, numSize, pNumValue[nCntNumber]);
		if (pNumber != NULL)
		{ // 生成成功時
			// 生成したポインタを保存 (CObjectによって解放されるためdeleteは禁止)
			m_apNumber[nCntNumber] = pNumber;
		}
	}

	if (pNumValue != nullptr)
	{ // メモリ解放
		delete[] pNumValue;
		pNumValue = nullptr;
	}

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CTimer::Uninit(void)
{
	for (int nCntNumber = 0; nCntNumber < MAX_TIMERNUM; nCntNumber++)
	{ // タイマーが生成した数値オブジェクトを破棄
		if (m_apNumber[nCntNumber] != nullptr)
		{ // NULLではなかった場合、破棄 + 終了処理
			m_apNumber[nCntNumber]->Uninit();
			delete m_apNumber[nCntNumber];
			m_apNumber[nCntNumber] = nullptr;
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
	auto pManager = CManager::GetInstance();		// マネージャへのポインタ

	if (m_nTime > 0 && m_nCounter % 60 == 0 && m_bUpdate == true)
	{ // 0秒より大きい且つ60秒経ったらカウントダウン
		AddTimer(-1);
	}

	for (int nCntNumber = 0; nCntNumber < MAX_TIMERNUM; nCntNumber++)
	{ // タイマーが生成した数値オブジェクトを更新
		if (m_apNumber[nCntNumber] != nullptr)
		{ // NULLではなかった場合、破棄 + 終了処理
			m_apNumber[nCntNumber]->Update();
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
	for (int nCntNumber = 0; nCntNumber < MAX_TIMERNUM; nCntNumber++)
	{ // タイマーが生成した数値オブジェクトを更新
		if (m_apNumber[nCntNumber] != nullptr)
		{ // NULLではなかった場合、破棄 + 終了処理
			m_apNumber[nCntNumber]->Draw();
		}
	}
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
	int *pNumValue;			// 各桁の数値

	// タイマーを加算
	m_nTime = nTime;

	// 桁数分だけメモリを確保
	pNumValue = new int[m_nNumTime];

	for (int nCntScore = 0; nCntScore < m_nNumTime; nCntScore++)
	{ // 各桁の数値を求める
		pNumValue[nCntScore] = m_nTime % (int)powf(10.0f, (float)(m_nNumTime - nCntScore)) / (int)powf(10.0f, (float)(m_nNumTime - nCntScore) - 1.0f);
	}

	for (int nCntScore = 0; nCntScore < m_nNumTime; nCntScore++)
	{ // 各桁の数値オブジェクトに適用
		if (m_apNumber[nCntScore] != nullptr)
		{ // NULLじゃなければ
			// 値を変更
			m_apNumber[nCntScore]->SetNumber(pNumValue[nCntScore]);
		}
	}

	if (pNumValue != nullptr)
	{ // メモリ解放
		delete[] pNumValue;
		pNumValue = nullptr;
	}
}

//==================================================================================
// --- タイマー追加処理 ---
//==================================================================================
void CTimer::AddTimer(const int nValue)
{
	int *pNumValue;			// 各桁の数値

	// タイマーを加算
	m_nTime += nValue;

	// 桁数分だけメモリを確保
	pNumValue = new int[m_nNumTime];

	for (int nCntScore = 0; nCntScore < m_nNumTime; nCntScore++)
	{ // 各桁の数値を求める
		pNumValue[nCntScore] = m_nTime % (int)powf(10.0f, (float)(m_nNumTime - nCntScore)) / (int)powf(10.0f, (float)(m_nNumTime - nCntScore) - 1.0f);
	}

	for (int nCntScore = 0; nCntScore < m_nNumTime; nCntScore++)
	{ // 各桁の数値オブジェクトに適用
		if (m_apNumber[nCntScore] != nullptr)
		{ // NULLじゃなければ
			// 値を変更
			m_apNumber[nCntScore]->SetNumber(pNumValue[nCntScore]);
		}
	}

	if (pNumValue != nullptr)
	{ // メモリ解放
		delete[] pNumValue;
		pNumValue = nullptr;
	}
}

//==================================================================================
// --- タイマー取得処理 ---
//==================================================================================
int CTimer::GetTimer(void) const
{
	return m_nTime;
}