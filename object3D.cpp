//==================================================================================
// 
// オブジェクト3Dクラスのソースファイル [object3D.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object3D.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "player.h"
#include "matrix.h"
#include "vec3math.h"

//==================================================================================
// --- オブジェクト3Dの生成処理 ---
//==================================================================================
CObject3D *CObject3D::Create(const bool bXYPlane, 
	const Vector3 &pos,
	const Vector3 &rot, 
	const Vector2 &size)
{
	CObject3D *pObject3D = nullptr;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pObject3D = new CObject3D;
	if (pObject3D == nullptr)
	{ // 生成に失敗した場合、NULLを返す
		return nullptr;
	}

	// 初期化処理
	pObject3D->Init(bXYPlane, pos, rot, size);

	return pObject3D;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObject3D::CObject3D(const int nPriority) : CObject(nPriority)
{ // メンバ変数をクリア
	m_pVtxBuff = nullptr;
	m_nIdxTexture = -1;
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_size = VECTOR2_NULL;
	ZeroMemory(m_aVtx, sizeof(m_aVtx));
	m_bDirty = false;
	m_bXYPlane = false;

	// タイプを指定
	SetType(TYPE_OBJ_3D);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObject3D::~CObject3D()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CObject3D::Init(const bool bXYPlane, 
	const Vector3 &pos, 
	const Vector3 &rot,
	const Vector2 &size)
{
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	HRESULT hr;						// テクスチャ読み込みの判定
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 頂点バッファ作成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr))
	{ // 頂点バッファの生成に失敗した場合、エラーを返す
		return hr;
	}

	// 引数の保存
	m_pos = pos;
	m_rot = rot;
	m_size = size;
	m_bXYPlane = bXYPlane;

	// 位置及び角度が変更されたためマトリックスの計算フラグを立てる
	m_bDirty = true;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = m_pos.x - (m_size.x * 0.5f);
	pVtx[1].pos.x = m_pos.x - (m_size.x * 0.5f);
	pVtx[2].pos.x = m_pos.x + (m_size.x * 0.5f);
	pVtx[3].pos.x = m_pos.x + (m_size.x * 0.5f);
	
	if (bXYPlane == true)
	{ // XY平面の作成ならY値に設定
		pVtx[0].pos.y = m_pos.y - (m_size.y * 0.5f);
		pVtx[1].pos.y = m_pos.y + (m_size.y * 0.5f);
		pVtx[2].pos.y = m_pos.y - (m_size.y * 0.5f);
		pVtx[3].pos.y = m_pos.y + (m_size.y * 0.5f);

		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.z = 0.0f;

		// 座標変換用変数設定
		pVtx[0].nor = Vector3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = Vector3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = Vector3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = Vector3(0.0f, 0.0f, -1.0f);
	}
	else
	{ // XZ平面の作成ならZ値に設定
		pVtx[0].pos.y = 0.0f;
		pVtx[1].pos.y = 0.0f;
		pVtx[2].pos.y = 0.0f;
		pVtx[3].pos.y = 0.0f;

		pVtx[0].pos.z = m_pos.z - (m_size.y * 0.5f);
		pVtx[1].pos.z = m_pos.z + (m_size.y * 0.5f);
		pVtx[2].pos.z = m_pos.z - (m_size.y * 0.5f);
		pVtx[3].pos.z = m_pos.z + (m_size.y * 0.5f);

		Vector3 norA, norB;		// 0,3番目の頂点の法線
		Vector3 norAB;			// 1,2番目の頂点の法線
		Vector3 aVec[4];		// 各頂点の境界線ベクトル

		// 各境界線ベクトルを求める
		// 0 -> 1 & 0 -> 2
		aVec[0] = pVtx[1].pos - pVtx[0].pos;
		aVec[1] = pVtx[2].pos - pVtx[0].pos;

		// 3 -> 1 & 3 -> 2
		aVec[2] = pVtx[1].pos - pVtx[3].pos;
		aVec[3] = pVtx[2].pos - pVtx[3].pos;

		// 法線を各ベクトルから求める
		D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
		D3DXVec3Cross(&norB, &aVec[3], &aVec[2]);

		// 法線を正規化
		D3DXVec3Normalize(&norA, &norA);
		D3DXVec3Normalize(&norB, &norB);

		// 2つの法線から1,2番目の頂点の法線を求める
		norAB = (norA + norB) * 0.5f;
		D3DXVec3Normalize(&norAB, &norAB);

		// 座標変換用変数設定
		pVtx[0].nor = norA;
		pVtx[1].nor = norAB;
		pVtx[2].nor = norAB;
		pVtx[3].nor = norB;

		// 頂点座標を保存
		m_aVtx[0] = pVtx[0].pos;
		m_aVtx[1] = pVtx[1].pos;
		m_aVtx[2] = pVtx[2].pos;
		m_aVtx[3] = pVtx[3].pos;
	}

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

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CObject3D::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff)
	{ // 確保されていれば解放する
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CObject3D::Update(void)
{

}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CObject3D::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ

	if (m_bDirty == true)
	{ // 位置や角度の変更によりマトリックスの再計算が必要な場合
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_mtxWorld);

		// ワールドマトリックスの計算
		Mtx::CalcWorld(&m_mtxWorld, m_pos, m_rot);

		// フラグをおろす
		m_bDirty = false;
	}

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// ライティングを無効に設定
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);

	// ライティングを有効に設定
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//==================================================================================
// --- 位置の変更処理 ---
//==================================================================================
void CObject3D::SetPosition(const Vector3 &position)
{ // 位置の変更及び変更フラグを立てる
	m_pos = position;
	m_bDirty = true;
}

//==================================================================================
// --- 角度の変更処理 ---
//==================================================================================
void CObject3D::SetRotation(const Vector3 &rotation)
{ // 角度の変更及び変更フラグを立てる
	m_rot = rotation;
	m_bDirty = true;
}

//==================================================================================
// --- サイズの変更処理 ---
//==================================================================================
void CObject3D::SetSize(const Vector2& size)
{ // サイズの変更及び変更フラグを立てる
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = (m_size.x * 0.5f);
	pVtx[1].pos.x = (m_size.x * 0.5f);
	pVtx[2].pos.x = (m_size.x * 0.5f);
	pVtx[3].pos.x = (m_size.x * 0.5f);
	
	if (m_bXYPlane == true)
	{ // XY平面の作成ならY値に設定
		pVtx[0].pos.y = (m_size.y * 0.5f);
		pVtx[1].pos.y = (m_size.y * 0.5f);
		pVtx[2].pos.y = (m_size.y * 0.5f);
		pVtx[3].pos.y = (m_size.y * 0.5f);

		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.z = 0.0f;
	}
	else
	{ // XZ平面の作成ならZ値に設定
		pVtx[0].pos.y = 0.0f;
		pVtx[1].pos.y = 0.0f;
		pVtx[2].pos.y = 0.0f;
		pVtx[3].pos.y = 0.0f;

		pVtx[0].pos.z = (m_size.y * 0.5f);
		pVtx[1].pos.z = (m_size.y * 0.5f);
		pVtx[2].pos.z = (m_size.y * 0.5f);
		pVtx[3].pos.z = (m_size.y * 0.5f);
	}

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	m_bDirty = true;
}

//==================================================================================
// --- 高さ取得処理 ---
//==================================================================================
float CObject3D::GetHeight(const Vector3 &pos)
{
	Vector3 aVtx[4];			// 各頂点の座標
	float fHeight = 0.0f;	// 計算結果

	for (int nCntVtx = 0; nCntVtx < 4; nCntVtx++)
	{ // 各頂点のマトリックス適用版を求める
		D3DXVec3TransformCoord(&aVtx[nCntVtx], &m_aVtx[nCntVtx], &m_mtxWorld);
	}

	if (Vec3::IsInsideTriangle(pos, &aVtx[0]))
	{ // もし三角形の内側にいたら
		Vector3 vec1, vec2;		// 各頂点間のベクトル
		Vector3 nor;			// 法線ベクトル

		// 各ベクトルを求める
		vec1 = aVtx[1] - aVtx[0];
		vec2 = aVtx[2] - aVtx[0];

		// 法線を計算
		D3DXVec3Cross(&nor, &vec1, &vec2);
		D3DXVec3Normalize(&nor, &nor);

		// 高さを求める
		fHeight = Vec3::Height(pos, aVtx[0], nor);
	}
	else if (Vec3::IsInsideTriangle(pos, &aVtx[1], true))
	{ // もし三角形の内側にいたら
		Vector3 vec1, vec2;		// 各頂点間のベクトル
		Vector3 nor;			// 法線ベクトル

		// 各ベクトルを求める
		vec1 = aVtx[1] - aVtx[3];
		vec2 = aVtx[2] - aVtx[3];

		// 法線を計算
		D3DXVec3Cross(&nor, &vec2, &vec1);
		D3DXVec3Normalize(&nor, &nor);

		// 高さを求める
		fHeight = Vec3::Height(pos, aVtx[3], nor);
	}

	return fHeight;
}