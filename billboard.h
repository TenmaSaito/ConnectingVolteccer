//==================================================================================
// 
// ビルボードクラスのヘッダーファイル [billboard.h]
// Author : TENMA SAITO
// Date   : 2026/7/14
// 
//==================================================================================
#ifndef _BILLBOARD_H_		// インクルードガード
#define _BILLBOARD_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** ビルボードクラス ***
//**********************************************************************************
class CBillboard
{
public:
	CBillboard();
	~CBillboard();

	static CBillboard *Create(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size);

	HRESULT Init(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 &pos) { m_pos = pos; }
	const D3DXVECTOR3 *GetPosition(void) const { return &m_pos; }
	void SetSize(const D3DXVECTOR2 &size);
	const D3DXVECTOR2 *GetSize(void) const { return &m_size; }
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture);
	void BindTexture(const int nIdxTexture);
	void SetParent(const D3DXMATRIX *pMtxParent) { m_pMtxParent = pMtxParent; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	LPDIRECT3DTEXTURE9 m_pTexture;		// テクスチャバッファ
	int m_nIdxTexture;			// テクスチャインデックス
	D3DXVECTOR3 m_pos;			// 中心座標
	D3DXVECTOR2 m_size;			// サイズ
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	const D3DXMATRIX *m_pMtxParent;		// 親マトリックスへのポインタ
	bool m_bUseIndex;			// テクスチャインデックスを使用するか
};
#endif