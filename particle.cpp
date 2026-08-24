//==================================================================================
// 
// パーティクル(クォータニオン仕様)クラスのソースファイル [particle.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/24
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "particle.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "matrix.h"
#include "vec3math.h"
#include "rand.h"
#include "objectBillboard.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CParticleQuaternion *CParticleQuaternion::Create(const Vector3 &pos,
	const Vector2 &baseScale,
	const Color &col,
	const Vector3 &vecQua,
	const float fAngle,
	const int nNumEffect,
	const int nLife)
{
	CParticleQuaternion *pParticleQuaternion = new CParticleQuaternion;		// 生成したパーティクルへのポインタ
	if (pParticleQuaternion != nullptr)
	{ // 生成出来ていれば、初期化処理
		pParticleQuaternion->Init(pos,
			baseScale,
			col,
			vecQua,
			fAngle,
			nNumEffect,
			nLife);
	}

	return pParticleQuaternion;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CParticleQuaternion::CParticleQuaternion(const int nPriority) : CObject(nPriority)
{ // タイプを指定
	SetType(TYPE_PARTICLE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CParticleQuaternion::~CParticleQuaternion()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CParticleQuaternion::Init(const Vector3 &pos,
	const Vector2 &baseScale,
	const Color &col,
	const Vector3 &vecQua,
	const float fAngle,
	const int nNumEffect,
	const int nLife)
{ // 引数の値を保存
	m_pos = pos;
	m_baseScale = baseScale;
	m_col = col;
	m_vecQua = vecQua;
	m_fAngle = fAngle;
	m_nLife = nLife;
	m_nNumEffect = nNumEffect;

	// クォータニオンを生成
	D3DXQuaternionRotationAxis(&m_qua,
		&m_vecQua,
		m_fAngle);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CParticleQuaternion::Uninit(void)
{ // オブジェクト解放
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CParticleQuaternion::Update(void)
{
	CRand *pRand = CRand::GetInstance();		// 乱数生成インスタンス

	for (int nCntEffect = 0; nCntEffect < m_nNumEffect; nCntEffect++)
	{ // エフェクト生成
		if (m_nLife < 3) continue;
		CObjectBillboard *pBill = CObjectBillboard::Create(m_pos,
			Vec3::Random() * pRand->Generate(0.1f, 5.0f),
			m_baseScale * pRand->Generate(0.5f, 1.5f),
			pRand->Generate(1, m_nLife - 1));

		// 色を指定
		pBill->SetColor(m_col);

		// 親マトリックスを設定
		pBill->SetParent(&m_mtxWorld); 

		// テクスチャインデックスを設定
		pBill->BindTexture(m_nIdxTexture);

		// 自動スケール減少をオンに設定
		pBill->SetEnableScaleDown(true);

		// 描画前処理のラムダ式を登録
		pBill->SetStateFunctionBeforeDraw([](LPDIRECT3DDEVICE9 pDevice) 
			{ // ライティングを無効に設定
				pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

				// Zテストを無効にする
				pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

				// 加算合成開始
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			});

		// 描画後処理のラムダ式を登録
		pBill->SetStateFunctionAfterDraw([](LPDIRECT3DDEVICE9 pDevice)
			{ // 加算合成終了
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				// Zテストを無効にする
				pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
				pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

				// ライティングを有効に設定
				pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
			});
	}

	m_nLife--;		// 寿命を減らす
	if (m_nLife <= 0)
	{ // 寿命が尽きた場合、終了
		Uninit();
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CParticleQuaternion::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxTrans;		// 計算用マトリックス
	D3DXMATRIX mtxView;			// ビューマトリックスの取得用

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

	if (m_pMtxParent != nullptr)
	{ // 親が存在する場合、マトリックスを掛け合わせる
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, m_pMtxParent);
	}
}