//==================================================================================
// 
// オブジェクトラインクラスのヘッダーファイル [objectLine.h]
// Author : TENMA SAITO
// Date   : 2026/7/27
// 
//==================================================================================
#ifndef _OBJECTLINE_H_		// インクルードガード
#define _OBJECTLINE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
// *** オブジェクトラインクラス ***
//**********************************************************************************
class CObjectLine : public CObject
{
public:
	CObjectLine(const int nPriority = 6);
	~CObjectLine();

	static CObjectLine *Create(const Vector3 &start, const Vector3 &end);
	static CObjectLine *Create(const Vector3 &origin, const Vector3 &vec, const float fLength);

	HRESULT Init(const Vector3 &p1, const Vector3 &p2);
	HRESULT Init(const Vector3 &origin, const Vector3 &vec, const float fLength);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPoint(const Vector3 &start, const Vector3 &end);
	const Vector3 *GetStart(void) const { return &m_start; }
	const Vector3 *GetEnd(void) const { return &m_end; }
	void SetVector(const Vector3 &origin, const Vector3 &vec, const float fLength);
	const Vector3 *GetOrigin(void) const { return &m_start; }
	const Vector3 *GetVector(void) const { return &m_vec; }
	float GetLength(void) const { return m_fLength; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	void SetLife(const int nLife) { m_nLife = nLife; }
	int GetLife(void) const { return m_nLife; }

private:
	HRESULT CreateVertex(void);
	void UpdateVertex(void);

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	Vector3 m_start, m_end;			// 始点、終点
	Vector3 m_vec;			// 始点と方向ベクトル
	float m_fLength;		// ベクトルの長さ
	Color m_color;			// 色
	int m_nLife;			// 寿命
	bool m_bDisp;			// 描画するか
};
#endif
