//==================================================================================
// 
// 2Dポリゴンクラスのソースファイル [polygon2D.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/13
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "polygon2D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPolygon2D *CPolygon2D::Create(const D3DXVECTOR3 &pos,
	const D3DXVECTOR3 &rot,
	const D3DXVECTOR2 &size)
{
	CPolygon2D *pPolygon = new CPolygon2D;		// 生成したポリゴンへのポインタ
	if (pPolygon != nullptr)
	{ // 初期化処理
		pPolygon->Init(pos, rot, size);
	}

	return pPolygon;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPolygon2D::CPolygon2D()
{ // メンバ変数のクリア
	m_pVtxBuff = nullptr;
	m_pTexture = nullptr;
	m_nIdxTexture = -1;
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_size = VECTOR2_NULL;
	m_fLength = 0.0f;
	m_fAngle = 0.0f;
	m_bUseIndex = false;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPolygon2D::~CPolygon2D()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPolygon2D::Init(const D3DXVECTOR3 &pos, 
	const D3DXVECTOR3 &rot, 
	const D3DXVECTOR2 &size)
{
	VERTEX_2D* pVtx = NULL;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();	// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスへのポインタ

	// 引数を保存
	m_pos = pos;
	m_rot = rot;
	m_size = size;

	// 頂点バッファ作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 対角線の長さと角度を求める
	m_fLength = sqrtf(powf(size.x, 2) + powf(size.y, 2)) * 0.5f;
	m_fAngle = atan2f(size.x, size.y);

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.y + D3DX_PI + m_fAngle) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.y + D3DX_PI + m_fAngle) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.y + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.y + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.y - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.y - m_fAngle) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.y + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.y + m_fAngle) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 座標変換用変数設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラー設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPolygon2D::Uninit(void)
{ // バッファの破棄
	SafeRelease(m_pVtxBuff);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPolygon2D::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPolygon2D::Draw(void)
{ 
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	LPDIRECT3DTEXTURE9 pTexture = nullptr;		// 貼り付けるテクスチャへのポインタ

	if (m_bUseIndex == false)
	{ // 登録されたテクスチャを使用
		pTexture = m_pTexture;
	}
	else
	{ // テクスチャ管理オブジェクトから取得
		pTexture = CTexture::GetInstance()->GetAddress(m_nIdxTexture);
	}

	// レンダリングターゲットを一時的に元に戻す
	pRenderer->SetDefaultTarget();

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture);

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);
}

//==================================================================================
// --- テクスチャの登録処理 ---
//==================================================================================
void CPolygon2D::BindTexture(LPDIRECT3DTEXTURE9 pTexture)
{
	m_pTexture = pTexture;
	m_bUseIndex = false;
}

//==================================================================================
// --- テクスチャの登録処理 ---
//==================================================================================
void CPolygon2D::BindTexture(const int nIdxTexture)
{
	m_nIdxTexture = nIdxTexture;
	m_bUseIndex = true;
}

//==================================================================================
// --- 位置の設定処理 ---
//==================================================================================
void CPolygon2D::SetPosition(const D3DXVECTOR3 &position)
{

}

//==================================================================================
// --- 角度の設定処理 ---
//==================================================================================
void CPolygon2D::SetRotation(const D3DXVECTOR3 &rotation)
{

}

//==================================================================================
// --- サイズの設定処理 ---
//==================================================================================
void CPolygon2D::SetSize(const D3DXVECTOR2 &size)
{
	VERTEX_2D* pVtx = NULL;		// 頂点情報へのポインタ

	// サイズの保存
	m_size = size;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.y + D3DX_PI + m_fAngle) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.y + D3DX_PI + m_fAngle) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.y + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.y + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.y - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.y - m_fAngle) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.y + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.y + m_fAngle) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}