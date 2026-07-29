//==================================================================================
// 
// スクリーンショットクラスのソースファイル [screenShot.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/16
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "windowCapture.h"
#include "manager.h"
#include "renderer.h"
#include "filestream.h"
#include <string>
#include <wrl/client.h>						// ComPtrに必要

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CWindowCapture::CWindowCapture()
{ // メンバ変数のクリア
	ZeroMemory(m_aDirectly, sizeof(m_aDirectly));
	m_nNumAll = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CWindowCapture::~CWindowCapture()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CWindowCapture::Init(const char *pDirectlyName)
{ // ディレクトリ名を登録
	strcpy_s(m_aDirectly, pDirectlyName);

	// ディレクトリフォルダを作成
	CreateDirectory(pDirectlyName, nullptr);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CWindowCapture::Uninit(void)
{
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CWindowCapture::Update(void)
{
}

//==================================================================================
// --- スクリーンショット処理 ---
//==================================================================================
HRESULT CWindowCapture::ScreenShot(D3DXIMAGE_FILEFORMAT format, const char *pFileName)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	CRenderer *pRenderer = pManager->GetRenderer();		// レンダラーへのポインタ
	HWND hWnd = pManager->GetWindowHandle();			// ウィンドウハンドル
	LPDIRECT3D9 pObject = nullptr;			// D3Dオブジェクトへのポインタ
	LPDIRECT3DDEVICE9 pDevice = nullptr;	// デバイスへのポインタ
	std::unique_ptr<CFileStream> pFile;		// ファイルストリーム
	std::string filepath = m_aDirectly;		// ファイルパス
	HRESULT hr = S_OK;						// 処理結果

	// JPGのみ対応 (変更可能なように引数は残す)
	if (format != D3DXIFF_JPG) return S_FALSE;

	// ファイルストリームを生成
	pFile = std::make_unique<CFileStream>();
	if (pFile == nullptr) return S_FALSE;

	// ファイルパスを作成
	filepath += "/";
	filepath += pFileName;

	// 既に同じ名前の画像が無いか確認
	if (pFile->CreateFile(filepath.c_str(), true, CFileStream::FLAG_CANCEL) == false)
	{ // 存在していた場合
		int nIdReturn = 0;		// 質問の返答結果

		// ファイルを上書きするか確認
		nIdReturn = MessageBox(hWnd,
			"既に同じ名前の画像ファイルが存在します。\n上書きしますか?", "Info", MB_YESNO | MB_ICONINFORMATION);
		if (nIdReturn == IDNO)
		{ // 上書きしない場合、処理を中断
			// ファイルを閉じる
			pFile->CloseFile();

			return S_FALSE;
		}

		// ファイルを閉じる
		pFile->CloseFile();
	}

	// 各変数を取得
	pObject = pRenderer->GetD3DObject();
	pDevice = pRenderer->GetDevice();
	
	Microsoft::WRL::ComPtr<IDirect3DSurface9> pSurface;			// 書き出し用サーフェイスへのポインタ	
	Microsoft::WRL::ComPtr<IDirect3DSurface9> pRenderSurface;	// スクリーンのサーフェイスへのポインタ	
	D3DSURFACE_DESC desc;		// サーフェイスの情報

	// バックバッファのサーフェイス取得
	hr = pDevice->GetRenderTarget(0, &pRenderSurface);
	if (FAILED(hr))
	{ // レンダーターゲット取得失敗
		return E_FAIL;
	}

	// バックバッファのサーフェイスの情報を取得
	hr = pRenderSurface->GetDesc(&desc);
	if (FAILED(hr))
	{ // サーフェイスの情報取得失敗
		return E_FAIL;
	}

	// 書き出し用サーフェイス生成
	hr = pDevice->CreateOffscreenPlainSurface(desc.Width,
		desc.Height,
		desc.Format,
		D3DPOOL_SYSTEMMEM,
		&pSurface, NULL);
	if (FAILED(hr))
	{ // サーフェイス生成失敗
		return E_FAIL;
	}

	// サーフェイスにバックバッファのイメージを保存する
	hr = pDevice->GetRenderTargetData(pRenderSurface.Get(), pSurface.Get());
	if (FAILED(hr))
	{ // イメージ保存失敗
		return E_FAIL;
	}

	// テクスチャを画像ファイルとして書き出し
	if (FAILED(hr = D3DXSaveSurfaceToFile(filepath.c_str(), format, pSurface.Get(), nullptr, nullptr)))
	{ // 書き出し失敗
		return E_FAIL;
	}

	return S_OK;
}