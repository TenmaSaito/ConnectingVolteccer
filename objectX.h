//==================================================================================
// 
// オブジェクトXクラスのヘッダーファイル [objectX.h]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
#ifndef _OBJECTX_H_		// インクルードガード
#define _OBJECTX_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_OBJX_PRIORITY	DEFAULT_OBJ_PRIORITY			// objXの基本優先順位
#define DEFAULT_OBJX_POS		VECTOR3_NULL					// objXの基本位置
#define DEFAULT_OBJX_ROT		VECTOR3_NULL					// objXの基本角度
#define MATERIAL_MAXNUM			(16)							// マテリアルの最大数

//**********************************************************************************
// *** オブジェクトXクラス ***
//**********************************************************************************
class CObjectX : public CObject
{
public:
	static CObjectX *Create(const char *pXFileName,
		const D3DXVECTOR3 &pos = DEFAULT_OBJX_POS,
		const D3DXVECTOR3 &rot = DEFAULT_OBJX_ROT);

	CObjectX(const int nPriority = DEFAULT_OBJX_PRIORITY);
	~CObjectX();

	HRESULT Init(const char *pXFileName, const D3DXVECTOR3 &pos, const D3DXVECTOR3 &rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 &position) { m_pos = position; }
	const D3DXVECTOR3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const D3DXVECTOR3 &rotation) { m_rot = rotation; }
	const D3DXVECTOR3 *GetRotation(void) const { return &m_rot; }
	const D3DXVECTOR3 *GetVtxMin(void) const { return &m_vtxMin; }
	const D3DXVECTOR3 *GetVtxMax(void) const { return &m_vtxMax; }
	const char *GetFileName(void) const { return m_aFileName; }

private:
	HRESULT	LoadXFile(const char *pXFileName);

	LPD3DXMESH m_pMesh;			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat;	// マテリアルへのポインタ
	int *m_pIdx;				// テクスチャインデックスの配列
	DWORD m_dwNumMat;			// マテリアルの数
	D3DXVECTOR3 m_vtxMin, m_vtxMax;	// モデルの各最大最小頂点の位置
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 角度
	char m_aFileName[MAX_PATH];	// ファイル名
};
#endif