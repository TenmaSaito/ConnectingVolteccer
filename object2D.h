//==================================================================================
// 
// 2Dオブジェクトクラスのヘッダーファイル [object2D.h]
// Author : TENMA SAITO
// Date   : 2026/5/11
// 
//==================================================================================
#ifndef _OBJECT2D_H_		// インクルードガード
#define _OBJECT2D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_OBJ2D_PRIORITY		(6)		// オブジェクト2Dの基本優先順位

//**********************************************************************************
// *** オブジェクト2Dクラス ***
//**********************************************************************************
class CObject2D : public CObject
{
public:
	CObject2D(const int nPriority = DEFAULT_OBJ2D_PRIORITY);
	~CObject2D();

	static CObject2D *Create(const Vector3 &pos);
	static CObject2D *Create(const Vector3 &pos, const Vector3 &rot);
	static CObject2D *Create(const Vector2 &size);
	static CObject2D *Create(const Vector3 &pos, const Vector2 &size);

	HRESULT Init(void);
	HRESULT Init(const Vector2 &size);
	HRESULT Init(const Vector3 &pos);
	HRESULT Init(const Vector3 &pos, const Vector3 &rot);
	HRESULT Init(const Vector3 &pos, const Vector2 &size);
	HRESULT Init(const Vector3 &pos, const Vector3 &rot, const Vector2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const Vector3 &position);
	Vector3 GetPosition(void) const { return m_pos; }
	void SetOffset(const Vector3 &offset);
	Vector3 GetOffset(void) const { return m_offset; }
	void SetRotation(const Vector3 &rotation);
	Vector3 GetRotation(void) const { return m_rot; }
	void SetPositionAndRotation(const Vector3 &position, const Vector3 &rotation);
	void SetSize(const Vector2 &size);
	Vector2 GetSize(void) const { return m_size; }
	void SetLength(const float fLength);
	float GetLength(void) const { return m_fLength; }
	void SetColor(const Color &color);
	Color GetColor(void) const { return m_color; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) { return m_bDisp; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	int m_nIdxTexture;		// テクスチャインデックス
	Vector3 m_pos;			// オブジェクトの位置
	Vector3 m_offset;		// オブジェクト描画時のオフセット
	Vector3 m_rot;			// オブジェクトの角度
	Vector2 m_size;			// オブジェクトのサイズ
	Color m_color;			// 色
	float m_fLength;		// 対角線の長さ
	float m_fAngle;			// 対角線の角度
	bool m_bDisp;			// 描画の有無
};
#endif