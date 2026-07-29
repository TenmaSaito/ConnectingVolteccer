//==================================================================================
// 
// 雷エフェクトクラスのヘッダーファイル [thunderEffect.h]
// Author : TENMA SAITO
// Date   : 2026/6/8
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "thunderEffect.h"
#include "meshOrbit3D.h"
#include "rand.h"
#include "matrix.h"
#include "manager.h"
#include "debugproc.h"
#include "texture.h"
#include "color.h"
#include "sparkEffect.h"
#include "vec3math.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define EFFECT_GRADATION_PATH		"data/TEXTURE/gradation102.jpg"		// 雷エフェクトのテクスチャ

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CThunderEffect *CThunderEffect::Create(const D3DXVECTOR3 &min,
	const D3DXVECTOR3 &max,
	const D3DXMATRIX *pMtxParent)
{
	CThunderEffect *pThunder = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pThunder = new CThunderEffect;
	if (pThunder == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pThunder->Init(min, max, pMtxParent);

	return pThunder;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CThunderEffect::CThunderEffect()
{ // 各メンバ変数をクリア
	m_pos = VECTOR3_NULL;
	m_min = VECTOR3_NULL;
	m_max = VECTOR3_NULL;
	m_fMinWidth = 0.0f;
	m_fMaxWidth = 0.0f;
	m_pMtxParent = nullptr;
	m_pOrbit = nullptr;

	// タイプの指定
	CObject::SetType(TYPE_THUNDER);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CThunderEffect::~CThunderEffect()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CThunderEffect::Init(const D3DXVECTOR3 &min,
	const D3DXVECTOR3 &max,
	const D3DXMATRIX *pMtxParent)
{
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ
	VERTEX_3D *pVtx = nullptr;		// 頂点へのポインタ
	D3DXCOLOR col = Color::GetColor(Color::COLOR_YELLOW);

	// オービットの最大最小値を設定
	m_fMinWidth = 3.0f;
	m_fMaxWidth = 30.0f;

	// マトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// マトリックスの計算
	Mtx::CalcWorld(&m_mtxWorld,
		m_pos,
		VECTOR3_NULL);

	// 親マトリックスとリンク
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, pMtxParent);

	// オービットを生成
	m_pOrbit = CMeshOrbit3D::Create(&m_mtxWorld, D3DXVECTOR3(10.0f, 0.0f, 0.0f), D3DXVECTOR3(-10.0f, 0.0f, 0.0f), 10);
	if (m_pOrbit == nullptr)
	{ // 生成失敗
		return E_FAIL;
	}

	// 色を指定
	if (SUCCEEDED(m_pOrbit->LockVertex(&pVtx)))
	{ // ロック成功時
		for (int nCntVtx = 0; nCntVtx < m_pOrbit->GetVertexNum(); nCntVtx++)
		{ // 全頂点の色を変更
			pVtx[nCntVtx].col = col;
		}
	}

	// テクスチャを指定
	m_pOrbit->BindTexture(pTexture->Register(EFFECT_GRADATION_PATH));

	// 親マトリックスを保存,設定
	m_pMtxParent = pMtxParent;

	// 最大最小を保存
	m_min = min;
	m_max = max;

	// 成功
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CThunderEffect::Uninit(void)
{
	if (m_pOrbit != nullptr)
	{ // オービットの破棄 + 終了処理
		m_pOrbit->Uninit();
		m_pOrbit = nullptr;
	}

	// オブジェクトの破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CThunderEffect::Update(void)
{ // 既に死んでいる場合スキップ
	if (IsDeath() == true) return;

	CDebugProc *pProc = CManager::GetInstance()->GetDebugProc();		// デバッグ表示へのポインタ
	CRand *pRand = CRand::GetInstance();							// 乱数生成機へのポインタ
	float fWidth = pRand->Generate(m_fMinWidth, m_fMaxWidth);		// オービットの幅

	// 座標を更新
	m_pos.x = pRand->Generate(m_min.x, m_max.x);

	// オフセットを更新
	m_pOrbit->SetOffset(D3DXVECTOR3(fWidth * 0.5f, 0.0f, 0.0f), D3DXVECTOR3(-fWidth * 0.5f, 0.0f, 0.0f));

	D3DXVECTOR3 posSpark;

	// 自身の位置を取得
	D3DXVec3TransformCoord(&posSpark, &m_pos, m_pMtxParent);

	if (pRand->Generate(0, 100) > 60)
	{ // スパークエフェクトを生成
		CSparkEffect::Create(posSpark,
			D3DXVECTOR3(0.0f, pRand->Generate(0.0f, DOUBLE_PI), 0.0f),
			pRand->Generate(3.0f, 6.0f), 
			pRand->Generate(5, 10));
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CThunderEffect::Draw(void)
{
	// マトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// マトリックスの計算
	Mtx::CalcWorld(&m_mtxWorld,
		m_pMtxParent,
		m_pos,
		VECTOR3_NULL);
}

//==================================================================================
// --- オービットの終了判定処理 ---
//==================================================================================
bool CThunderEffect::IsEndAnim(const float fEpsilon) const
{
	return m_pOrbit->IsFinish(fEpsilon);
}