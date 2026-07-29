//==================================================================================
// 
// テクスチャレンダラークラスのソースファイル [textureRenderer.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/6
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "textureRenderer.h"
#include "manager.h"
#include "renderer.h"
#include <wrl/client.h>

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CTextureRenderer::CTextureRenderer()
{ // メンバ変数をクリア
	m_pTextureMT = nullptr;
	m_pRenderMT = nullptr;
	m_pZBuffMT = nullptr;
	m_viewportMT = {};
	m_posV = VECTOR3_NULL;
	m_posR = VECTOR3_NULL;
	m_vecU = VECTOR3_NULL;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CTextureRenderer::~CTextureRenderer()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CTextureRenderer::Init(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャーへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();		// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスへのポインタ
	Microsoft::WRL::ComPtr<IDirect3DSurface9> pRenderDef = nullptr;		// デフォルトのインターフェースへのポインタ
	Microsoft::WRL::ComPtr<IDirect3DSurface9> pZBuffDef = nullptr;		// デフォルトのZバッファへのポインタ
	D3DVIEWPORT9 viewportDef = {};		// デフォルトのビューポート

	// レンダリングターゲット用テクスチャ作成
	if (FAILED(pDevice->CreateTexture(SCREEN_WIDTH,
		SCREEN_HEIGHT,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pTextureMT,
		NULL)))
	{ // テクスチャ作成失敗
		return E_FAIL;
	}

	// テクスチャレンダリング用インターフェース取得
	if (FAILED(m_pTextureMT->GetSurfaceLevel(0, &m_pRenderMT)))
	{ // インターフェース取得失敗
		return E_FAIL;
	}

	// Zバッファ作成
	if (FAILED(pDevice->CreateDepthStencilSurface(SCREEN_WIDTH,
		SCREEN_HEIGHT,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pZBuffMT,
		NULL)))
	{ // Zバッファ生成失敗
		return E_FAIL;
	}

	// 現在のレンダリングターゲットとZバッファを取得
	if (FAILED(pDevice->GetRenderTarget(0, &pRenderDef))
		|| FAILED(pDevice->GetDepthStencilSurface(&pZBuffDef)))
	{ // 取得失敗
		return E_FAIL;
	}

	// 現在のビューポートを取得
	pDevice->GetViewport(&viewportDef);

	// レンダリングターゲットを切り替え
	if (FAILED(pDevice->SetRenderTarget(0, m_pRenderMT))
		|| FAILED(pDevice->SetDepthStencilSurface(m_pZBuffMT)))
	{ // 切り替え失敗
		return E_FAIL;
	}

	// レンダリングターゲットをクリア
	if (FAILED(pDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0)))
	{ // クリア失敗
		return E_FAIL;
	}

	// レンダリングターゲットを元に戻す
	if (FAILED(pDevice->SetRenderTarget(0, pRenderDef.Get()))
		|| FAILED(pDevice->SetDepthStencilSurface(pZBuffDef.Get())))
	{ // 切り替え失敗
		return E_FAIL;
	}

	// ビューポート設定
	m_viewportMT.X = 0;
	m_viewportMT.Y = 0;
	m_viewportMT.Width = SCREEN_WIDTH;
	m_viewportMT.Height = SCREEN_HEIGHT;
	m_viewportMT.MinZ = 0.0f;
	m_viewportMT.MaxZ = 1.0f;

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CTextureRenderer::Uninit(void)
{ // 各バッファの解放
	SafeRelease(m_pTextureMT);
	SafeRelease(m_pRenderMT);
	SafeRelease(m_pZBuffMT);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CTextureRenderer::Update(void)
{
}

//==================================================================================
// --- レンダリングターゲット変更処理 ---
//==================================================================================
void CTextureRenderer::BeginRenderer(const D3DXVECTOR3 &posV, const D3DXVECTOR3 &posR, const D3DXVECTOR3 &vecU)
{

}