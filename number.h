//==================================================================================
// 
// 数字表示クラスのヘッダーファイル [number.h]
// Author : TENMA SAITO
// Date   : 2026/5/11
// 
//==================================================================================
#ifndef _NUMBER_H_		// インクルードガード
#define _NUMBER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** 数値表示クラス ***
//**********************************************************************************
class CNumber
{
public:
	// 数字テクスチャの種類
	typedef enum
	{
		TYPE_VOLTNUM_000 = 0,	// 白文字に黄色のインライン
		TYPE_VOLTNUM_001,		// 黒文字に黄色のインライン
		TYPE_VOLTNUM_002,		// 黒文字に黄色のインライン (非立体)
		TYPE_VOLTNUM_003,		// 黒文字に黄色のインライン + 白のアウトライン
		TYPE_MAX
	} TYPE;

	CNumber();
	~CNumber();

	static CNumber *Create(const TYPE type, const Vector3 &pos, const Vector2 &size, const int nValue);

	HRESULT Init(const TYPE type, const Vector3 &pos, const Vector2 &size, const int nValue);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const Vector3 &position);
	const Vector3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const Vector3 &rotation);
	const Vector3 *GetRotation(void) const { return &m_rot; }
	void SetPositionAndRotation(const Vector3 &position, const Vector3 &rotation);
	void SetSize(const Vector2 &size);
	const Vector2 *GetSize(void) const { return &m_size; }
	void SetColor(const Color &col);
	void SetAlpha(const float fAlpha) { SetColor(Color(m_col.r, m_col.g, m_col.b, fAlpha)); }
	const Color *GetColor(void) const { return &m_col; }
	float GetLength(void) const { return m_fLength; }
	void SetNumber(const int nNumber);
	int GetNumber(void) const { return m_nNumber; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = nullptr;			// 頂点バッファ
	Vector3 m_pos = VECTOR3_NULL;		// オブジェクトの位置
	Vector3 m_rot = VECTOR3_NULL;		// オブジェクトの角度
	Vector2 m_size = VECTOR2_NULL;		// オブジェクトのサイズ
	Color m_col = COLOR_NULL;	// 色
	float m_fLength = 0.0f;		// 対角線の長さ
	float m_fAngle = 0.0f;		// 対角線の角度
	int m_nNumber = 0;			// 値
	int m_nIdxTexture = -1;		// テクスチャインデックス
	bool m_bDisp = true;		// 描画フラグ
};
#endif