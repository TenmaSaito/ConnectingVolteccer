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

	static CPolygon2D *Create(const Vector3 &pos, 
		const Vector3 &rot, 
		const Vector2 &size);

	HRESULT Init(const Vector3 &pos,
		const Vector3 &rot, 
		const Vector2 &size);
	void Uninit();
	void Update();
	void Draw();
	void SetPosition(const Vector3 &position);
	void SetRotation(const Vector3 &rotation);
	void SetSize(const Vector2 &size);
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture);
	void BindTexture(const int nIdxTexture);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	LPDIRECT3DTEXTURE9 m_pTexture;		// テクスチャバッファ
	int m_nIdxTexture;			// テクスチャインデックス
	Vector3 m_pos;			// 中心座標
	Vector3 m_rot;			// 角度
	Vector2 m_size;			// サイズ
	float m_fLength;			// 対角線の長さ
	float m_fAngle;				// 対角線の角度
	bool m_bUseIndex;			// テクスチャインデックスを使用するか
};
#endif
