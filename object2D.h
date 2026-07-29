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

	static CObject2D *Create(const D3DXVECTOR3 &pos);
	static CObject2D *Create(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &rot);
	static CObject2D *Create(const D3DXVECTOR2 &size);
	static CObject2D *Create(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size);

	HRESULT Init(void);
	HRESULT Init(const D3DXVECTOR2 &size);
	HRESULT Init(const D3DXVECTOR3 &pos);
	HRESULT Init(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &rot);
	HRESULT Init(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size);
	HRESULT Init(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &rot, const D3DXVECTOR2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 &position);
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	void SetOffset(const D3DXVECTOR3 &offset);
	D3DXVECTOR3 GetOffset(void) const { return m_offset; }
	void SetRotation(const D3DXVECTOR3 &rotation);
	D3DXVECTOR3 GetRotation(void) const { return m_rot; }
	void SetPositionAndRotation(const D3DXVECTOR3 &position, const D3DXVECTOR3 &rotation);
	void SetSize(const D3DXVECTOR2 &size);
	D3DXVECTOR2 GetSize(void) const { return m_size; }
	void SetLength(const float fLength);
	float GetLength(void) const { return m_fLength; }
	void SetColor(const D3DXCOLOR &color);
	D3DXCOLOR GetColor(void) const { return m_color; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) { return m_bDisp; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	int m_nIdxTexture;		// テクスチャインデックス
	D3DXVECTOR3 m_pos;		// オブジェクトの位置
	D3DXVECTOR3 m_offset;	// オブジェクト描画時のオフセット
	D3DXVECTOR3 m_rot;		// オブジェクトの角度
	D3DXVECTOR2 m_size;		// オブジェクトのサイズ
	D3DXCOLOR m_color;		// 色
	float m_fLength;		// 対角線の長さ
	float m_fAngle;			// 対角線の角度
	bool m_bDisp;			// 描画の有無
};
#endif