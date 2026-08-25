//==================================================================================
// 
// メッシュオービット2Dクラスのソースファイル [meshOrbit2D.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/30
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "meshOrbit2D.h"
#include "mesh2D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "vec2math.h"
#include "vec3math.h"
#include "texture.h"
#include "particle.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define FILE_PATH		"data/TEXTURE/gradation102.jpg"		// テクスチャパス

//==================================================================================
// --- オービットの生成処理 ---
//==================================================================================
CMeshOrbit2D *CMeshOrbit2D::Create(const D3DXVECTOR3 &pos,
	const D3DXVECTOR3 &rot,
	const int nNumOrbit,
	const float fWidth)
{
	CMeshOrbit2D *pOrbit2D = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pOrbit2D = new CMeshOrbit2D;
	if (pOrbit2D == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pOrbit2D->Init(nNumOrbit, fWidth);

	// 位置と角度を設定
	pOrbit2D->ResetPosition(pos);
	pOrbit2D->ResetRotation(rot);

	// テクスチャを紐づけ
	pOrbit2D->m_pMesh->BindTexture(CTexture::GetInstance()->Register(FILE_PATH));

	return pOrbit2D;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMeshOrbit2D::CMeshOrbit2D(const int nPriority) : CObject(nPriority)
{ // メンバ変数のクリア
	m_pMesh = nullptr;
	m_pPos = nullptr;
	m_rot = VECTOR3_NULL;
	m_fWidth = 0.0f;
	m_nNumPosition = 0;

	// タイプを設定
	SetType(TYPE_ORBIT2D);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMeshOrbit2D::~CMeshOrbit2D()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CMeshOrbit2D::Init(const int nNumOrbit, const float fWidth)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点バッファへのポインタ
	WORD *pIdx = nullptr;		// インデックスへのポインタ
	HRESULT hr = S_OK;			// メッシュ生成の判定
	int nNumVtx = 4 + (2 * (nNumOrbit - 1));		// 頂点数
	int nNumIdx = 4 + (2 * (nNumOrbit - 1));		// インデックス数

	// メッシュを生成
	if (m_pMesh == nullptr)
	{ // 未だ生成されていなければ、メッシュを生成
		m_pMesh = CMesh2D::Create(nNumVtx, nNumIdx, nNumIdx - 2);
		if (m_pMesh == nullptr)
		{ // メッシュの生成失敗時
			return E_FAIL;
		}
	}

	// 生成したメッシュの頂点バッファを取得
	if (SUCCEEDED(m_pMesh->LockVertex(&pVtx)))
	{
		for (int nCntVtx = 0; nCntVtx < m_pMesh->GetVertexNum(); nCntVtx++)
		{ // 頂点バッファを更新
			pVtx[nCntVtx].rhw = 1.0f;
			pVtx[nCntVtx].col = COLOR_ONE;

			pVtx[nCntVtx].tex = D3DXVECTOR2((nCntVtx % 4) / 2, nCntVtx % 2);
		}
	}

	// 生成したメッシュのインデックスを設定
	if (SUCCEEDED(m_pMesh->LockIndex(&pIdx)))
	{ // ロック成功時
		for (int nCntIdx = 0; nCntIdx < nNumIdx; nCntIdx++)
		{ // インデックス設定
			pIdx[nCntIdx] = nCntIdx;
		}
	}

	// ロック解除
	m_pMesh->UnlockIndex();

	// 座標のバッファを生成
	if (m_pPos == nullptr)
	{ // 未だ生成されていなければ、座標のバッファを生成
		m_pPos = new D3DXVECTOR3[nNumVtx];
		if (m_pPos == nullptr)
		{ // 座標のバッファの生成失敗時
			return E_FAIL;
		}
	}

	// 引数の値を保存
	m_nNumPosition = nNumVtx;
	m_fWidth = fWidth;

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMeshOrbit2D::Uninit(void)
{
	// メッシュの破棄
	if (m_pMesh != nullptr)
	{ // 生成されていれば、解放
		m_pMesh->Uninit();
		m_pMesh = nullptr;
	}

	// 座標を解放
	if (m_pPos != nullptr)
	{ // 生成されていれば、解放
		delete[] m_pPos;
		m_pPos = nullptr;
	}

	// オブジェクトの解放
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMeshOrbit2D::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CMeshOrbit2D::Draw(void)
{ // メッシュの描画
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイスの取得

	// 加算合成開始
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_pMesh->Draw();

	// 加算合成終了
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//==================================================================================
// --- オービットの先頭位置の設定処理 ---
//==================================================================================
void CMeshOrbit2D::SetPosition(const D3DXVECTOR3 &pos)
{
	size_t moveSize = sizeof(D3DXVECTOR3) * (m_nNumPosition - 2);		// ずらすバッファのバイト数
	int nNumVtx = m_pMesh->GetVertexNum();		// 頂点数
	VERTEX_2D *pVtx = nullptr;		// 頂点バッファへのポインタ

	// 位置をずらす
	memmove(m_pPos[2], m_pPos[0], moveSize);

	// 位置を保存
	m_pPos[0] = Vec2::ToVector3(Vec2::Arc(m_fWidth * 0.5f, m_rot.y + HALF_PI, Vec3::ToVector2(pos)));
	m_pPos[1] = Vec2::ToVector3(Vec2::Arc(m_fWidth * 0.5f, m_rot.y - HALF_PI, Vec3::ToVector2(pos)));

	// 頂点バッファをロック
	if (FAILED(m_pMesh->LockVertex(&pVtx)))
	{ // ロック失敗
		return;
	}

	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{ // 頂点バッファを更新
		pVtx[nCntVtx].pos = m_pPos[nCntVtx];
	}

	// ロック解除
	m_pMesh->UnlockVertex();
}

//==================================================================================
// --- オービット全体の位置の設定処理 ---
//==================================================================================
void CMeshOrbit2D::ResetPosition(const D3DXVECTOR3 &pos)
{
	size_t moveSize = sizeof(D3DXVECTOR3) * (m_nNumPosition - 2);		// ずらすバッファのバイト数
	VERTEX_2D *pVtx = nullptr;		// 頂点バッファへのポインタ

	// 位置を保存
	m_pPos[0] = Vec2::ToVector3(Vec2::Arc(m_fWidth * 0.5f, m_rot.y + HALF_PI, Vec3::ToVector2(pos)));
	m_pPos[1] = Vec2::ToVector3(Vec2::Arc(m_fWidth * 0.5f, m_rot.y - HALF_PI, Vec3::ToVector2(pos)));

	// 位置を全体に適用
	for (int nCntPosition = 0; nCntPosition < m_nNumPosition; nCntPosition++)
	{
		m_pPos[nCntPosition] = m_pPos[nCntPosition % 2];
	}

	// 頂点バッファをロック
	if (FAILED(m_pMesh->LockVertex(&pVtx)))
	{ // ロック失敗
		return;
	}

	for (int nCntVtx = 0; nCntVtx < m_pMesh->GetVertexNum(); nCntVtx++)
	{ // 頂点バッファを更新
		pVtx[nCntVtx].pos = m_pPos[nCntVtx];
	}

	// ロック解除
	m_pMesh->UnlockVertex();
}

//==================================================================================
// --- オービットの先頭角度の設定処理 ---
//==================================================================================
void CMeshOrbit2D::SetRotation(const D3DXVECTOR3 &rot)
{ // 角度を保存
	m_rot = Vec3::FixedRotation(rot);
}

//==================================================================================
// --- オービット全体の角度の設定処理 ---
//==================================================================================
void CMeshOrbit2D::ResetRotation(const D3DXVECTOR3 &rot)
{ // 角度を保存
	m_rot = Vec3::FixedRotation(rot);
}

//==================================================================================
// --- オービットの先頭幅の設定処理 ---
//==================================================================================
void CMeshOrbit2D::SetWidth(const float fWidth)
{
	m_fWidth = fWidth;
}

//==================================================================================
// --- オービットの色の設定処理 ---
//==================================================================================
void CMeshOrbit2D::SetColor(const Color &col)
{
	VERTEX_2D *pVtx = nullptr;		// 頂点バッファへのポインタ

	// 頂点バッファをロック
	if (FAILED(m_pMesh->LockVertex(&pVtx)))
	{ // ロック失敗
		return;
	}

	for (int nCntVtx = 0; nCntVtx < m_pMesh->GetVertexNum(); nCntVtx++)
	{ // 頂点バッファを更新
		pVtx[nCntVtx].col = col;
	}

	// ロック解除
	m_pMesh->UnlockVertex();
}