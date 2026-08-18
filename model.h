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
		const Vector3 &pos,
		const Vector3 &rot);

	CModel();
	~CModel();

	HRESULT Init(const char *pXFileName,
		const Vector3 &pos,
		const Vector3 &rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetParent(CModel *pParent) { m_pParent = pParent; }
	Matrix GetMtxWorld(void) const { return m_mtxWorld; }
	void SetPosition(const Vector3 &position) { m_pos = position; }
	Vector3 GetPosition(void) const { return m_pos; }
	void SetRotation(const Vector3 &rotation) { m_rot = rotation; }
	Vector3 GetRotation(void) const { return m_rot; }
	void SetPositionLocal(const Vector3& position) { m_posLocal = position; }
	Vector3 GetPositionLocal(void) const { return m_posLocal; }
	void SetRotationLocal(const Vector3& rotation) { m_rotLocal = rotation; }
	Vector3 GetRotationLocal(void) const { return m_rotLocal; }

private:
	LPD3DXMESH m_pMesh;			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat;	// マテリアルへのポインタ
	int *m_pIdx;			// テクスチャインデックスの配列
	DWORD m_dwNumMat;		// マテリアルの数
	Vector3 m_pos;			// 位置
	Vector3 m_posLocal;		// オフセット座標
	Vector3 m_rot;			// 角度
	Vector3 m_rotLocal;		// オフセット角度
	Matrix m_mtxWorld;		// ワールドマトリックス
	CModel *m_pParent;		// 親モデルへのポインタ
};
#endif