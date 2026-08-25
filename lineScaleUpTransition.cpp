//==================================================================================
// 
// 線拡大の遷移クラスのソースファイル [lineScaleUpTransition.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/22
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "lineScaleUpTransition.h"
#include "polygon2D.h"
#include "meshOrbit2D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "color.h"
#include "rand.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define FIRST_POSITION		(Vector3(0.0f, SCREEN_MIDDLE.y, 0.0f))		// ラインの座標サイズ
#define FIRST_LINESIZE		(Vector2(1.0f, 25.0f))		// ラインの初期サイズ
#define FIRST_OUTLINESIZE	(Vector2(1.0f, 30.0f))		// アウトラインの初期サイズ
#define SCALE_UP_VALUE		(1.15f)			// サイズの拡大倍率
#define SCALE_DOWN_VALUE	(0.85f)			// サイズの縮小倍率
#define ORBIT_POS_Y_RANGE	(125.0f)		// オービットの移動幅
#define MIN_ORBIT_WIDTH		(10.0f)			// オービットの最小サイズ
#define MAX_ORBIT_WIDTH		(25.0f)			// オービットの最大サイズ

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CLineScaleUpTransition::CLineScaleUpTransition()
{ // メンバ変数をクリア
	m_nTransitionTime = 0;
	m_pVoltOrbit2D = nullptr;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CLineScaleUpTransition::~CLineScaleUpTransition()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CLineScaleUpTransition::Init(void)
{ // ポリゴン生成
	CPolygon2D *pLine = nullptr;		// 電線用ポリゴンへのポインタ
	CPolygon2D *pLineOutLine = nullptr;	// 電線アウトライン用ポリゴンへのポインタ
	CPolygon2D *pVolt = nullptr;		// 電流用ポリゴンへのポインタ

	// 電線ポリゴンの生成
	pLine = CPolygon2D::Create(FIRST_POSITION,
		VECTOR3_NULL,
		FIRST_LINESIZE);

	// 灰色に設定
	pLine->SetColor(Colors::GetColor(Colors::C_GRAY));

	// ポインタを登録
	m_pLine.reset(pLine);

	// アウトラインポリゴンの生成
	pLineOutLine = CPolygon2D::Create(FIRST_POSITION,
		VECTOR3_NULL,
		FIRST_OUTLINESIZE);

	// 灰色に設定
	pLineOutLine->SetColor(Colors::GetColor(Colors::C_WHITE));

	// ポインタを登録
	m_pLineOutLine.reset(pLineOutLine);

	// 電流ポリゴンの生成
	pVolt = CPolygon2D::Create(FIRST_POSITION,
		VECTOR3_NULL,
		FIRST_LINESIZE);

	// 黄色に設定
	pVolt->SetColor(Colors::GetColor(Colors::C_YELLOW));

	// ポインタを登録
	m_pVolt.reset(pVolt);
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CLineScaleUpTransition::Uninit(void)
{
	if (m_pLine != nullptr)
	{ // 生成済みなら
		m_pLine->Uninit();
		m_pLine.reset();
	}

	if (m_pLineOutLine != nullptr)
	{ // 生成済みなら
		m_pLineOutLine->Uninit();
		m_pLineOutLine.reset();
	}

	if (m_pVolt != nullptr)
	{ // 生成済みなら
		m_pVolt->Uninit();
		m_pVolt.reset();
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CLineScaleUpTransition::Update(void)
{
	STATE state = GetState();		// 遷移状態

	if (state == STATE_IN)
	{ // 遷移演出中(ワイプイン)なら
		m_nTransitionTime--;		// 時間減少
		UpdateTransitionIn();
	}
	else if (state == STATE_OUT)
	{ // 遷移演出中(ワイプアウト)なら
		UpdateTransitionOut();
	}

	// 頂点座標の更新
	UpdateVertex();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CLineScaleUpTransition::Draw(void)
{ // ポリゴンの描画 (待機状態の場合、描画スキップ)
	if (GetState() == STATE_STAY) return;
	m_pLineOutLine->Draw();
	m_pLine->Draw();
	m_pVolt->Draw();
}

//==================================================================================
// --- 遷移演出開始処理 --
//==================================================================================
void CLineScaleUpTransition::StartTransition(const int nTransitionTime, const CScene::MODE modeNext)
{ // ワイプイン開始
	SetState(STATE_IN);
	SetModeNext(modeNext);

	// 引数の保存
	m_nTransitionTime = nTransitionTime;

	// オービットを生成
	m_pVoltOrbit2D = CMeshOrbit2D::Create(FIRST_POSITION, Vector3(0.0f, HALF_PI, 0.0f), 50, 0.1f);
	m_pVoltOrbit2D->SetColor(Colors::GetColor(Colors::C_YELLOW));
}

//==================================================================================
// --- 遷移演出終了処理 ---
//==================================================================================
void CLineScaleUpTransition::EndTransition(const int nTransitionTime)
{ // ワイプアウト開始
	SetState(STATE_OUT);
}

//==================================================================================
// --- 遷移演出中(ワイプイン)処理 ---
//==================================================================================
void CLineScaleUpTransition::UpdateTransitionIn(void)
{
	const Vector3 *pPos = m_pLine->GetPosition();		// 現在の座標
	const Vector2 *pScale = m_pLine->GetSize();			// 現在のサイズ
	CRand *pRand = CRand::GetInstance();		// 乱数生成インスタンスへのポインタ
	Vector2 scaleNext;		// 次のサイズ

	// 変更後のサイズを計算
	scaleNext.x = pScale->x * SCALE_UP_VALUE;		// 幅は拡張
	scaleNext.y = pScale->y;				// 縦幅は今のサイズを維持

	// 画面サイズを超えないように調整
	if (scaleNext.x > SCREEN_WIDTH) scaleNext.x = SCREEN_WIDTH;

	// サイズを適用
	m_pLine->SetSize(scaleNext);
	m_pLineOutLine->SetSize(Vector2(scaleNext.x, FIRST_OUTLINESIZE.y));

	// 位置が中央になるように調整
	m_pLine->SetPosition(Vector3((pScale->x * 0.5f), pPos->y, pPos->z));
	m_pLineOutLine->SetPosition(Vector3((pScale->x * 0.5f), pPos->y, pPos->z));

	if (scaleNext.x >= SCREEN_WIDTH)
	{ // 画面を電線が横切ったら、電流の更新開始
		pPos = m_pVolt->GetPosition();		// 現在の座標
		pScale = m_pVolt->GetSize();		// 現在のサイズ
		
		// 変更後のサイズを計算
		scaleNext.x = pScale->x * SCALE_UP_VALUE;		// 幅は拡張
		scaleNext.y = (pScale->x >= SCREEN_WIDTH) ? pScale->y * SCALE_UP_VALUE : pScale->y;		// 縦幅は横幅が画面いっぱいになったら拡張

		// 画面サイズを超えないように調整
		if (scaleNext.x > SCREEN_WIDTH) scaleNext.x = SCREEN_WIDTH;
		if (scaleNext.y > SCREEN_HEIGHT) scaleNext.y = SCREEN_HEIGHT;

		// サイズを適用
		m_pVolt->SetSize(scaleNext);

		// 位置が中央になるように調整
		m_pVolt->SetPosition(Vector3((pScale->x * 0.5f), pPos->y, pPos->z));

		// オービットの位置を指定
		m_pVoltOrbit2D->SetPosition(Vector3(pScale->x,
			pPos->y + pRand->Generate(-ORBIT_POS_Y_RANGE, ORBIT_POS_Y_RANGE),
			pPos->z));

		// オービットの幅を指定
		m_pVoltOrbit2D->SetWidth(pRand->Generate(MIN_ORBIT_WIDTH, MAX_ORBIT_WIDTH));

		if (scaleNext == SCREEN_SIZE && m_nTransitionTime <= 0)
		{ // 画面を電流が覆いつくし、指定時間以上経過していれば
			// 元ある電線を消す
			m_pLine->SetDisp(false);
			m_pLineOutLine->SetDisp(false);

			// オービットを破棄
			m_pVoltOrbit2D->Uninit();
			m_pVoltOrbit2D = nullptr;

			// モード切り替え
			CManager::GetInstance()->SetMode(GetNextMode());
			SetState(STATE_OUT);
		}
	}
}

//==================================================================================
// --- 遷移演出中(ワイプアウト)処理 ---
//==================================================================================
void CLineScaleUpTransition::UpdateTransitionOut(void)
{
	const Vector2 *pScale = m_pVolt->GetSize();			// 現在のサイズ
	Vector2 scaleNext;		// 次のサイズ

	// 変更後のサイズを計算
	scaleNext.x = pScale->x;					// 幅はそのまま
	scaleNext.y = pScale->y * SCALE_DOWN_VALUE;	// 縦幅を縮小していく

	if (scaleNext.y <= 0.5f) scaleNext.y = 0.0f;

	// サイズを適用
	m_pVolt->SetSize(scaleNext);

	if (scaleNext.y <= 0.0f)
	{ // 画面からラインが消えたら、ワイプアウト終了
		SetState(STATE_STAY);

		// 電線の状態を元に戻す
		m_pLine->SetPosition(FIRST_POSITION);
		m_pLine->SetSize(FIRST_LINESIZE);
		m_pLine->SetDisp(true);

		// 電線アウトラインの状態を元に戻す
		m_pLineOutLine->SetPosition(FIRST_POSITION);
		m_pLineOutLine->SetSize(FIRST_OUTLINESIZE);
		m_pLineOutLine->SetDisp(true);

		// 電流の状態を元に戻す
		m_pVolt->SetPosition(FIRST_POSITION);
		m_pVolt->SetSize(FIRST_LINESIZE);
	}
}

//==================================================================================
// --- 頂点座標更新処理 ---
//==================================================================================
void CLineScaleUpTransition::UpdateVertex(void)
{
	
}