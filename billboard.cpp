//==================================================================================
// 
// ビルボードクラスのソースファイル [billboard.h]
// Author : TENMA SAITO
// Date   : 2026/7/14
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "billboard.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "matrix.h"
#include "effect.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CBillboard *CBillboard::Create(const Vector3 &pos, const Vector2 &size)
{
	CBillboard *pBillboard = new CBillboard;		// 生成したビルボード
	NULLPOINTER_ASSERT(pBillboard);

	if (pBillboard != nullptr)
	{ // 初期化処理
		pBillboard->Init(pos, size);
	}

	return pBillboard;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CBillboard::CBillboard()
{ // メンバ変数のクリア
	m_pVtxBuff = nullptr;
	m_pTexture = nullptr;
	m_pMtxParent = nullptr;
	m_nIdxTexture = -1;
	m_pos = VECTOR3_NULL;
	m_size = VECTOR2_NULL;
	m_bUseIndex = false;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CBillboard::~CBillboard()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CBillboard::Init(const Vector3 &pos, const Vector2 &size)
{
	VERTEX_3D *pVtx = NULL;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();	// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスへのポインタ

	// 引数を保存
	m_pos = pos;
	m_size = size;

	// 頂点バッファ作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = -m_size.x * 0.5f;
	pVtx[0].pos.y = m_size.y * 0.5f;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_size.x * 0.5f;
	pVtx[1].pos.y = m_size.y * 0.5f;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = -m_size.x * 0.5f;
	pVtx[2].pos.y = -m_size.y * 0.5f;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_size.x * 0.5f;
	pVtx[3].pos.y = -m_size.y * 0.5f;
	pVtx[3].pos.z = 0.0f;

	// 座標変換用変数設定
	pVtx[0].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = Vector3(0.0f, 0.0f, -1.0f);

	// 頂点カラー設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標設定
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CBillboard::Uninit(void)
{ // バッファ解放
	SafeRelease(m_pVtxBuff);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CBillboard::Update(void)
{

}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CBillboard::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	Matrix mtxView;		// ビューマトリックス

	/*** カメラのビューマトリックスを取得 ***/
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	/*** マトリックスの逆行列を求める (※ 位置を反映する前に必ず行うこと！) ***/
	D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);

	/** 逆行列によって入ってしまった位置情報を初期化 **/
	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;

	// 位置の計算
	Mtx::CalcPosition(&m_mtxWorld, m_pos);

	if (m_pMtxParent)
	{ // 親が存在するならマトリックスを適用
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, m_pMtxParent);
	}

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャ設定
	if (m_bUseIndex)
	{
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));
	}
	else
	{
		pDevice->SetTexture(0, m_pTexture);
	}

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);
}

//==================================================================================
// --- テクスチャの登録処理 ---
//==================================================================================
void CBillboard::BindTexture(LPDIRECT3DTEXTURE9 pTexture)
{
	m_pTexture = pTexture;
	m_bUseIndex = false;
}

//==================================================================================
// --- テクスチャの登録処理 ---
//==================================================================================
void CBillboard::BindTexture(const int nIdxTexture)
{
	m_nIdxTexture = nIdxTexture;
	m_bUseIndex = true;
}

//==================================================================================
// --- サイズの設定処理 ---
//==================================================================================
void CBillboard::SetSize(const Vector2 &size)
{
	VERTEX_3D *pVtx = NULL;		// 頂点情報へのポインタ

	// サイズを保存
	m_size = size;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = -m_size.x * 0.5f;
	pVtx[0].pos.y = -m_size.y * 0.5f;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_size.x * 0.5f;
	pVtx[1].pos.y = -m_size.y * 0.5f;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = -m_size.x * 0.5f;
	pVtx[2].pos.y = m_size.y * 0.5f;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_size.x * 0.5f;
	pVtx[3].pos.y = m_size.y * 0.5f;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}