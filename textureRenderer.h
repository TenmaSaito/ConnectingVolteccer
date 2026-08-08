//==================================================================================
// 
// テクスチャレンダラークラスのヘッダーファイル [textureRenderer.h]
// Author : TENMA SAITO
// Date   : 2026/7/6
// 
//==================================================================================
#ifndef _TEXTURE_RENDERER_H_		// インクルードガード
#define _TEXTURE_RENDERER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** テクスチャレンダラークラス ***
//**********************************************************************************
class CTextureRenderer
{
public:
	CTextureRenderer();
	~CTextureRenderer();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void BeginRenderer(const Vector3 &posV, const Vector3 &posR, const Vector3 &vecU);
	LPDIRECT3DTEXTURE9 GetTexture(void) const { return m_pTextureMT; }
	Vector3 GetPosV(void) const { return m_posV; }
	Vector3 GetPosR(void) const { return m_posV; }
	Vector3 GetVecU(void) const { return m_vecU; }

private:
	LPDIRECT3DTEXTURE9 m_pTextureMT;	// マルチターゲットレンダリング用テクスチャ
	LPDIRECT3DSURFACE9 m_pRenderMT;		// テクスチャレンダリング用インターフェース
	LPDIRECT3DSURFACE9 m_pZBuffMT;		// 〃Zバッファ
	D3DVIEWPORT9 m_viewportMT;			// 〃ビューポート
	Vector3 m_posV;		// 視点
	Vector3 m_posR;		// 注視点
	Vector3 m_vecU;		// 上方向ベクトル
};
#endif
