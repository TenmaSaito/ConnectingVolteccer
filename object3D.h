//==================================================================================
// 
// オブジェクト3Dクラスのヘッダーファイル [object3D.h]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
#ifndef _OBJECT3D_H_		// インクルードガード
#define _OBJECT3D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_OBJ3D_PRIORITY	DEFAULT_OBJ_PRIORITY			// obj3Dの基本優先順位
#define DEFAULT_OBJ3D_POS		VECTOR3_NULL					// obj3dの基本位置
#define DEFAULT_OBJ3D_ROT		VECTOR3_NULL					// obj3Dの基本角度
#define DEFAULT_OBJ3D_SIZE		D3DXVECTOR2(100.0f, 100.0f)		// obj3Dの基本サイズ

//**********************************************************************************
// *** オブジェクト3Dクラス ***
//**********************************************************************************
class CObject3D : public CObject
{
public:
	static CObject3D *Create(const bool bXYPlane,
		const D3DXVECTOR3 &pos = DEFAULT_OBJ3D_POS,
		const D3DXVECTOR3 &rot = DEFAULT_OBJ3D_ROT,
		const D3DXVECTOR2 &size = DEFAULT_OBJ3D_SIZE);

	CObject3D(const int nPriority = DEFAULT_OBJ3D_PRIORITY);
	~CObject3D();

	HRESULT Init(const bool bXYPlane, const D3DXVECTOR3 &pos, const D3DXVECTOR3 &rot, const D3DXVECTOR2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetPosition(const D3DXVECTOR3 &position);
	const D3DXVECTOR3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const D3DXVECTOR3 &rotation);
	const D3DXVECTOR3 *SetRotation(void) const { return &m_rot; }
	void SetSize(const D3DXVECTOR2 &size);
	const D3DXVECTOR2 *GetSize(void) const { return &m_size; }
	bool GetDirty(void) const { return m_bDirty; }
	float GetHeight(const D3DXVECTOR3 &pos);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	int m_nIdxTexture;					// テクスチャのインデックス
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	D3DXVECTOR3 m_pos;					// 位置
	D3DXVECTOR3 m_rot;					// 角度
	D3DXVECTOR2 m_size;					// サイズ
	D3DXVECTOR3 m_aVtx[4];				// 各頂点座標
	bool m_bDirty;						// マトリックスの更新フラグ
	bool m_bXYPlane;					// XY平面フラグ
};
#endif