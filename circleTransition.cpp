//==================================================================================
// 
// 円状の遷移クラスのソースファイル [circleTransition.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/22
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "circleTransition.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
// フレーム無しの円のテクスチャパス
#define NOFRAME_CIRCLE_PATH		"data/TEXTURE/Transition/Circle/noFrame.png"	
// フレーム有りの円のテクスチャパス
#define FRAME_CIRCLE_PATH		"data/TEXTURE/Transition/Circle/frame.png"
#define MAX_RADIUS				(SCREEN_WIDTH)		// 半径の最大値
#define RADIUS_INCREASE			(23.0f)		// 半径の増加値
#define RADIUS_DECREASE			(23.0f)		// 半径の減少値

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CCircleTransition::CCircleTransition()
{ // メンバ変数をクリア
	ZeroMemory(m_aCircle, sizeof(m_aCircle));
	m_nCountTransition = 0;
	m_nTransitionTime = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CCircleTransition::~CCircleTransition()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CCircleTransition::Init(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();		// レンダラーへのポインタ
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスへのポインタへのポインタ
	VERTEX_2D *pVtx = nullptr;		// 頂点へのポインタ
	int nIdxFrame = -1;				// フレーム有りのテクスチャのインデックス
	int nIdxNoFrame = -1;			// フレーム無しのテクスチャのインデックス
	int nCntCircle = 0;				// 

	// テクスチャ読み込み
	nIdxFrame = pTexture->Register(FRAME_CIRCLE_PATH);
	nIdxNoFrame = pTexture->Register(NOFRAME_CIRCLE_PATH);

	for (auto &circle : m_aCircle)
	{
		Vector3 *pPos = &circle.pos;		// 位置
		Vector3 *pRot = &circle.rot;		// 角度
		float fRadius = circle.fRadius;			// 半径

		// 頂点バッファを作成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_2D,
			D3DPOOL_MANAGED,
			&circle.pVtxBuff,
			nullptr);

		// テクスチャの設定
		circle.nIdxTexture = nIdxNoFrame;

		// 座標を中心に設定し、角度をリセット
		*pPos = WINDOW_MIDDLE;
		*pRot = VECTOR3_NULL;

		// 頂点ロック
		circle.pVtxBuff->Lock(0, 0, (void **)&pVtx, 0);

		// 頂点座標設定
		pVtx[0].pos.x = pPos->x + sinf(pRot->y + D3DX_PI + QUARTER_PI) * fRadius;
		pVtx[0].pos.y = pPos->y + cosf(pRot->y + D3DX_PI + QUARTER_PI) * fRadius;
		pVtx[0].pos.z = 0.0f;

		pVtx[1].pos.x = pPos->x + sinf(pRot->y + D3DX_PI - QUARTER_PI) * fRadius;
		pVtx[1].pos.y = pPos->y + cosf(pRot->y + D3DX_PI - QUARTER_PI) * fRadius;
		pVtx[1].pos.z = 0.0f;

		pVtx[2].pos.x = pPos->x + sinf(pRot->y - QUARTER_PI) * fRadius;
		pVtx[2].pos.y = pPos->y + cosf(pRot->y - QUARTER_PI) * fRadius;
		pVtx[2].pos.z = 0.0f;

		pVtx[3].pos.x = pPos->x + sinf(pRot->y + QUARTER_PI) * fRadius;
		pVtx[3].pos.y = pPos->y + cosf(pRot->y + QUARTER_PI) * fRadius;
		pVtx[3].pos.z = 0.0f;

		// 座標変換用変数設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラー設定
		pVtx[0].col = Color(1.0f, 1.0f / (float)nCntCircle, 1.0f, 1.0f);
		pVtx[1].col = Color(1.0f, 1.0f / (float)nCntCircle, 1.0f, 1.0f);
		pVtx[2].col = Color(1.0f, 1.0f / (float)nCntCircle, 1.0f, 1.0f);
		pVtx[3].col = Color(1.0f, 1.0f / (float)nCntCircle, 1.0f, 1.0f);

		// テクスチャ座標設定
		pVtx[0].tex = Vector2(0.0f, 0.0f);
		pVtx[1].tex = Vector2(1.0f, 0.0f);
		pVtx[2].tex = Vector2(0.0f, 1.0f);
		pVtx[3].tex = Vector2(1.0f, 1.0f);

		// ロック終了
		circle.pVtxBuff->Unlock();

		nCntCircle++;
	}

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CCircleTransition::Uninit(void)
{
	for (auto &circle : m_aCircle)
	{ // 頂点バッファ破棄
		SafeRelease(circle.pVtxBuff);
	}
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CCircleTransition::Update(void)
{
	STATE state = GetState();		// 遷移状態

	if (state == STATE_IN)
	{ // 遷移演出中(ワイプイン)なら
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
void CCircleTransition::Draw(void)
{
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	LPDIRECT3DTEXTURE9 pTexture = nullptr;		// 貼り付けるテクスチャへのポインタ

	for (auto &circle : m_aCircle)
	{ // テクスチャ管理オブジェクトから取得
		pTexture = CTexture::GetInstance()->GetAddress(circle.nIdxTexture);

		// レンダリングターゲットを一時的に元に戻す
		pRenderer->SetDefaultTarget();

		// 頂点バッファをストリームに設定
		pDevice->SetStreamSource(0, circle.pVtxBuff, 0, sizeof(VERTEX_2D));

		// テクスチャ設定
		pDevice->SetTexture(0, pTexture);

		// 頂点フォーマット設定
		pDevice->SetFVF(FVF_VERTEX_2D);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			0,
			2);
	}
}

//==================================================================================
// --- 遷移演出開始処理 ---
//==================================================================================
void CCircleTransition::StartTransition(const int nTransitionTime, const CScene::MODE modeNext)
{ // ワイプイン開始
	SetState(STATE_IN);
	SetModeNext(modeNext);
}

//==================================================================================
// --- 遷移演出終了処理 ---
//==================================================================================
void CCircleTransition::EndTransition(const int nTransitionTime)
{ // ワイプアウト開始
	SetState(STATE_OUT);
}

//==================================================================================
// --- 遷移演出中(ワイプイン)処理 ---
//==================================================================================
void CCircleTransition::UpdateTransitionIn(void)
{
	Circle *pPrev = nullptr;		// 前回の円
	float fRadiusUp = 100.0f;		// 半径を拡大する基準
	float fIncrease = RADIUS_INCREASE;		// 拡大する大きさ

	for (auto &circle : m_aCircle)
	{
		if (pPrev != nullptr)
		{ // 最初の円ではない場合
			if (pPrev->fRadius >= fRadiusUp && circle.fRadius < MAX_RADIUS)
			{ // 前回の円が一定の半径以上且つ最大半径以下なら拡大開始
				circle.fRadius += fIncrease;
			}
		}
		else
		{ // 最初の円なら
			if (circle.fRadius < MAX_RADIUS)
			{ // 最大半径以下なら拡大
				circle.fRadius += fIncrease;
			}
		}

		pPrev = &circle;		// ポインタ更新
	}

	if (pPrev->fRadius >= MAX_RADIUS)
	{ // 一番最後の円のサイズが最大になったら、モード切替
		CManager::GetInstance()->SetMode(GetNextMode());
		SetState(STATE_OUT);
	}
}

//==================================================================================
// --- 遷移演出中(ワイプアウト)処理 ---
//==================================================================================
void CCircleTransition::UpdateTransitionOut(void)
{
	Circle *pNext = nullptr;		// 次の円
	float fRadiusLow = MAX_RADIUS - 100.0f;		// 半径を縮小する基準
	float fDecrease = RADIUS_DECREASE;			// 縮小する大きさ

	for (int nCntCircle = CIRCLE_NUM - 1; nCntCircle > -1; nCntCircle--)
	{
		Circle *pCurrent = &m_aCircle[nCntCircle];		// 現在の円

		if (pNext != nullptr)
		{ // 最初の円ではない場合
			if (pNext->fRadius <= fRadiusLow && pCurrent->fRadius > 0.0f)
			{ // 前回の円が一定の半径以下且つ0.0f以上なら縮小開始
				pCurrent->fRadius -= fDecrease;
			}
		}
		else
		{ // 最初の円なら
			if (pCurrent->fRadius > 0.0f)
			{ // 0.0f以上なら縮小
				pCurrent->fRadius -= fDecrease;
			}
		}

		if (pCurrent->fRadius <= 0.0f)
		{ // 半径が0以下になった場合修正
			pCurrent->fRadius = 0.0f;
		}

		pNext = pCurrent;		// ポインタ更新
	}

	if (pNext->fRadius <= 0.0f)
	{ // 一番最後の円のサイズが最小になったら、ワイプアウト終了
		SetState(STATE_STAY);
	}
}

//==================================================================================
// --- 頂点座標更新処理 ---
//==================================================================================
void CCircleTransition::UpdateVertex(void)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点へのポインタ

	for (auto &circle : m_aCircle)
	{
		Vector3 *pPos = &circle.pos;		// 位置
		Vector3 *pRot = &circle.rot;		// 角度
		float fRadius = circle.fRadius;			// 半径

		// 頂点ロック
		circle.pVtxBuff->Lock(0, 0, (void **)&pVtx, 0);

		// 頂点座標設定
		pVtx[0].pos.x = pPos->x + sinf(pRot->y + D3DX_PI + QUARTER_PI) * fRadius;
		pVtx[0].pos.y = pPos->y + cosf(pRot->y + D3DX_PI + QUARTER_PI) * fRadius;

		pVtx[1].pos.x = pPos->x + sinf(pRot->y + D3DX_PI - QUARTER_PI) * fRadius;
		pVtx[1].pos.y = pPos->y + cosf(pRot->y + D3DX_PI - QUARTER_PI) * fRadius;

		pVtx[2].pos.x = pPos->x + sinf(pRot->y - QUARTER_PI) * fRadius;
		pVtx[2].pos.y = pPos->y + cosf(pRot->y - QUARTER_PI) * fRadius;

		pVtx[3].pos.x = pPos->x + sinf(pRot->y + QUARTER_PI) * fRadius;
		pVtx[3].pos.y = pPos->y + cosf(pRot->y + QUARTER_PI) * fRadius;

		// ロック終了
		circle.pVtxBuff->Unlock();
	}
}