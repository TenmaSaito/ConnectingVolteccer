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
#include <functional>

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
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const Vector3 &position);
	const Vector3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const Vector3 &rotation);
	const Vector3 *GetRotation(void) const { return &m_rot; }
	void SetSize(const Vector2 &size);
	const Vector2 *GetSize(void) const { return &m_size; }
	void SetColor(const Color &color);
	void SetAlpha(const float fAlpha) { SetColor(Color(m_col.r, m_col.g, m_col.b, fAlpha)); }
	const Color *GetColor(void) const { return &m_col; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture);
	void BindTexture(const int nIdxTexture);
	void SetStateFunctionBeforeDraw(std::function<void(LPDIRECT3DDEVICE9)> before) { m_beforeDraw = before; }
	void SetStateFunctionAfterDraw(std::function<void(LPDIRECT3DDEVICE9)> after) { m_afterDraw = after; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;		// 頂点バッファ
	LPDIRECT3DTEXTURE9 m_pTexture = nullptr;			// テクスチャバッファ
	int m_nIdxTexture = -1;			// テクスチャインデックス
	Vector3 m_pos = VECTOR3_NULL;	// 中心座標
	Vector3 m_rot = VECTOR3_NULL;	// 角度
	Vector2 m_size = VECTOR2_NULL;	// サイズ
	Color m_col = COLOR_NULL;		// 色
	float m_fLength = 0.0f;			// 対角線の長さ
	float m_fAngle = 0.0f;			// 対角線の角度
	bool m_bUseIndex = false;		// テクスチャインデックスを使用するか
	bool m_bDisp = true;			// 描画フラグ
	std::function<void(LPDIRECT3DDEVICE9)> m_beforeDraw;	// 描画前の呼び出し関数
	std::function<void(LPDIRECT3DDEVICE9)> m_afterDraw;		// 描画後の呼び出し関数
};
#endif
