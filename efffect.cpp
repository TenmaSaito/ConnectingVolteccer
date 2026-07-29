//==================================================================================
// 
// エフェクトクラスのソースファイル [effect.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/12
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "effect.h"
#include "manager.h"
#include "renderer.h"
#include "vec2math.h"
#include "texture.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define INIT_WIDTH		(100)		// 初期の幅
#define INIT_HEIGHT		(100)		// 初期の高さ
#define TEX_PATH		"data/TEXTURE/effect000.jpg"		// エフェクトのテクスチャ

//**********************************************************************************
// *** 静的メンバ変数 ***
//**********************************************************************************
int CEffect::m_nIdxTexture = -1;			// テクスチャインデックス

//==================================================================================
// --- テクスチャの読み込み処理 ---
//==================================================================================
HRESULT CEffect::Load(void)
{
	return S_OK;
}

//==================================================================================
// --- 数値オブジェクトの生成処理 (位置、サイズ指定) ---
//==================================================================================
CEffect *CEffect::Create(const D3DXVECTOR3 &pos, 
	const int nLife,
	const float fRadius,
	const D3DXVECTOR3 &move,
	const float fGravity)
{
	CEffect *pEffect = nullptr;		// 生成したオブジェクトへのポインタ
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ

	// オブジェクトの生成
	pEffect = new CEffect;
	if (pEffect == nullptr)
	{ // 生成に失敗した場合、NULLを返す
		return nullptr;
	}

	// 初期化処理
	pEffect->Init(pos, nLife, fRadius, move, fGravity);

	// テクスチャを適用
	pEffect->BindTexture(pTexture->Register(TEX_PATH));

	return pEffect;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CEffect::CEffect(const int nPriority) : CObjectBillboard(nPriority)
{ // メンバ変数をクリア
	m_move = VECTOR3_NULL;
	m_nLife = 0;
	m_fRadius = 0.0f;
	m_fDecreaseRadius = 0.0f;
	m_fGravity = 0.0f;

	// タイプを指定
	SetType(TYPE_EFFECT);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CEffect::~CEffect()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CEffect::Init(const D3DXVECTOR3 &pos, 
	const int nLife, 
	const float fRadius,
	const D3DXVECTOR3 &move,
	const float fGravity)
{
	HRESULT hr = S_OK;			// 関数の結果
	VERTEX_3D *pVtx = NULL;		// 頂点情報へのポインタ
	D3DXVECTOR2 size = {};		// ポリゴンサイズ

	// 親クラスの初期化
	hr = CObjectBillboard::Init(pos, Vec2::ToSquareSize(fRadius));

	// 引数を保存
	m_move = move;
	m_nLife = nLife;
	m_fRadius = fRadius;
	m_fGravity = fGravity;

	// 体力と半径から、各フレームの半径減少係数を求める
	m_fDecreaseRadius = m_fRadius / static_cast<float>(m_nLife);

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CEffect::Uninit(void)
{ // 親クラスの終了処理
	CObjectBillboard::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CEffect::Update(void)
{
	D3DXVECTOR3 pos = CObjectBillboard::GetPosition();		// エフェクトの位置

	// エフェクトを移動
	pos += m_move;

	// 重力を掛ける
	pos.y -= m_fGravity;

	// 位置を適用
	CObjectBillboard::SetPosition(pos);

	// 体力を減らす
	m_nLife--;
	if (m_nLife <= 0)
	{ // 寿命が尽きた場合、消滅
		Uninit();
	}
	else
	{ // 半径減少
		m_fRadius -= m_fDecreaseRadius;

		// 減少した半径を適用
		CObjectBillboard::SetSize(Vec2::ToSquareSize(m_fRadius));
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CEffect::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetDeviceByInstance();		// デバイスの取得

	// ライティングを無効に設定
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

	// 加算合成開始
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 親クラスの描画処理
	CObjectBillboard::Draw();

	// 加算合成終了
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ライティングを有効に設定
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}