//==================================================================================
// 
// 草クラスのソースファイル [grass.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/16
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "grass.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "rand.h"
#include "debugproc.h"
#include "input.h"
#include "vec3math.h"
#include "matrix.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define OX_EPSILON		(0.00001f)		// 浮動小数による判定誤差防止

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CGrass *CGrass::Create(const Vector3 &pos, const Vector2 &size, const float fSwaySpeed, const float fSwayWidth)
{
	CGrass *pGrass = nullptr;

	// 草を生成
	pGrass = new CGrass;
	if (pGrass == nullptr)
	{ // 生成失敗
		return nullptr;
	}

	// 草の初期化
	pGrass->Init(pos, size, fSwaySpeed, fSwayWidth);
	return pGrass;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CGrass::CGrass()
{ // メンバ変数のクリア
	m_pVtxBuff = nullptr;
	m_nIdxTexture = -1;
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_rotDest = VECTOR3_NULL;
	m_size = VECTOR2_NULL;
	m_fSwaySpeed = 0.0f;
	m_fSwayWidth = 0.0f;
	m_fLerpCount = 0.0f;
	m_fSpeed = 0.0f;
	m_bCollisionPlayer = false;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CGrass::~CGrass()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CGrass::Init(const Vector3 &pos, const Vector2 &size, const float fSwaySpeed, const float fSwayWidth)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CRand *pRand = CRand::GetInstance();				// 乱数デバイスへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	HRESULT hr;						// 頂点バッファの生成結果
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
	m_size = size;
	m_fSwayWidth = fSwayWidth;
	m_fSwaySpeed = fSwaySpeed;

	// 角度をランダムに変更
	m_rot.y = pRand->Generate(-D3DX_PI, D3DX_PI);

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標設定
	pVtx[0].pos.x = -m_size.x * 0.5f;
	pVtx[0].pos.y = 0.0f;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = -m_size.x * 0.5f;
	pVtx[1].pos.y = m_size.y;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_size.x * 0.5f;
	pVtx[2].pos.y = 0.0f;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_size.x * 0.5f;
	pVtx[3].pos.y = m_size.y;
	pVtx[3].pos.z = 0.0f;

	// 法線ベクトルの設定
	pVtx[0].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = Vector3(0.0f, 0.0f, -1.0f);

	// 頂点カラー設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

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
void CGrass::Uninit(void)
{
	// 頂点バッファの破棄
	SafeRelease(m_pVtxBuff);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CGrass::Update(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CRand *pRand = CRand::GetInstance();				// 乱数デバイスへのポインタ
	float fHeight = 0.0f;		// メッシュフィールドとの高さ

	if (m_fSpeed == 0.4f)
	{ // プレイヤーと接触直後
		m_rot.x = (m_rotDest.x - m_rot.x) * m_fSpeed;
		m_rot.z = (m_rotDest.z - m_rot.z) * m_fSpeed;
	}
	else
	{ // 待機中
#if 0
		m_rot.x += m_rotDest.x * m_fSpeed;
		if (fabsf(m_rot.x) >= fabsf(m_fRotPower))
		{
			if (m_fRotPower >= 0.0f)
			{ // 角度が指定の幅に達したら
				// 傾く幅をランダムに指定
				m_fRotPower = pRand->Generate(-0.25f, -0.1f);
			}
			else
			{
				// 傾く幅をランダムに指定
				m_fRotPower = pRand->Generate(0.1f, 0.25f);
			}

			m_rotDest.x = m_fRotPower;
		}
#endif
	}

	m_rot = Vec3::FixedRotation(m_rot);

	m_rotDest.x += (0.0f - m_rotDest.x) * 0.035f;
	m_rotDest.z += (0.0f - m_rotDest.z) * 0.035f;
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CGrass::Draw(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture *pTexture = CTexture::GetInstance();		// テクスチャへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスの計算
	Mtx::CalcWorld(&m_mtxWorld, m_pos, m_rot);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// 頂点フォーマット設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);
}

//==================================================================================
// --- 当たり判定処理 ---
//==================================================================================
bool CGrass::Collision(const Vector3 &pos, const float fRadius)
{
	CManager *pManager = CManager::GetInstance();	// マネージャーへのポインタ
	auto pProc = pManager->GetDebugProc();			// デバッグ表示へのポインタ
	Vector3 vecToGrass = VECTOR3_NULL;			// プレイヤーから草へのベクトル
	Vector3 norVec = VECTOR3_NULL;	// 正規化されたベクトル
	Vector3 rot = VECTOR3_NULL;		// 計算後の角度
	float fLength = 0.0f;		// posと草の距離
	float fAngle = 0.0f;		// 角度
	float fPower = 0.0f;		// 倒れる強さ

	// 距離を測る
	fLength = Vec3::Length(pos, m_pos);
	if (fLength >= fRadius)
	{ // 当たり判定をする半径外
		return false;
	}

	// プレイヤーと草の正規化ベクトルを求める
	vecToGrass = m_pos - pos;

	// ベクトルの正規化した値を求める
	D3DXVec3Normalize(&norVec, &vecToGrass);

	// 倒す割合を求める
	fPower = ((fRadius - fLength) / fRadius);

	// 倒す角度を求める
	rot.x = ((cosf(m_rot.y) * norVec.z) + (sinf(m_rot.y) * norVec.x)) * (D3DX_PI) * fPower;
	rot.z = ((cosf(m_rot.y) * norVec.x) + (sinf(m_rot.y) * -norVec.z)) * (-D3DX_PI) * fPower;
	
	// 代入
	m_rotDest.x = rot.x;
	m_rotDest.z = rot.z;

	// 角度の強制
	m_rotDest = Vec3::FixedRotation(m_rotDest);

	// 目標角度に近づく速度を変更
	m_fSpeed = 0.4f;

	return true;
}