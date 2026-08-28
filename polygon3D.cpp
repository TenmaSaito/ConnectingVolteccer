//==================================================================================
// 
// ポリゴン3Dクラスのヘッダーファイル [poplygon3D.h]
// Author : TENMA SAITO
// Date   : 2026/8/28
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "polygon3D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "matrix.h"
#include "effect.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPolygon3D *CPolygon3D::Create(const Vector3 &pos, const Vector3 &rot, const Vector2 &size)
{
	CPolygon3D *pPoly = new CPolygon3D;		// 生成したポリゴン
	NULLPOINTER_ASSERT(pPoly);

	if (pPoly != nullptr)
	{ // 初期化処理
		pPoly->Init(pos, rot, size);
	}

	return pPoly;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPolygon3D::CPolygon3D()
{ // メンバ変数のクリア
	m_pVtxBuff = nullptr;
	m_pMtxParent = nullptr;
	m_nIdxTexture = -1;
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_size = VECTOR2_NULL;
	m_col = COLOR_NULL;
	m_bDisp = true;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPolygon3D::~CPolygon3D()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPolygon3D::Init(const Vector3 &pos, const Vector3 &rot, const Vector2 &size)
{
	VERTEX_3D *pVtx = NULL;		// 頂点情報へのポインタ
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();	// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスへのポインタ

	// 引数を保存
	m_pos = pos;
	m_rot = rot;
	D3DXQuaternionRotationYawPitchRoll(&m_qua, rot.y, rot.x, rot.z);
	m_size = size;
	m_col = COLOR_ONE;

	// 頂点バッファ作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * DEFAULT_VERTEX_NUM,
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
	pVtx[0].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = Color(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = Color(1.0f, 1.0f, 1.0f, 1.0f);

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
void CPolygon3D::Uninit(void)
{ // バッファ解放
	SafeRelease(m_pVtxBuff);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPolygon3D::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPolygon3D::Draw(void)
{ // 描画フラグが降りていた時、スキップ
	if (m_bDisp != true) return;

	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	Matrix mtxView;		// ビューマトリックス

	// マトリックスの初期化
	Mtx::Identity(&m_mtxWorld);

	// マトリックスの計算
	Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_pos, m_qua);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// 描画前関数呼び出し
	if (m_beforeDraw) m_beforeDraw(pDevice);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);

	// 描画後関数呼び出し
	if (m_afterDraw) m_afterDraw(pDevice);
}

//==================================================================================
// --- 角度の設定処理 (オイラー角) ---
//==================================================================================
void CPolygon3D::SetRotation(const Vector3 &rot)
{ // 引数を保存
	m_rot = rot;

	// クォータニオンを再計算
	D3DXQuaternionRotationYawPitchRoll(&m_qua, rot.y, rot.x, rot.z);
}

//==================================================================================
// --- 角度の設定処理 (四元数) ---
//==================================================================================
void CPolygon3D::SetQuaternion(const Quaternion &qua)
{ // 引数を保存
	m_qua = qua;

	// 角度を再計算
	Matrix mtxNormal;						// 角度計算用マトリックス
	Vector3 rot = VECTOR3_NULL;				// 初期値

	// マトリックスから角度を求める
	Mtx::Identity(&mtxNormal);
	Mtx::CalcRotation(&mtxNormal, m_qua);
	D3DXVec3TransformNormal(&m_rot, &rot, &mtxNormal);
}

//==================================================================================
// --- 角度の設定処理 (任意軸 + 回転度数) ---
//==================================================================================
void CPolygon3D::SetQuaternionRotationAxis(const Vector3 &vec, const float fAngle)
{ // クォータニオンを求める
	D3DXQuaternionRotationAxis(&m_qua, &vec, fAngle);

	// 角度を再計算
	Matrix mtxNormal;						// 角度計算用マトリックス
	Vector3 rot = VECTOR3_NULL;				// 初期値

	// マトリックスから角度を求める
	Mtx::Identity(&mtxNormal);
	Mtx::CalcRotation(&mtxNormal, m_qua);
	D3DXVec3TransformNormal(&m_rot, &rot, &mtxNormal);
}

//==================================================================================
// --- 任意軸 + 回転度数の取得処理 ---
//==================================================================================
std::pair<Vector3, float> CPolygon3D::GetQuaternionRotationAxis(void) const
{
	std::pair<Vector3, float> rotAxis;		// 任意軸と回転度数

	// それぞれを求める
	D3DXQuaternionToAxisAngle(&m_qua,
		&rotAxis.first,
		&rotAxis.second);

	return rotAxis;
}

//==================================================================================
// --- サイズの設定処理 ---
//==================================================================================
void CPolygon3D::SetSize(const Vector2 &size)
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

//==================================================================================
// --- 色の変更処理 ---
//==================================================================================
void CPolygon3D::SetColor(const Color &color)
{
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 引数を保存
	m_col = color;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void **)&pVtx, 0);

	// 頂点カラー設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}