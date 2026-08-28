//==================================================================================
// 
// ポリゴン3Dクラスのヘッダーファイル [poplygon3D.h]
// Author : TENMA SAITO
// Date   : 2026/8/28
// 
//==================================================================================
#ifndef _POLYGON_3D_H_		// インクルードガード
#define _POLYGON_3D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <functional>
#include <utility>

//**********************************************************************************
// *** ポリゴン3Dクラス ***
//**********************************************************************************
class CPolygon3D
{
public:
	CPolygon3D();
	~CPolygon3D();

	static CPolygon3D *Create(const Vector3 &pos, const Vector3 &rot, const Vector2 &size);

	HRESULT Init(const Vector3 &pos, const Vector3 &rot, const Vector2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const Vector3 &pos) { m_pos = pos; }
	const Vector3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const Vector3 &rot);
	const Vector3 *GetRotation(void) const { return &m_rot; }
	void SetQuaternion(const Quaternion &qua);
	const Quaternion *GetQuaternion(void) const { return &m_qua; }
	void SetQuaternionRotationAxis(const Vector3 &vec, const float fAngle);
	std::pair<Vector3, float> GetQuaternionRotationAxis(void) const;
	void SetSize(const Vector2 &size);
	const Vector2 *GetSize(void) const { return &m_size; }
	void SetColor(const Color &color);
	const Color *GetColor(void) const { return &m_col; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	void SetStateFunctionBeforeDraw(std::function<void(LPDIRECT3DDEVICE9)> before) { m_beforeDraw = before; }
	void SetStateFunctionAfterDraw(std::function<void(LPDIRECT3DDEVICE9)> after) { m_afterDraw = after; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	int m_nIdxTexture;			// テクスチャインデックス
	Vector3 m_pos;				// 中心座標
	Vector3 m_rot;				// 角度
	Quaternion m_qua;			// クォータニオン
	Vector2 m_size;				// サイズ
	Color m_col;				// 色
	Matrix m_mtxWorld;			// ワールドマトリックス
	const Matrix *m_pMtxParent;	// 親マトリックスへのポインタ
	bool m_bDisp;				// 描画するか
	std::function<void(LPDIRECT3DDEVICE9)> m_beforeDraw;	// Draw関数前の呼び出し関数
	std::function<void(LPDIRECT3DDEVICE9)> m_afterDraw;		// Draw関数後の呼び出し関数
};
#endif