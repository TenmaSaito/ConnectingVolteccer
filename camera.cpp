//==================================================================================
// 
// カメラクラスのソースファイル [camera.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "camera.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "vec3math.h"
#include "debugproc.h"
#include "ray.h"
#include "effect.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_FOVY		(45.0f)		// 視野角
#define DEFAULT_ZN			(1.0f)		// 最短距離
#define DEFAULT_ZF			(10000.0f)	// 最遠距離
#define DEFAULT_SPD			(1.0f)		// 移動速度
#define DEFAULT_ROTSPD		(0.01f)		// 回転速度
#define DEFAULT_LENGTH		(300.0f)	// 注視点との距離

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CCamera::CCamera(const TYPE type)
{ // メンバ変数をクリア
	m_posV = VECTOR3_NULL;
	m_posR = VECTOR3_NULL;
	m_vecU = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_vp = {};
	m_bEnableOrtho = false;

	// タイプを保存
	m_type = type;

	// カメラを登録
	m_apCamera[type] = this;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CCamera::~CCamera()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
void CCamera::Init(const Vector3 &posV, const Vector3 &posR)
{
	{ // メンバ変数を初期化
		D3DXMatrixIdentity(&m_mtxProjection);
		D3DXMatrixIdentity(&m_mtxView);
		m_rot = VECTOR3_NULL;
	}

	{ // 視点・注視点・上方向ベクトル・ビューポートを設定
		m_posV = posV;
		m_posR = posR;
		m_vecU = Vector3(0.0f, 1.0f, 0.0f);
		m_vp = DEFAULT_VP;
	}
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CCamera::Uninit(void)
{
	Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CCamera::Update(void)
{ // 視点座標を注視点座標から求める
	m_posV.x = m_posR.x + (sinf(m_rot.z) * sinf(m_rot.y) * DEFAULT_LENGTH);
	m_posV.y = m_posR.y + (cosf(m_rot.z) * DEFAULT_LENGTH);
	m_posV.z = m_posR.z + (sinf(m_rot.z) * cosf(m_rot.y) * DEFAULT_LENGTH);
}

//==================================================================================
// --- カメラの設置処理 ---
//==================================================================================
void CCamera::SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイスへのポインタ
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };		// 画面サイズ

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	if (m_bEnableOrtho == true)
	{ // 正射影投影によるプロジェクションマトリックスの設定
		// マトリックスの作成
		D3DXMatrixOrthoLH(&m_mtxProjection,					// 書き出し先
			static_cast<float>(m_vp.Width),					// スクリーンの幅
			static_cast<float>(m_vp.Height),				// スクリーンの高さ
			DEFAULT_ZN,					// カメラの描画最小範囲
			DEFAULT_ZF);				// カメラの描画最大範囲
	}
	else
	{ // 透視投影によるプロジェクションマトリックスの設定
		// マトリックスの作成
		D3DXMatrixPerspectiveFovLH(&m_mtxProjection,		// 書き出し先
			D3DXToRadian(DEFAULT_FOVY),						// 視野角
			static_cast<float>(m_vp.Width)
			/ static_cast<float>(m_vp.Height),				// スクリーンのアスペクト比
			DEFAULT_ZN,					// カメラの描画最小範囲
			DEFAULT_ZF);				// カメラの描画最大範囲
	}

	// マトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	{ // ビューマトリックスの設定
		// マトリックスの初期化
		D3DXMatrixIdentity(&m_mtxView);

		// マトリックスの作成
		D3DXMatrixLookAtLH(&m_mtxView,
			&m_posV,		// 視点
			&m_posR,		// 注視点
			&m_vecU);		// 上方向ベクトル

		// マトリックスの設定
		pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
	}
}

//==================================================================================
// --- カメラの内外判定処理 ---
//==================================================================================
bool CCamera::IsVisible(const Vector3 &pos)
{
	bool bBeside = false;		// 横判定
	bool bVertical = false;		// 縦判定

	// 横幅を基準とした縦幅のアスペクト比
	float fAspectVertical = static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(SCREEN_WIDTH);

	{ // 縦横それぞれ判定を行う
		bBeside = Vec3::IsInsideViewOfBeside(pos, m_posV, m_posR, D3DXToRadian(DEFAULT_FOVY));
		bVertical = Vec3::IsInsideViewOfVertical(pos, m_posV, m_posR, D3DXToRadian(DEFAULT_FOVY) * fAspectVertical);
	}

	return bBeside && bVertical;
}

//==================================================================================
// --- カメラのレイ取得処理 ---
//==================================================================================
Vector3 CCamera::GetRay(void) const
{
	return Vec3::Direction(m_posR, m_posV);
}

//==================================================================================
// --- カメラからスクリーン上の座標へのレイ取得処理 ---
//==================================================================================
Vector3 CCamera::GetRayToScreen(const Vector2 &screenPos)
{
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DVIEWPORT9 vp;		// ビューポート
	Matrix mtxVp, mtxView, mtxProj;				// ビューポート行列, ビュー行列, プロジェクション行列
	Matrix mtxVpInv, mtxViewInv, mtxProjInv;	// ビューポート逆行列, ビュー逆行列, プロジェクション逆行列
	Vector3 from, to;	// レイベクトルの始点、終点

	// ビューポート行列を初期化
	D3DXMatrixIdentity(&mtxVp);

	// ビューポートを取得
	pDevice->GetViewport(&vp);

	// ビュー行列、プロジェクション行列を取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProj);

	// レイベクトルの始点, 終点を設定
	from = Vector3(screenPos.x, screenPos.y, 0.0f);
	to = Vector3(screenPos.x, screenPos.y, 1.0f);

	// 行列計算
	mtxVp._11 = static_cast<float>(vp.Width) / 2.0f;
	mtxVp._22 = -1.0f * static_cast<float>(vp.Height / 2);
	mtxVp._33 = static_cast<float>(vp.MaxZ - vp.MinZ);
	mtxVp._41 = static_cast<float>(vp.X + (vp.Width / 2));
	mtxVp._42 = static_cast<float>(vp.Y + (vp.Height / 2));
	mtxVp._43 = vp.MinZ;

	// 各行列の逆行列を計算
	D3DXMatrixInverse(&mtxVpInv, nullptr, &mtxVp);
	D3DXMatrixInverse(&mtxViewInv, nullptr, &mtxView);
	D3DXMatrixInverse(&mtxProjInv, nullptr, &mtxProj);

	// 行列をかけ合わせる
	mtxVpInv *= mtxProjInv * mtxViewInv;

	// 各座標をワールド座標に変換
	D3DXVec3TransformCoord(&from, &from, &mtxVpInv);
	D3DXVec3TransformCoord(&to, &to, &mtxVpInv);

	return Vec3::Direction(to, from);
}

//==================================================================================
// --- 全カメラの解放処理 ---
//==================================================================================
void CCamera::ReleaseAll(void)
{
	for (auto &pCamera : m_apCamera)
	{ // カメラの種類分繰り返す
		if (pCamera == nullptr) continue;

		// カメラの終了処理
		pCamera->Uninit();
	}
}

//==================================================================================
// --- 全カメラの更新処理 ---
//==================================================================================
void CCamera::UpdateAll(void)
{
	for (auto &pCamera : m_apCamera)
	{ // カメラの種類分繰り返す
		if (pCamera == nullptr) continue;

		// カメラの更新処理
		pCamera->Update();
	}
}

//==================================================================================
// --- 現在のカメラのタイプ取得処理 ---
//==================================================================================
CCamera::TYPE CCamera::GetTarget(void)
{
	return m_currentType;
}

//==================================================================================
// --- カメラの設置開始処理 ---
//==================================================================================
CCamera *CCamera::Begin(const int nIdx)
{ // nullの場合スキップ
	if (m_apCamera[nIdx] == nullptr) return nullptr;

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイスへのポインタ

	// ビューポートを取得
	pDevice->GetViewport(&m_vpDef);

	// ビューポートを設定
	pDevice->SetViewport(&m_apCamera[nIdx]->m_vp);

	// カメラを設置
	m_apCamera[nIdx]->SetCamera();

	// 設置タイプの更新
	m_currentType = m_apCamera[nIdx]->m_type;

	return m_apCamera[nIdx];
}

//==================================================================================
// --- カメラの設置終了処理 ---
//==================================================================================
void CCamera::End(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイスへのポインタ

	// ビューポートを元に戻す
	pDevice->SetViewport(&m_vpDef);
}

//==================================================================================
// --- カメラの解放処理 ---
//==================================================================================
void CCamera::Release(void)
{
	if (m_apCamera[m_type] != nullptr)
	{ // NULLではなかった場合 (未解放の場合)
		// NULLを代入し、解放
		m_apCamera[m_type] = nullptr;
		delete this;
	}
}