//==================================================================================
// 
// ランキング管理クラスのソースファイル [rankingManager.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/10
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "rankingManager.h"
#include "rankingDrawer.h"
#include "filestream.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define RANKING_PATH	"data/SCORE/ranking.bin"		// ランキングデータ書き出し先

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CRankingManager *CRankingManager::Create(const bool bAddCurrent)
{
	CRankingManager *pRankManager = new CRankingManager;		// 生成したオブジェクトへのポインタ
	if (pRankManager != nullptr)
	{ // 生成出来ていれば初期化
		pRankManager->Init(bAddCurrent);
	}

	return pRankManager;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CRankingManager::CRankingManager()
{ // メンバ変数のクリア
	m_aScore = {};
	m_apRankingDrawer = {};
	m_nCurrentRank = -1;

	// タイプを指定
	SetType(TYPE_RANKING_MANAGER);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CRankingManager::~CRankingManager()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CRankingManager::Init(const bool bAddCurrent)
{ // ランキングを読み込み
	std::unique_ptr pFile = std::make_unique<CFileStream>();		// ファイルストリームへのポインタ
	float fScore = 0.0f;		// 今回のスコア

	// ファイル読み込み失敗時、処理スキップ
	if (pFile->OpenFile(RANKING_PATH, true) == false) return E_FAIL;

	// ランキングデータ読み込み
	pFile->Read(&m_aScore[0], MAX_RANKING_NUM);

	// ファイルを閉じる
	pFile->CloseFile();

	if (bAddCurrent)
	{ // スコアを追加する場合、今回のスコアを読み込み
		if (pFile->OpenFile(CURRENT_SCORE_PATH, true) == false) return S_FALSE;

		// 今回のスコアを読み込み
		pFile->Read(fScore);

		// ファイルを閉じる
		pFile->CloseFile();

		std::array<float, MAX_RANKING_NUM + 1> aSort = {};		// ソート用変数

		// ランキングに乗っているスコアを代入
		std::ranges::copy_n(m_aScore.begin(), MAX_RANKING_NUM, aSort.begin());
		aSort[MAX_RANKING_NUM] = fScore;		// 今回のスコアを代入

		// ランキングを降順ソート
		std::ranges::sort(aSort, std::greater<float>{});

		// ソート後の結果をランキング用ファイルに書き出し
		if (pFile->CreateFile(RANKING_PATH, true, CFileStream::FLAG_OVERWRITE))
		{ // ファイル作成成功時、書き出し
			pFile->Write(&aSort[0], MAX_RANKING_NUM);

			// ファイルを閉じる
			pFile->CloseFile();
		}

		// ソート後の結果を代入
		std::ranges::copy_n(aSort.begin(), MAX_RANKING_NUM, m_aScore.begin());
	}

	m_apRankingDrawer[0].reset(CRankingDrawer::Create(SCREEN_MIDDLE,
		Vector2(300.0f, 185.0f),
		0,
		m_aScore[0],
		false));

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CRankingManager::Uninit(void)
{ 
	for (auto &drawer : m_apRankingDrawer)
	{ // nullの場合はスキップ
		if (drawer == nullptr) continue;

		// 破棄
		drawer->Uninit();
		drawer.reset();
	}

	// 破棄
	CObject::Release();
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CRankingManager::Update(void)
{ 
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CRankingManager::Draw(void)
{
	for (auto &drawer : m_apRankingDrawer)
	{ // nullの場合はスキップ
		if (drawer == nullptr) continue;

		// 描画
		drawer->Draw();
	}
}