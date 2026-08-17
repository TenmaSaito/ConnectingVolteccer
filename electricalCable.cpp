//==================================================================================
// 
// 電線クラスのヘッダーファイル [electricalCable.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/9
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "electricalCable.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "texture.h"
#include "utilityPole.h"
#include "vec2math.h"
#include "vec3math.h"
#include "matrix.h"
#include "effect.h"
#include "planet.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define CABLE_HEIGHT		(10.0f)		// ケーブルの太さ

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CElectricalCable *CElectricalCable::Create(const CUtilityPole *pStart,
	const CUtilityPole *pEnd)
{
	CElectricalCable *pCable = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pCable = new CElectricalCable;
	if (pCable == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pCable->Init(pStart, pEnd);

	return pCable;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CElectricalCable::CElectricalCable()
{ // メンバ変数をクリア
	m_pVtxBuff = nullptr;
	m_nIdxTexture = -1;
	m_pStart = nullptr;
	m_pEnd = nullptr;
	m_pMtxParent = nullptr;
	m_pos = VECTOR3_NULL;
#ifndef ENABLE_QUATERNION
	m_rot = VECTOR3_NULL;
#else
	m_vecQua = VECTOR3_NULL;
	m_fAngle = 0.0f;
	D3DXQuaternionIdentity(&m_qua);
#endif

	// タイプの指定
	CObject::SetType(TYPE_CABLE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CElectricalCable::~CElectricalCable()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CElectricalCable::Init(const CUtilityPole *pStart, 
	const CUtilityPole *pEnd)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイスへのポインタ
	Vector3 posStart = VECTOR3_NULL;		// 始点の座標
	Vector3 posStart2 = VECTOR3_NULL;		// 始点の座標2
	Vector3 posEnd = VECTOR3_NULL;			// 終点の座標
	Vector3 posEnd2 = VECTOR3_NULL;			// 終点の座標2
	float fLength;			// 電柱間の距離
	HRESULT hr = S_OK;		// 初期化結果
	VERTEX_3D *pVtx;		// 頂点へのポインタ

	// 二点の位置をマトリックスから求める
	posStart.y = pStart->GetVtxMax()->y;
	posStart2.y = pStart->GetVtxMax()->y + 10.0f;
	posEnd.y = pEnd->GetVtxMax()->y;
	posEnd2.y = pEnd->GetVtxMax()->y + 10.0f;

	D3DXVec3TransformCoord(&posStart, &posStart, pStart->GetMatrix());
	D3DXVec3TransformCoord(&posStart2, &posStart2, pStart->GetMatrix());
	D3DXVec3TransformCoord(&posEnd, &posEnd, pEnd->GetMatrix());
	D3DXVec3TransformCoord(&posEnd2, &posEnd2, pEnd->GetMatrix());

	// 二点間の距離を求める
	fLength = Vec3::Length(posEnd, posStart);

	// 頂点バッファの作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標を設定
	pVtx[0].pos = posStart;
	pVtx[1].pos = posStart2;
	pVtx[2].pos = posEnd;
	pVtx[3].pos = posEnd2;

	// 頂点の法線を設定
	pVtx[0].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = Vector3(0.0f, 0.0f, -1.0f);

	// 頂点カラー設定
	pVtx[0].col = Color(0.0f, 1.0f, 0.0f, 1.0f);
	pVtx[1].col = Color(0.0f, 1.0f, 0.0f, 1.0f);
	pVtx[2].col = Color(0.0f, 1.0f, 0.0f, 1.0f);
	pVtx[3].col = Color(0.0f, 1.0f, 0.0f, 1.0f);

	// テクスチャ座標設定
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// ロック解除
	m_pVtxBuff->Unlock();

	// 現在の惑星のクォータニオンから任意軸と角度を求める
	const Quaternion *pQuaPlanet = pManager->GetScene<CGame>()->GetPlanet()->GetQuaternion();
	D3DXQuaternionToAxisAngle(pQuaPlanet,
		&m_vecQua,
		&m_fAngle);

	// 取得した情報からクォータニオンを生成
	D3DXQuaternionRotationAxis(&m_qua,
		&m_vecQua,
		-m_fAngle);

	// 初期化結果を返す
	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CElectricalCable::Uninit(void)
{
	if (m_pVtxBuff != nullptr)
	{ // 頂点バッファを破棄
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// オブジェクトの破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CElectricalCable::Update(void)
{
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CElectricalCable::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの設定
#ifndef ENABLE_QUATERNION
	Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_pos, m_rot);
#else
	Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_pos, m_qua);
#endif
	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// カリングとライティングを無効に設定
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);

	// カリングとライティングを有効に設定
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}