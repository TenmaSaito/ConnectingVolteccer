//==================================================================================
// 
// モデルクラスのヘッダーファイル [model.h]
// Author : TENMA SAITO
// Date   : 2026/6/2
// 
//==================================================================================
#ifndef _MODEL_H_		// インクルードガード
#define _MODEL_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** モデルクラス ***
//**********************************************************************************
class CModel
{
public:
	static CModel *Create(const char *pXFileName,
		const D3DXVECTOR3 &pos,
		const D3DXVECTOR3 &rot);

	CModel();
	~CModel();

	HRESULT Init(const char *pXFileName,
		const D3DXVECTOR3 &pos,
		const D3DXVECTOR3 &rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(CModel *pParent) { m_pParent = pParent; }
	D3DXMATRIX GetMtxWorld(void) const { return m_mtxWorld; }
	void SetPosition(const D3DXVECTOR3 &position) { m_pos = position; }
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	void SetRotation(const D3DXVECTOR3 &rotation) { m_rot = rotation; }
	D3DXVECTOR3 GetRotation(void) const { return m_rot; }
	void SetPositionLocal(const D3DXVECTOR3& position) { m_posLocal = position; }
	D3DXVECTOR3 GetPositionLocal(void) const { return m_posLocal; }
	void SetRotationLocal(const D3DXVECTOR3& rotation) { m_rotLocal = rotation; }
	D3DXVECTOR3 GetRotationLocal(void) const { return m_rotLocal; }

private:
	LPD3DXMESH m_pMesh;			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat;	// マテリアルへのポインタ
	int *m_pIdx;			// テクスチャインデックスの配列
	DWORD m_dwNumMat;		// マテリアルの数
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_posLocal;	// オフセット座標
	D3DXVECTOR3 m_rot;		// 角度
	D3DXVECTOR3 m_rotLocal;	// オフセット角度
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	CModel *m_pParent;		// 親モデルへのポインタ
};
#endif