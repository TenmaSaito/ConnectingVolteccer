//==================================================================================
// 
// オブジェクトビルボードクラスのソースファイル [objectBillboard.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectBillboard.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "matrix.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CObjectBillboard *CObjectBillboard::Create(const Vector3 &pos,
	const Vector3 &move,
	const Vector2 &size,
	const int nLife)
{
	CObjectBillboard *pObject3D = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pObject3D = new CObjectBillboard;
	if (pObject3D == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pObject3D->Init(pos, move, size, nLife);

	return pObject3D;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObjectBillboard::CObjectBillboard(const int nPriority) : CObject(nPriority)
{ // メンバ変数をクリア
	m_pVtxBuff = nullptr;
	m_nIdxTexture = INVALID_TEXID;
	m_pos = VECTOR3_NULL;
	m_move = VECTOR3_NULL;
	m_size = VECTOR2_NULL;
	m_col = COLOR_NULL;
	m_pMtxParent = nullptr;
	m_bDisp = true;
	m_bAlpha = true;
	m_bScaleDown = false;
	m_fScaleFrame = 0.0f;
	m_nLife = 0;

	// タイプを指定
	SetType(TYPE_OBJ_BILLBOARD);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObjectBillboard::~CObjectBillboard()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CObjectBillboard::Init(const Vector3 &pos,
	const Vector3 &move,
	const Vector2 &size, 
	const int nLife)
{
	CRenderer *pRenderer = CManager::GetInstance()->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	HRESULT hr;					// テクスチャ読み込みの判定
	VERTEX_3D *pVtx = NULL;		// 頂点情報へのポインタ

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
	m_move = move;
	m_size = size;
	m_nLife = nLife;
	m_col = COLOR_ONE;

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
	pVtx[0].col = COLOR_ONE;
	pVtx[1].col = COLOR_ONE;
	pVtx[2].col = COLOR_ONE;
	pVtx[3].col = COLOR_ONE;

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
void CObjectBillboard::Uninit(void)
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
void CObjectBillboard::Update(void)
{
	if (m_bScaleDown == true)
	{ // 自動スケール減少ありの場合
		SetSize(Vector2(m_size.x - m_fScaleFrame, m_size.y - m_fScaleFrame));
	}

	m_pos += m_move;	// 位置更新

	m_nLife--;		// 寿命を減らす
	if (m_nLife <= 0)
	{ // 寿命が尽きた場合、終了
		Uninit();
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CObjectBillboard::Draw(void)
{ // 描画フラグが立っていなければスキップ
	if (m_bDisp == false) return;

	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	Matrix mtxView;		// ビューマトリックス

	// カメラのビューマトリックスを取得 
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// マトリックスの逆行列を求める
	D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);

	// 逆行列によって入ってしまった位置情報を初期化
	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;

	// 位置の計算
	Mtx::CalcPosition(&m_mtxWorld, m_pos);

	if (m_pMtxParent != nullptr)
	{ // 親が存在するならマトリックスを掛け合わせる
		Matrix mtxParentUnRotate;	// 回転を考慮しない親マトリックス

		// マトリックスを初期化
		D3DXMatrixIdentity(&mtxParentUnRotate);

		// 移動要素のみ取り出す
		mtxParentUnRotate._41 = m_pMtxParent->_41;
		mtxParentUnRotate._42 = m_pMtxParent->_42;
		mtxParentUnRotate._43 = m_pMtxParent->_43;

		// マトリックスを掛け合わせる
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParentUnRotate);
	}

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	if (m_bAlpha)
	{ // αテストを有効にする
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 30);
	}

	// 描画前関数呼び出し
	if (m_beforeDraw) m_beforeDraw(pDevice);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);

	// 描画後関数呼び出し
	if (m_afterDraw) m_afterDraw(pDevice);

	if (m_bAlpha)
	{ // αテストを無効にする
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	}
}

//==================================================================================
// --- サイズの変更処理 ---
//==================================================================================
void CObjectBillboard::SetSize(const Vector2 &size)
{ // サイズの変更及び変更フラグを立てる
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 引数を保存
	m_size = size;

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

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 色の変更処理 ---
//==================================================================================
void CObjectBillboard::SetColor(const Color &color)
{
	VERTEX_3D *pVtx = nullptr;		// 頂点情報へのポインタ

	// 引数を保存
	m_col = color;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラー設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================
// --- 自動スケール減少のフラグ指定処理 ---
//==================================================================================
void CObjectBillboard::SetEnableScaleDown(const bool bEnable)
{ // 引数を保存
	m_bScaleDown = bEnable;
	if (m_bScaleDown == true)
	{ // フラグが立った場合、残りフレーム数から減少係数を計算
		m_fScaleFrame = 1.0f / static_cast<float>(m_nLife);
	}
	else
	{ // フラグが下りた場合、無し
		m_fScaleFrame = 0.0f;
	}
}