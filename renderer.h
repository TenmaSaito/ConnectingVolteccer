//==================================================================================
// 
// レンダラークラスのヘッダーファイル [renderer.h]
// Author : TENMA SAITO
// Date   : 2026/5/8
// 
//==================================================================================
#ifndef _RENDERER_H_		// インクルードガード
#define _RENDERER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define ENABLE_MULTI_TARGET_RENDERING		// マルチターゲットレンダリング
#define ENABLE_FEEDBACK_EFFECT				// フィードバックエフェクト
#define FEEDBACK_TEX_NUM		(2)			// フィードバックエフェクト用バッファの配列数

//**********************************************************************************
// *** レンダラークラス ***
//**********************************************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(const HWND hWnd, const BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	LPDIRECT3D9 GetD3DObject(void) const;
	LPDIRECT3DDEVICE9 GetDevice(void) const;
	void SetDefaultTarget(void);
	void ChangeTarget(const Vector3 &posV, 
		const Vector3 &posR, 
		const Vector3 &vecU,
		const D3DVIEWPORT9 *pViewport = nullptr);
#ifdef ENABLE_FEEDBACK_EFFECT
	LPDIRECT3DTEXTURE9 GetTextureMT(void) { return m_apTextureMT[0]; }
	void SetEnableFeedBack(const bool bEnable) { m_bEnableFeedBack = bEnable; }
	bool GetEnableFeedBack(void) const { return m_bEnableFeedBack; }
#else
	LPDIRECT3DTEXTURE9 GetTextureMT(void) { return m_pTextureMT; }
#endif

private:
#ifdef ENABLE_FEEDBACK_EFFECT
	void DrawFeedBack(void);
	void DrawScreen(void);
	void SwapTarget(void);
#endif
	LPDIRECT3D9 m_pD3D;					// Direct3Dオブジェクトへのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;		// 〃デバイスへのポインタ
	LPDIRECT3DSURFACE9 m_pRenderDef;	// デフォルトインターフェース
	LPDIRECT3DSURFACE9 m_pZBuffDef;		// 〃Zバッファ
	D3DVIEWPORT9 m_viewportDef;			// 〃ビューポート
	Matrix m_mtxProjDef;			// 〃プロジェクションマトリックス
	Matrix m_mtxViewDef;			// 〃ビューマトリックス
#ifdef ENABLE_FEEDBACK_EFFECT
	LPDIRECT3DTEXTURE9 m_apTextureMT[FEEDBACK_TEX_NUM];		// レンダリングターゲット用テクスチャ
	LPDIRECT3DSURFACE9 m_apRenderMT[FEEDBACK_TEX_NUM];		// テクスチャレンダリング用インターフェース
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffMT;	// フィードバックエフェクト用頂点バッファ
	bool m_bEnableFeedBack;					// フィードバックエフェクトの有無
#else
	LPDIRECT3DTEXTURE9 m_pTextureMT;	// レンダリングターゲット用テクスチャ
	LPDIRECT3DSURFACE9 m_pRenderMT;		// テクスチャレンダリング用インターフェース
#endif
	LPDIRECT3DSURFACE9 m_pZBuffMT;		// 〃Zバッファ
	D3DVIEWPORT9 m_viewportMT;			// 〃ビューポート
};
#endif