//==================================================================================
// 
// 雷エフェクトクラスのヘッダーファイル [thunderEffect.h]
// Author : TENMA SAITO
// Date   : 2026/6/8
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "sparkEffect.h"
#include "meshOrbit3D.h"
#include "rand.h"
#include "matrix.h"
#include "manager.h"
#include "debugproc.h"
#include "texture.h"
#include "color.h"
#include "vec3math.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define EFFECT_GRADATION_PATH		"data/TEXTURE/gradation102.jpg"		// 雷エフェクトのテクスチャ

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CSparkEffect *CSparkEffect::Create(const Vector3 &pos,
	const Vector3 &rot,
	const float fSpeed,
	const int nLife,
	const Vector3 &min,
	const Vector3 &max)
{
	CSparkEffect *pSpark = NULL;		// 生成したオブジェクトへのポインタ

	// オブジェクトの生成
	pSpark = new CSparkEffect;
	if (pSpark == NULL)
	{ // 生成に失敗した場合、NULLを返す
		return NULL;
	}

	// 初期化処理
	pSpark->Init(pos, rot, fSpeed, nLife, min, max);

	return pSpark;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CSparkEffect::CSparkEffect()
{ // 各メンバ変数をクリア
	m_pos = VECTOR3_NULL;
	m_posParent = VECTOR3_NULL;
	m_rotParent = VECTOR3_NULL;
	m_min = VECTOR3_NULL;
	m_max = VECTOR3_NULL;
	m_fMinWidth = 0.0f;
	m_fMaxWidth = 0.0f;
	m_fSpeed = 0.0f;
	m_nLife = 0;
	m_pOrbit = nullptr;

	// タイプの指定
	CObject::SetType(TYPE_SPARK);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CSparkEffect::~CSparkEffect()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CSparkEffect::Init(const Vector3 &pos,
	const Vector3 &rot,
	const float fSpeed,
	const int nLife,
	const Vector3 &min,
	const Vector3 &max)
{
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ
	D3DXCOLOR col = Color::GetColor(Color::COLOR_YELLOW);	// 色
	VERTEX_3D *pVtx = nullptr;		// 頂点へのポインタ
	Matrix mtxParentTemp;		// 都度生成される疑似的な親マトリックス

	// オービットの最大最小値を設定
	m_fMinWidth = 0.5f;
	m_fMaxWidth = 10.0f;

	// 引数を保存
	m_min = min;
	m_max = max;
	m_rotParent = rot;
	m_posParent = pos;
	m_fSpeed = fSpeed;
	m_nLife = nLife;

	// マトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);
	D3DXMatrixIdentity(&mtxParentTemp);

	// 親マトリックスの計算
	Mtx::CalcWorld(&mtxParentTemp,
		m_posParent,
		m_rotParent);

	// マトリックスの計算
	Mtx::CalcWorld(&m_mtxWorld,
		m_pos,
		VECTOR3_NULL);

	// 親マトリックスを適用
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParentTemp);

	// オービットを生成
	m_pOrbit = CMeshOrbit3D::Create(&m_mtxWorld, Vector3(10.0f, 0.0f, 0.0f), Vector3(-10.0f, 0.0f, 0.0f));
	if (m_pOrbit == nullptr)
	{ // 生成失敗
		return E_FAIL;
	}

	// 色を指定
	if (SUCCEEDED(m_pOrbit->LockVertex(&pVtx)))
	{ // ロック成功時
		for (int nCntVtx = 0; nCntVtx < m_pOrbit->GetVertexNum(); nCntVtx++)
		{ // 全頂点の色を変更
			pVtx[nCntVtx].col = col;
		}
	}

	// テクスチャを指定
	m_pOrbit->BindTexture(pTexture->Register(EFFECT_GRADATION_PATH));

	// 成功
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CSparkEffect::Uninit(void)
{
	if (m_pOrbit != nullptr)
	{ // オービットの破棄 + 終了処理
		m_pOrbit->Uninit();
		m_pOrbit = nullptr;
	}

	// オブジェクトの破棄
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CSparkEffect::Update(void)
{
	CDebugProc* pProc = CManager::GetInstance()->GetDebugProc();		// デバッグ表示へのポインタ
	CRand* pRand = CRand::GetInstance();							// 乱数生成機へのポインタ
	float fWidth = pRand->Generate(m_fMinWidth, m_fMaxWidth);		// オービットの幅

	// 親の座標を更新
	m_posParent += Vec3::Direction(m_rotParent + Vector3(1.0f, 0.0f, 0.0f)) * m_fSpeed;

	// 座標を更新
	m_pos.x = CRand::GetInstance()->Generate(m_min.x, m_max.x) * 2;

	// オフセットを更新
	m_pOrbit->SetOffset(Vector3(fWidth * 0.5f, 0.0f, 0.0f), Vector3(-fWidth * 0.5f, 0.0f, 0.0f));

	// 寿命を減少させる
	m_nLife--;
	if (m_nLife <= 0)
	{ // 寿命が尽きたとき、消滅
		Uninit();
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CSparkEffect::Draw(void)
{
	Matrix mtxParentTemp;		// 都度生成される疑似的な親マトリックス

	// マトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);
	D3DXMatrixIdentity(&mtxParentTemp);

	// 親マトリックスの計算
	Mtx::CalcWorld(&mtxParentTemp,
		m_posParent,
		m_rotParent);

	// マトリックスの計算
	Mtx::CalcWorld(&m_mtxWorld,
		m_pos,
		VECTOR3_NULL);

	// 親マトリックスを適用
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParentTemp);
}