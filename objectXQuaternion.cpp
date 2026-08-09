//==================================================================================
// 
// Xモデル(クォータニオン仕様)クラスのソースファイル [objectXQuaternion.h]
// Author : TENMA SAITO
// Date   : 2026/7/3
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectXQuaternion.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "input.h"
#include "vec3math.h"
#include "matrix.h"
#include "util.h"
#include "camera.h"
#include "ray.h"

//==================================================================================
// --- オブジェクト3Dの生成処理 ---
//==================================================================================
CObjectXQuaternion *CObjectXQuaternion::Create(const char *pXFileName,
	const Vector3 &pos, 
	const Vector3 &vecQua, 
	const float fAngle)
{
	CObjectXQuaternion *pObjectXQuaternion = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pObjectXQuaternion = new CObjectXQuaternion;
	if (pObjectXQuaternion == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pObjectXQuaternion->Init(pXFileName, pos, vecQua, fAngle);

	return pObjectXQuaternion;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObjectXQuaternion::CObjectXQuaternion(const int nPriority) : CObject(nPriority)
{ // メンバ変数をクリア
	m_pMesh = nullptr;
	m_pBuffMat = nullptr;
	m_pMtxParent = nullptr;
	m_dwNumMat = 0;
	m_pos = VECTOR3_NULL;
	m_scale = VECTOR3_ONE;
	m_vecQua = VECTOR3_NULL;
	m_fAngle = 0.0f;
	m_vtxMin = VECTOR3_NULL;
	m_vtxMax = VECTOR3_NULL;
	m_bHitByPlayerCamRay = false;
	m_bCalcMatrix = false;
	D3DXMatrixIdentity(&m_mtxWorld);
	D3DXQuaternionIdentity(&m_qua);
	strcpy(m_aFileName, "");

	// タイプの設定
	SetType(TYPE_XMODEL);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObjectXQuaternion::~CObjectXQuaternion()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CObjectXQuaternion::Init(const char* pXFileName, 
	const Vector3 &pos, 
	const Vector3 &vecQua, 
	const float fAngle)
{ // Xファイル読み込み
	LoadXFile(pXFileName);

	// 引数の値を保存
	m_pos = pos;
	m_vecQua = vecQua;
	m_fAngle = fAngle;

	// ファイル名保存
	strcpy_s(m_aFileName, pXFileName);

	// クォータニオンを生成
	D3DXQuaternionRotationAxis(&m_qua,
		&m_vecQua,
		m_fAngle);

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 初期化処理 (クォータニオン指定) ---
//==================================================================================
HRESULT CObjectXQuaternion::Init(const char *pXFileName, 
	const Vector3 &pos,
	const Quaternion &qua)
{ // Xファイル読み込み
	LoadXFile(pXFileName);

	// 引数の値を保存
	m_pos = pos;
	m_qua = qua;

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CObjectXQuaternion::Uninit(void)
{
	if (m_pMesh != nullptr)
	{ // メッシュを解放
		m_pMesh->Release();
		m_pMesh = nullptr;
	}

	if (m_pBuffMat != nullptr)
	{ // マテリアルを解放
		m_pBuffMat->Release();
		m_pBuffMat = nullptr;
	}

	// インデックスをリセット
	m_vIdx.clear();

	// 自分自身を破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CObjectXQuaternion::Update(void)
{
	auto pPlayerCam = CCamera::GetCamera(CCamera::TYPE_PLAYER);
	Vector3 posV = *pPlayerCam->GetPosV();
	Vector3 posLocalV;
	Vector3 ray = pPlayerCam->GetRay();
	Vector3 posLocalRay;
	Vector3 posWorld = GetWorldPosition();
	BOOL bResult = FALSE;
	FLOAT fLength;
	HRESULT hr;
	Matrix mtxInv;

	posWorld.y = posV.y;

	if (CRay(posV, *pPlayerCam->GetPosR()).GetLength() >= Vec3::Length(posV, posWorld))
	{
		// ワールド座標系を自身のローカル座標系に逆変換するマトリックスを求める
		D3DXMatrixInverse(&mtxInv, nullptr, GetMatrix());

		// 逆変換マトリックスで座標をローカル座標へ変換
		D3DXVec3TransformCoord(&posLocalV, &posV, &mtxInv);
		D3DXVec3TransformNormal(&posLocalRay, &ray, &mtxInv);
		ray = Vec3::Normalize(ray);

		hr = D3DXIntersect(m_pMesh,
			&posLocalV,
			&posLocalRay,
			&bResult,
			nullptr,
			nullptr,
			nullptr,
			&fLength,
			nullptr,
			nullptr);
	}

	m_bHitByPlayerCamRay = (bResult) ? true : false;
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CObjectXQuaternion::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL *pMat = nullptr;		// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	if (m_scale == VECTOR3_ONE)
	{ // スケーリングを行わないクォータニオンによるマトリックス計算
		Mtx::CalcWorld(&m_mtxWorld,
			m_pMtxParent,
			m_pos,
			m_qua);
	}
	else
	{ // スケーリング含むクォータニオンによるマトリックス計算
		Mtx::CalcWorld(&m_mtxWorld,
			m_pMtxParent,
			m_scale,
			m_pos,
			m_qua);
	}

	// マトリックスが計算されたためフラグを立てる
	m_bCalcMatrix = true;

	//  ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//  現在のマテリアルを保存
	pDevice->GetMaterial(&matDef);

	// マテリアルを取得
	pMat = static_cast<D3DXMATERIAL*>(m_pBuffMat->GetBufferPointer());

	if (m_bHitByPlayerCamRay)
	{ // αテストを有効にする + Zバッファへの書き込みを無効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 30);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}

	// 各マテリアルを描画
	for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
	{
		D3DMATERIAL9 matSub = pMat[nCntMat].MatD3D;

		if (m_bHitByPlayerCamRay == true)
		{
			matSub.Diffuse.a = 0.3f;
		}

		// マテリアルの設定
		pDevice->SetMaterial(&matSub);

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_vIdx.at(nCntMat)));

		// モデル(パーツ)の描画
		m_pMesh->DrawSubset(nCntMat);
	}

	if (m_bHitByPlayerCamRay)
	{ // αテストを無効にする + Zバッファへの書き込みを有効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==================================================================================
// --- 絶対座標の取得処理 ---
//==================================================================================
Vector3 CObjectXQuaternion::GetWorldPosition(void)
{
	Vector3 posWorld;		// 変換後の位置

	if (m_bCalcMatrix == false)
	{ // もしマトリックスが計算されていなければ、マトリックスを計算
		Mtx::CalcWorld(&m_mtxWorld,
			m_pMtxParent,
			m_scale,
			m_pos,
			m_qua);

		// 計算フラグを立てる
		m_bCalcMatrix = true;
	}

	// 位置を変換
	D3DXVec3TransformCoord(&posWorld, &m_pos, &m_mtxWorld);
	return posWorld;
}

//==================================================================================
// --- クォータニオンの計算処理 ---
//==================================================================================
void CObjectXQuaternion::CaluQuaternion(void)
{
	Quaternion quaMove;				// 計算先

	// クォータニオンを初期化
	D3DXQuaternionIdentity(&quaMove);

	// クォータニオンを設定
	D3DXQuaternionRotationAxis(&quaMove,
		&m_vecQua,
		m_fAngle);

	// クォータニオンを適用
	m_qua = m_qua * quaMove;
}

//==================================================================================
// --- 角度の設定処理 ---
//==================================================================================
void CObjectXQuaternion::SetAngle(const float fAngle)
{ // クォータニオンをリセット
	D3DXQuaternionIdentity(&m_qua);

	// クォータニオンの設定処理
	D3DXQuaternionRotationAxis(&m_qua,
		&m_vecQua,
		fAngle);

	m_fAngle = fAngle;		// 角度の保存
}

//==================================================================================
// --- 角度の追加処理 ---
//==================================================================================
void CObjectXQuaternion::AddAngle(const float fAngle)
{
	Quaternion quaMove;		// クォータニオン

	// クォータニオンの設定処理
	D3DXQuaternionRotationAxis(&quaMove,
		&m_vecQua,
		fAngle);

	m_qua = m_qua * quaMove;		// クォータニオンの掛け算
	m_fAngle = fAngle;				// 角度の保存
}

//==================================================================================
// --- Xファイルの読み込み処理 ---
//==================================================================================
HRESULT	CObjectXQuaternion::LoadXFile(const char* pXFileName)
{
	CManager* pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer* pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	CTexture* pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL* pMat = NULL;		// マテリアルへのポインタ
	HRESULT hr = S_OK;				// モデル読み込み結果
	int nNumVtx = 0;				// 頂点数
	DWORD dwSizeFVF = 0;			// 頂点フォーマットのサイズ
	BYTE* pVtxBuff = NULL;			// 頂点バッファへのポインタ

	// Xファイルの読み込み
	hr = D3DXLoadMeshFromX(pXFileName,			// 読み込むXファイル名
		D3DXMESH_SYSTEMMEM,
		pDevice,						// デバイスポインタ
		NULL,
		&m_pBuffMat,		// マテリアルへのポインタ
		NULL,
		&m_dwNumMat,		// マテリアルの数
		&m_pMesh);		// メッシュへのポインタ
	if (FAILED(hr))
	{ // 読み込み失敗
		return E_FAIL;
	}

	// マテリアルデータへのポインタを取得
	pMat = static_cast<D3DXMATERIAL*>(m_pBuffMat->GetBufferPointer());

	for (int nCntMat = 0; nCntMat < static_cast<int>(m_dwNumMat); nCntMat++)
	{ // マテリアル数分だけテクスチャチェック
		// テクスチャの読み込み
		m_vIdx.push_back(pTexture->Register(pMat[nCntMat].pTextureFilename));
	}

	// 頂点数を取得
	nNumVtx = m_pMesh->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	DWORD dwFvf = m_pMesh->GetFVF();
	dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// 頂点バッファをロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	// 頂点の最大、最小値を取得
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		Vector3 vtx = *(Vector3*)pVtxBuff;	// 頂点座標の代入

		// 最小値を取得
		m_vtxMin.x = (m_vtxMin.x > vtx.x) ? vtx.x : m_vtxMin.x;
		m_vtxMin.y = (m_vtxMin.y > vtx.y) ? vtx.y : m_vtxMin.y;
		m_vtxMin.z = (m_vtxMin.z > vtx.z) ? vtx.z : m_vtxMin.z;

		// 最大値を取得
		m_vtxMax.x = (m_vtxMax.x < vtx.x) ? vtx.x : m_vtxMax.x;
		m_vtxMax.y = (m_vtxMax.y < vtx.y) ? vtx.y : m_vtxMax.y;
		m_vtxMax.z = (m_vtxMax.z < vtx.z) ? vtx.z : m_vtxMax.z;

		pVtxBuff += dwSizeFVF;		// 頂点フォーマットのサイズ分ポインタを進める
	}

	/*** 頂点バッファをアンロック ***/
	m_pMesh->UnlockVertexBuffer();

	// 読み込み結果を返す
	return S_OK;
}