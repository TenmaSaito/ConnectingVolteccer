//==================================================================================
// 
// レンダラークラスのソースファイル [renderer.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/8
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "renderer.h"
#include "manager.h"
#include "debugproc.h"
#include "object.h"
#include "camera.h"
#include "sceneTransition.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define FEEDBACK_SIZE		(8.0f)		// フィードバック用ポリゴンの差分
#define FEEDBACK_ALPHA		(0.80f)		// フィードバック用ポリゴンのα値

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CRenderer::CRenderer()
{
	// 各メンバ変数のクリア
	m_pD3D = nullptr;
	m_pD3DDevice = nullptr;
	m_pRenderDef = nullptr;
	m_pZBuffDef = nullptr;
	ZeroMemory(m_apRenderMT, sizeof(m_apRenderMT));
	ZeroMemory(m_apTextureMT, sizeof(m_apTextureMT));
	m_pVtxBuffMT = nullptr;
	m_pZBuffMT = nullptr;
	m_viewportDef = {};
	m_viewportMT = {};
	m_bEnableFeedBack = false;
	D3DXMatrixIdentity(&m_mtxProjDef);
	D3DXMatrixIdentity(&m_mtxViewDef);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CRenderer::~CRenderer()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ		

	// Direct3Dオブジェクトの作成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == nullptr)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,
		&d3ddm)))
	{
		return E_FAIL;
	}

	D3DFORMAT AutoDepthStencilFormat = D3DFMT_D24S8;		// 深度バッファ及びステンシルバッファのビット数

	// 深度24,ステンシル8bitで使用できるかチェック
	if (FAILED(m_pD3D->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		d3ddm.Format,
		D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE,
		AutoDepthStencilFormat)))
	{ // 深度24,ステンシル8bitを使えない場合
		// 深度24bitのみに制限して再チェック
		AutoDepthStencilFormat = D3DFMT_D24X8;
		if (FAILED(m_pD3D->CheckDeviceFormat(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE,
			AutoDepthStencilFormat)))
		{ // 深度24bitを使えない場合
			AutoDepthStencilFormat = D3DFMT_D16;		// 深度16bitのみに制限
		}
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));			// パラメータのゼロクリア

	d3dpp.BackBufferWidth = SCREEN_WIDTH;		// ゲームの画面サイズ(横)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;		// ゲームの画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;		// バックバッファの形式
	d3dpp.BackBufferCount = 1;					// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// ダブルバッファの切り替え(映像信号と同期)
	d3dpp.EnableAutoDepthStencil = TRUE;		// デプスバッファとステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = AutoDepthStencilFormat;	// デプスバッファとして使えるbitを使う
	d3dpp.Windowed = bWindow;					// ウィンドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;			// インターバル

	// Direct3Dデバイスの作成
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&d3dpp,
		&m_pD3DDevice)))
	{
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
			&d3dpp,
			&m_pD3DDevice)))
		{
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
				&d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	// Zバッファ作成
	if (FAILED(m_pD3DDevice->CreateDepthStencilSurface(SCREEN_WIDTH,
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

#ifdef ENABLE_FEEDBACK_EFFECT
	for (int nCntMulti = 0; nCntMulti < 2; nCntMulti++)
	{
		// レンダリングターゲット用テクスチャ作成
		if (FAILED(m_pD3DDevice->CreateTexture(SCREEN_WIDTH,
			SCREEN_HEIGHT,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&m_apTextureMT[nCntMulti],
			NULL)))
		{ // テクスチャ作成失敗
			return E_FAIL;
		}

		// テクスチャレンダリング用インターフェース取得
		if (FAILED(m_apTextureMT[nCntMulti]->GetSurfaceLevel(0, &m_apRenderMT[nCntMulti])))
		{ // インターフェース取得失敗
			return E_FAIL;
		}

		// 現在のレンダリングターゲットとZバッファを取得
		if (FAILED(m_pD3DDevice->GetRenderTarget(0, &m_pRenderDef))
			|| FAILED(m_pD3DDevice->GetDepthStencilSurface(&m_pZBuffDef)))
		{ // 取得失敗
			return E_FAIL;
		}

		// 現在のビューポートを取得
		m_pD3DDevice->GetViewport(&m_viewportDef);

		// レンダリングターゲットを切り替え
		if (FAILED(m_pD3DDevice->SetRenderTarget(0, m_apRenderMT[nCntMulti]))
			|| FAILED(m_pD3DDevice->SetDepthStencilSurface(m_pZBuffMT)))
		{ // 切り替え失敗
			return E_FAIL;
		}

		// レンダリングターゲットをクリア
		if (FAILED(m_pD3DDevice->Clear(0, NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0)))
		{ // クリア失敗
			return E_FAIL;
		}

		// レンダリングターゲットを元に戻す
		if (FAILED(m_pD3DDevice->SetRenderTarget(0, m_pRenderDef))
			|| FAILED(m_pD3DDevice->SetDepthStencilSurface(m_pZBuffDef)))
		{ // 切り替え失敗
			return E_FAIL;
		}
	}

	// フィードバック用頂点バッファ生成
	m_pD3DDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffMT,
		NULL);

	VERTEX_2D *pVtx = nullptr;		// 頂点バッファへのポインタ

	// 頂点バッファをロック
	m_pVtxBuffMT->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = Vector3(-FEEDBACK_SIZE, -FEEDBACK_SIZE, 0.0f);
	pVtx[1].pos = Vector3(SCREEN_WIDTH + FEEDBACK_SIZE, -FEEDBACK_SIZE, 0.0f);
	pVtx[2].pos = Vector3(-FEEDBACK_SIZE, SCREEN_HEIGHT + FEEDBACK_SIZE, 0.0f);
	pVtx[3].pos = Vector3(SCREEN_WIDTH + FEEDBACK_SIZE, SCREEN_HEIGHT + FEEDBACK_SIZE, 0.0f);

	// 座標変換用変数設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラー設定
	pVtx[0].col = Color(1.0f, 1.0f, 1.0f, FEEDBACK_ALPHA);
	pVtx[1].col = Color(1.0f, 1.0f, 1.0f, FEEDBACK_ALPHA);
	pVtx[2].col = Color(1.0f, 1.0f, 1.0f, FEEDBACK_ALPHA);
	pVtx[3].col = Color(1.0f, 1.0f, 1.0f, FEEDBACK_ALPHA);

	// テクスチャ座標設定
	pVtx[0].tex = Vector2(0.0f, 0.0f);
	pVtx[1].tex = Vector2(1.0f, 0.0f);
	pVtx[2].tex = Vector2(0.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 1.0f);

	// 頂点バッファをアンロック
	m_pVtxBuffMT->Unlock();
#else
#endif
	// ビューポート設定
	m_viewportMT.X = 0;
	m_viewportMT.Y = 0;
	m_viewportMT.Width = SCREEN_WIDTH;
	m_viewportMT.Height = SCREEN_HEIGHT;
	m_viewportMT.MinZ = 0.0f;
	m_viewportMT.MaxZ = 1.0f;

	// レンダーステートの設定(消さないこと！ALPHA値の設定を適用する為の設定！)
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);		// テクスチャの拡縮補間の設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);		// テクスチャの繰り返しの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定(テクスチャのアルファブレンドの設定[テクスチャとポリゴンのALPHA値を混ぜる！])
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// 初期化成功
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CRenderer::Uninit(void)
{
	// 頂点バッファの破棄
	SafeRelease(m_pVtxBuffMT);

	// 取得していたインターフェースを破棄
	SafeRelease(m_pRenderDef);

	// 取得していたZバッファを破棄
	SafeRelease(m_pZBuffDef);

	// レンダリングターゲット用テクスチャとテクスチャインターフェースの破棄
	for (int nCntMulti = 0; nCntMulti < FEEDBACK_TEX_NUM; nCntMulti++)
	{
		SafeRelease(m_apTextureMT[nCntMulti]);
		SafeRelease(m_apRenderMT[nCntMulti]);
	}

	// テクスチャZバッファの破棄
	SafeRelease(m_pZBuffMT);

	// Direct3Dデバイスの破棄
	SafeRelease(m_pD3DDevice);

	// Direct3Dオブジェクトの破棄
	SafeRelease(m_pD3D);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CRenderer::Update(void)
{ // 全てのオブジェクトを更新
	CObject::UpdateAll();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CRenderer::Draw(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CDebugProc *pDebugProc = pManager->GetDebugProc();	// デバッグ表示へのポインタ

	// 画面クリア(バックバッファとZバッファのクリア)
	m_pD3DDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// 描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{// 描画開始が成功した場合
			// カメラの設置
		CCamera *pCurrentCamera = CCamera::Begin(CCamera::GetFocus());
		if (pCurrentCamera != nullptr)
		{ // カメラが生成済みならビューポート変更
			// レンダリングターゲット変更
			ChangeTarget(*pCurrentCamera->GetPosV(),
				*pCurrentCamera->GetPosR(),
				*pCurrentCamera->GetVecU(),
				pCurrentCamera->GetViewport());
		}

		// 画面クリア(バックバッファとZバッファのクリア)
		m_pD3DDevice->Clear(0, NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			D3DCOLOR_RGBA(0, 0, 50, 0), 1.0f, 0);

		// 全てのオブジェクトを描画
		CObject::DrawAll();

		// カメラの設置終了
		CCamera::End();

#ifdef ENABLE_MULTI_TARGET_RENDERING
		if (m_bEnableFeedBack == true)
		{ // フィードバック用ポリゴンにテクスチャ1を張り付けて描画
			DrawFeedBack();
		}
#endif 
		// ターゲットをスクリーンに戻す
		SetDefaultTarget();

		// スクリーン用ポリゴンの描画
		DrawScreen();

		// 遷移用トランジション描画
		pManager->GetTransition()->Draw();

		// デバッグ表示の描画
		pDebugProc->Draw();

		// 描画終了
		m_pD3DDevice->EndScene();
	}

	// テクスチャの入れ替え
	SwapTarget();

	// バックバッファとフロントバッファの入れ替え
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//==================================================================================
// --- デフォルトレンダリングターゲット設定処理 ---
//==================================================================================
void CRenderer::SetDefaultTarget(void)
{ // レンダリングターゲットを元に戻す
	m_pD3DDevice->SetRenderTarget(0, m_pRenderDef);
	m_pD3DDevice->SetDepthStencilSurface(m_pZBuffDef);
	m_pD3DDevice->SetViewport(&m_viewportDef);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjDef);
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_mtxViewDef);
}

//==================================================================================
// --- レンダリングターゲット変更処理 ---
//==================================================================================
void CRenderer::ChangeTarget(const Vector3 &posV,
	const Vector3 &posR,
	const Vector3 &vecU,
	const D3DVIEWPORT9 *pViewport)
{
	Matrix mtxProj, mtxView;	// ビュー・プロジェクションマトリックス
	const D3DVIEWPORT9 *pViewportMT = &m_viewportMT;		// 設定するビューポートへのポインタ
	float fAspect;					// アスペクト比

	if (pViewport != nullptr)
	{ // ビューポートがNULLではない場合、そちらを優先
		pViewportMT = pViewport;
	}

	// レンダリングターゲットを切り替え
	if (FAILED(m_pD3DDevice->SetRenderTarget(0, m_apRenderMT[0]))
		&& FAILED(m_pD3DDevice->SetDepthStencilSurface(m_pZBuffMT)))
	{ // 切り替え失敗
		return;
	}

	// ビューポート設定
	m_pD3DDevice->SetViewport(pViewportMT);

	// マトリックスの初期化
	D3DXMatrixIdentity(&mtxProj);
	D3DXMatrixIdentity(&mtxView);

	// プロジェクションマトリックスの作成
	fAspect = static_cast<float>(pViewportMT->Width) / static_cast<float>(pViewportMT->Height);
	D3DXMatrixPerspectiveFovLH(&mtxProj,
		D3DXToRadian(45.0f),						
		fAspect,				
		1.0f,					
		10000.0f);				

	// プロジェクションマトリックスの設定
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &m_mtxProjDef);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProj);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&mtxView,
		&posV,		
		&posR,		
		&vecU);		

	// ビューマトリックスの設定
	m_pD3DDevice->GetTransform(D3DTS_VIEW, &m_mtxViewDef);
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &mtxView);
}

//==================================================================================
// --- フィードバック用ポリゴン描画処理 ---
//==================================================================================
void CRenderer::DrawFeedBack(void)
{
	VERTEX_2D* pVtx = nullptr;		// 頂点バッファへのポインタ

	// 頂点バッファをロック
	m_pVtxBuffMT->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = Vector3(-FEEDBACK_SIZE, -FEEDBACK_SIZE, 0.0f);
	pVtx[1].pos = Vector3(SCREEN_WIDTH + FEEDBACK_SIZE, -FEEDBACK_SIZE, 0.0f);
	pVtx[2].pos = Vector3(-FEEDBACK_SIZE, SCREEN_HEIGHT + FEEDBACK_SIZE, 0.0f);
	pVtx[3].pos = Vector3(SCREEN_WIDTH + FEEDBACK_SIZE, SCREEN_HEIGHT + FEEDBACK_SIZE, 0.0f);

	// 頂点バッファをアンロック
	m_pVtxBuffMT->Unlock();

	// フィードバック用頂点バッファをストリームに設定
	m_pD3DDevice->SetStreamSource(0, m_pVtxBuffMT, 0, sizeof(VERTEX_2D));

	// テクスチャ0を設定
	m_pD3DDevice->SetTexture(0, m_apTextureMT[1]);

	// 頂点フォーマット設定
	m_pD3DDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴンの描画
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);
}

//==================================================================================
// --- フィードバック用ポリゴン描画処理 (スクリーン描画) ---
//==================================================================================
void CRenderer::DrawScreen(void)
{
	VERTEX_2D* pVtx = nullptr;		// 頂点バッファへのポインタ

	// 頂点バッファをロック
	m_pVtxBuffMT->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = Vector3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = Vector3(SCREEN_WIDTH, 0.0f, 0.0f);
	pVtx[2].pos = Vector3(0.0f, SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = Vector3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	// 頂点バッファをアンロック
	m_pVtxBuffMT->Unlock();

	// 頂点バッファのα値の適用を無効に設定
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// フィードバック用頂点バッファをストリームに設定
	m_pD3DDevice->SetStreamSource(0, m_pVtxBuffMT, 0, sizeof(VERTEX_2D));

	// テクスチャ0を設定
	m_pD3DDevice->SetTexture(0, m_apTextureMT[0]);

	// 頂点フォーマット設定
	m_pD3DDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴンの描画
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		2);

	// 頂点バッファのα値の適用を有効に設定
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}

//==================================================================================
// --- テクスチャのスワップ処理 ---
//==================================================================================
void CRenderer::SwapTarget(void)
{
	LPDIRECT3DSURFACE9 pSurfaceWk = nullptr;		// スワップ用変数
	LPDIRECT3DTEXTURE9 pTextureWk = nullptr;		// スワップ用変数

	// テクスチャを入れ替え
	pSurfaceWk = m_apRenderMT[0];
	m_apRenderMT[0] = m_apRenderMT[1];
	m_apRenderMT[1] = pSurfaceWk;

	pTextureWk = m_apTextureMT[0];
	m_apTextureMT[0] = m_apTextureMT[1];
	m_apTextureMT[1] = pTextureWk;
}

//==================================================================================
// --- Direct3Dオブジェクト取得処理 ---
//==================================================================================
LPDIRECT3D9 CRenderer::GetD3DObject(void) const
{
	return m_pD3D;
}

//==================================================================================
// --- Direct3Dデバイス処理 ---
//==================================================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice(void) const
{
	return m_pD3DDevice;
}