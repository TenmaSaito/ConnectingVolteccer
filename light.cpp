//==================================================================================
// 
// ライトクラスのソースファイル [camera.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "light.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "joypad.h"
#include "debugproc.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_FOVY		(45.0f)		// 視野角
#define DEFAULT_ZN			(0.0f)		// 最短距離
#define DEFAULT_ZF			(10000.0f)	// 最遠距離

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CLight::CLight()
{
	// メンバ変数をクリア
	memset(m_aLight, 0, sizeof(m_aLight));
	m_nIdxLight = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CLight::~CLight()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
void CLight::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetDeviceByInstance();		// デバイスへのポインタ
	D3DXVECTOR3 vecDir[3];			// ライトの方向ベクトル

	// ライトの情報を初期化
	memset(&m_aLight[0], 0, sizeof(m_aLight));

	// ライトの種類を設定
	m_aLight[0].Type = D3DLIGHT_DIRECTIONAL;
	m_aLight[1].Type = D3DLIGHT_DIRECTIONAL;
	m_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ライトの拡散光を設定
	m_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_aLight[1].Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	m_aLight[2].Diffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);

	// ライトの方向を設定
	vecDir[0] = D3DXVECTOR3(0.0f, -0.8f, -0.4f);
	D3DXVec3Normalize(&vecDir[0], &vecDir[0]);
	m_aLight[0].Direction = vecDir[0];

	vecDir[1] = D3DXVECTOR3(-0.3f, 0.4f, 0.5f);
	D3DXVec3Normalize(&vecDir[1], &vecDir[1]);
	m_aLight[1].Direction = vecDir[1];

	vecDir[2] = D3DXVECTOR3(0.2f, 0.1f, 0.1f);
	D3DXVec3Normalize(&vecDir[2], &vecDir[2]);
	m_aLight[2].Direction = vecDir[2];

	// ライトを設定する
	pDevice->SetLight(0, &m_aLight[0]);
	pDevice->SetLight(1, &m_aLight[1]);
	pDevice->SetLight(2, &m_aLight[2]);

	// ライトを有効にする
	pDevice->LightEnable(0, TRUE);
	pDevice->LightEnable(1, TRUE);
	pDevice->LightEnable(2, TRUE);
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CLight::Uninit(void)
{
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CLight::Update(void)
{
	CManager *pManager = CManager::GetInstance();			// マネージャーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイスへのポインタ
	auto pKeyboard = pManager->GetInputKeyboard();			// キーボードへのポインタ
	auto pJoypad = pManager->GetJoypad();					// ジョイパッドへのポインタ
	auto pProc = CManager::GetInstance()->GetDebugProc();		// デバッグ表示へのポインタ
	D3DXVECTOR3 vecDir = m_aLight[m_nIdxLight].Direction;	// ライトの向き 

	if (pKeyboard->GetPress(DIK_LSHIFT))
	{ // 左シフトを押している間ライティングモード
		pProc->Print("[Light操作中！ 操作中のライト : %d]\n", m_nIdxLight);
		pProc->Print("[上矢印 : 次のライトを操作 / 下矢印 : 前の矢印を操作\n]");
		pProc->Print("[QA : X座標 / WS : Y座標 / ED : Z座標 を操作！]\n");
		pProc->Print("[ライトの向き X:%f Y:%f Z:%f]\n", vecDir.x, vecDir.y, vecDir.z);

		if (pKeyboard->GetTrigger(DIK_UP))
		{ // ライトインデックスを増加
			m_nIdxLight = (m_nIdxLight + 1) % DEFAULT_LIGHT_NUM;
		}
		else if (pKeyboard->GetTrigger(DIK_DOWN))
		{ // ライトインデックスを減少
			m_nIdxLight = (m_nIdxLight + 1) % DEFAULT_LIGHT_NUM;
		}

		if (pKeyboard->GetPress(DIK_Q))
		{
			vecDir.x += 0.01f;
		}
		else if (pKeyboard->GetPress(DIK_A))
		{
			vecDir.x -= 0.01f;
		}

		if (pKeyboard->GetPress(DIK_W))
		{
			vecDir.y += 0.01f;
		}
		else if (pKeyboard->GetPress(DIK_S))
		{
			vecDir.y -= 0.01f;
		}

		if (pKeyboard->GetPress(DIK_E))
		{
			vecDir.z += 0.01f;
		}
		else if (pKeyboard->GetPress(DIK_D))
		{
			vecDir.z -= 0.01f;
		}

		// 正規化
		D3DXVec3Normalize(&vecDir, &vecDir);
		m_aLight[m_nIdxLight].Direction = vecDir;

		// ライトを再設定
		pDevice->SetLight(m_nIdxLight, &m_aLight[m_nIdxLight]);
	}
}