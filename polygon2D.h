//==================================================================================
// 
// ポリゴンクラスのヘッダーファイル [polygon.h]
// Author : TENMA SAITO
// Date   : 2026/7/13
// 
//==================================================================================
#ifndef _POLYGON2D_H_		// インクルードガード
#define _POLYGON2D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** ポリゴンクラス ***
//**********************************************************************************
class CPolygon2D
{
public:
	CPolygon2D();
	~CPolygon2D();

	static CPolygon2D *Create(const D3DXVECTOR3 &pos, 
		const D3DXVECTOR3 &rot, 
		const D3DXVECTOR2 &size);

	HRESULT Init(const D3DXVECTOR3 &pos,
		const D3DXVECTOR3 &rot, 
		const D3DXVECTOR2 &size);
	void Uninit();
	void Update();
	void Draw();
	void SetPosition(const D3DXVECTOR3 &position);
	void SetRotation(const D3DXVECTOR3 &rotation);
	void SetSize(const D3DXVECTOR2 &size);
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture);
	void BindTexture(const int nIdxTexture);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	LPDIRECT3DTEXTURE9 m_pTexture;		// テクスチャバッファ
	int m_nIdxTexture;			// テクスチャインデックス
	D3DXVECTOR3 m_pos;			// 中心座標
	D3DXVECTOR3 m_rot;			// 角度
	D3DXVECTOR2 m_size;			// サイズ
	float m_fLength;			// 対角線の長さ
	float m_fAngle;				// 対角線の角度
	bool m_bUseIndex;			// テクスチャインデックスを使用するか
};
#endif
