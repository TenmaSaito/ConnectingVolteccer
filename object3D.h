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
#define DEFAULT_OBJ3D_SIZE		Vector2(100.0f, 100.0f)		// obj3Dの基本サイズ

//**********************************************************************************
// *** オブジェクト3Dクラス ***
//**********************************************************************************
class CObject3D : public CObject
{
public:
	static CObject3D *Create(const bool bXYPlane,
		const Vector3 &pos = DEFAULT_OBJ3D_POS,
		const Vector3 &rot = DEFAULT_OBJ3D_ROT,
		const Vector2 &size = DEFAULT_OBJ3D_SIZE);

	CObject3D(const int nPriority = DEFAULT_OBJ3D_PRIORITY);
	~CObject3D();

	HRESULT Init(const bool bXYPlane, const Vector3 &pos, const Vector3 &rot, const Vector2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetPosition(const Vector3 &position);
	const Vector3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const Vector3 &rotation);
	const Vector3 *SetRotation(void) const { return &m_rot; }
	void SetSize(const Vector2 &size);
	const Vector2 *GetSize(void) const { return &m_size; }
	bool GetDirty(void) const { return m_bDirty; }
	float GetHeight(const Vector3 &pos);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	int m_nIdxTexture;		// テクスチャのインデックス
	Matrix m_mtxWorld;		// ワールドマトリックス
	Vector3 m_pos;			// 位置
	Vector3 m_rot;			// 角度
	Vector2 m_size;			// サイズ
	Vector3 m_aVtx[4];		// 各頂点座標
	bool m_bDirty;			// マトリックスの更新フラグ
	bool m_bXYPlane;		// XY平面フラグ
};
#endif