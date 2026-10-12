//==================================================================================
// 
// メッシュシリンダークラスのソースファイル [meshCylinder.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/30
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "meshCylinder.h"
#include "manager.h"
#include "renderer.h"
#include "matrix.h"
#include "vec3math.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CMeshCylinder *CMeshCylinder::Create(const int nNumAngle,
	const int nNumVertical,
	const Vector3 &pos,
	const Vector3 &rot,
	const float fRadius,
	const float fHeight)
{
	CMeshCylinder *pMesh = new CMeshCylinder;		// 生成したメッシュへのポインタ
	if (pMesh != nullptr)
	{ // 生成できていれば初期化
		pMesh->Init(nNumAngle, nNumVertical, pos, rot, fRadius, fHeight);
	}

	return pMesh;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMeshCylinder::CMeshCylinder()
{ // メンバ変数をクリア
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_fRadius = 0.0f;
	m_fHeight = 0.0f;
	m_nNumAngle = 0;
	m_nNumVertical = 0;
	m_pMtxParent = nullptr;

	SetType(TYPE_MESHSPHERE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMeshCylinder::~CMeshCylinder()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CMeshCylinder::Init(const int nNumAngle,
	const int nNumVertical,
	const Vector3 &pos,
	const Vector3 &rot,
	const float fRadius,
	const float fHeight)
{ // 引数を保存
	m_pos = pos;
	m_rot = rot;
	m_fRadius = fRadius;
	m_fHeight = fHeight;
	m_nNumAngle = nNumAngle;
	m_nNumVertical = nNumVertical;

	VERTEX_3D *pVtx = nullptr;	// 頂点バッファへのポインタ
	WORD *pIdx = nullptr;		// インデックスへのポインタ

	// メッシュの初期化
	CMesh3D::Init((nNumAngle + 1) * (nNumVertical + 1), 
		(nNumAngle * nNumVertical) + nNumVertical,
		(nNumAngle + 1) * (nNumVertical + 1));

	// 生成したメッシュの頂点バッファを取得
	if (SUCCEEDED(CMesh::LockVertex(&pVtx)))
	{ // ロック成功
		for (int nCntVertical = 0; nCntVertical < m_nNumVertical + 1; nCntVertical++)
		{ // 縦の分割数分繰り返し
			float fHeight = (m_fHeight / static_cast<float>(m_nNumVertical + 1)) * ((m_nNumVertical + 1) - nCntVertical);
			for (int nCntAngle = 0; nCntAngle < m_nNumAngle + 1; nCntAngle++)
			{ // 角の数分繰り返し
				Vector3 pos = VECTOR3_NULL;		// 頂点座標
				Vector3 nor = VECTOR3_NULL;		// 法線
				Vector2 tex = VECTOR2_NULL;		// テクスチャ座標
				float fAngle = (DOUBLE_PI / static_cast<float>(m_nNumAngle + 1)) * nCntAngle;
				int nVtx = nCntAngle + (nCntVertical * nCntAngle);		// 現在設定しているインデックス

				// 頂点座標を計算
				pos.x = sinf(fAngle) * (m_fRadius * 0.5f);
				pos.y = fHeight;
				pos.z = cosf(fAngle) * (m_fRadius * 0.5f);
				pVtx[nVtx].pos = pos;

				// 頂点カラーを指定
				pVtx[nVtx].col = COLOR_ONE;

				// 法線を計算
				pVtx[nVtx].nor = Vec3::Direction(pos, m_pos);

				// テクスチャ座標を計算
				pVtx[nVtx].tex.x = (1.0f / static_cast<float>(m_nNumAngle + 1)) * nCntAngle;
				pVtx[nVtx].tex.y = (1.0f * static_cast<float>(nCntVertical));
			}
		}
	}

	// 生成したメッシュのインデックスバッファを取得
	if (SUCCEEDED(CMesh::LockIndex(&pIdx)))
	{ // 取得成功時
		int nIdx1 = 0;					// 一つ目の変数に加算する値
		int nIdx2 = 0;					// 二つ目の変数に加算する値
		int nOffset = m_nNumAngle + 1;	// 変数に足す際にずらすオフセット
		int nCntUnswap = 0;			// スワップせずにインデックスを設定した回数
		bool bSwapping = false;		// スワップフラグ

		for (int nCntHeight = 0; nCntHeight < m_nNumVertical; nCntHeight++)
		{
			for (int nCntWidth = 0; nCntWidth <= nNumAngle; nCntWidth++, pIdx += 2)
			{
				pIdx[0] = (nNumAngle + 1) * (nCntHeight + 1) + nCntWidth;
				pIdx[1] = (nNumAngle + 1) * nCntHeight + nCntWidth;

				if (nCntHeight + 1 < nNumAngle && nCntWidth == nNumAngle)
				{
					pIdx[2] = (nNumAngle + 1) * nCntHeight + nCntWidth;
					pIdx[3] = (nNumAngle + 1) * (nCntHeight + 2);
					pIdx += 2;
				}
			}
		}
	}

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMeshCylinder::Uninit(void)
{ // メッシュの終了
	CMesh3D::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMeshCylinder::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CMeshCylinder::Draw(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイスへのポインタ

	// マトリックスを初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// マトリックスを計算
	Mtx::CalcWorld(&m_mtxWorld,
		m_pMtxParent,
		m_pos,
		m_rot);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// メッシュの描画
	CMesh3D::Draw();
}