//==================================================================================
// 
// 惑星クラスのヘッダーファイル [planet.h]
// Author : TENMA SAITO
// Date   : 2026/6/24
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "planet.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "player.h"
#include "xfile.h"
#include "matrix.h"
#include "input.h"
#include "vec3math.h"
#include "vec2math.h"
#include "util.h"
#include "effect.h"
#include "debugproc.h"
#include "playerCamera.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define RESIST_PLAYERSPEED		(0.001f)		// プレイヤーの移動量に掛ける減少係数

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CPlanet *CPlanet::Create(void)
{
	CPlanet *pPlanet = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pPlanet = new CPlanet;
	if (pPlanet == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pPlanet->Init();

	return pPlanet;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPlanet::CPlanet()
{ // メンバ変数のクリア
	m_pos = VECTOR3_NULL;
	m_vecQua = VECTOR3_NULL;
	m_vecQuaDest = VECTOR3_NULL;
	D3DXQuaternionIdentity(&m_qua);
	m_fAngle = 0.0f;
	m_nIdxModel = -1;
	m_pFocusLasso = nullptr;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPlanet::~CPlanet()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CPlanet::Init(void)
{
	// モデル読み込み
	m_nIdxModel = CXFile::GetInstance()->Resister("data/MODEL/sphere000.x");

	// 任意軸設定
	m_vecQua = Vec3::Direction(VECTOR3_NULL, D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CPlanet::Uninit(void)
{ // オブジェクトの破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CPlanet::Update(void)
{
	CManager *pManager = CManager::GetInstance();					// マネージャーへのポインタ
	CInputKeyboard *pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	CPlayer *pPlayer = pManager->GetScene<CGame>()->GetPlayer();	// プレイヤーへのポインタ
	D3DXQUATERNION quaMove;				// 計算先
	float fVecRot = 0.0f;				// 回転時の計算用変数
	float fSpeed = 0.0f;				// 回転速度

	if (m_pFocusLasso == nullptr)
	{ // プレイヤーフォーカス
		// プレイヤーの角度を取得
		fVecRot = pPlayer->GetRotationDest()->y;

		// プレイヤーの移動量を速度に変換
		fSpeed = Vec3::Length(*pPlayer->GetMove()) * RESIST_PLAYERSPEED;

		// 90°右に回す
		fVecRot += HALF_PI;

		// ベクトルを求める
		m_vecQuaOld = m_vecQua;
		m_vecQua = Vec2::ToVector3(Vec2::Direction(Util::FixedRotation(fVecRot)));
		m_vecQua.z = m_vecQua.y;
		m_vecQua.y = 0.0f;

		// クォータニオンを初期化
		D3DXQuaternionIdentity(&quaMove);

		// クォータニオンを設定
		D3DXQuaternionRotationAxis(&quaMove,
			&m_vecQua,
			fSpeed);

		// クォータニオンを適用
		m_qua = m_qua * quaMove;
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CPlanet::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	CXFile *pXFile = CXFile::GetInstance();					// Xモデルへのポインタ
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL *pMat = nullptr;		// マテリアルデータへのポインタ
	CXFile::XDATA *pXdata = nullptr;	// モデルデータへのポインタ

	// マトリックスを初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// クォータニオンによる角度計算
	D3DXMatrixRotationQuaternion(&m_mtxWorld, &m_qua);

	// 位置計算
	Mtx::CalcPosition(&m_mtxWorld, m_pos);

	//  ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//  現在のマテリアルを保存
	pDevice->GetMaterial(&matDef);

	if (pXFile->GetAddress(m_nIdxModel, &pXdata))
	{ // モデルデータ取得成功時
		// マテリアルを取得
		pMat = static_cast<D3DXMATERIAL*>(pXdata->pBuffMat->GetBufferPointer());

		// 各マテリアルを描画
		for (int nCntMat = 0; nCntMat < static_cast<int>(pXdata->dwNumMat); nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pTexture->GetAddress(pXdata->pIdx[nCntMat]));

			// モデル(パーツ)の描画
			pXdata->pMesh->DrawSubset(nCntMat);
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==================================================================================
// --- 回転のフォーカス変更処理 ---
//==================================================================================
void CPlanet::SetFocusLasso(const CLasso *pLasso)
{
	m_pFocusLasso = pLasso;
}