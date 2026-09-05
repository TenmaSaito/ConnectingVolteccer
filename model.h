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
// *** マクロ定義 ***
//**********************************************************************************
#define INVALID_MAT		D3DMATERIAL9{0}

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
	const CModel *GetParent(void) const { return m_pParent; }
	const Matrix *GetMtxWorld(void) const { return &m_mtxWorld; }
	void SetPosition(const Vector3 &position) { m_pos = position; }
	const Vector3 *GetPosition(void) const { return &m_pos; }
	void SetRotation(const Vector3 &rotation) { m_rot = rotation; }
	const Vector3 *GetRotation(void) const { return &m_rot; }
	void SetPositionLocal(const Vector3& position) { m_posLocal = position; }
	const Vector3 *GetPositionLocal(void) const { return &m_posLocal; }
	void SetRotationLocal(const Vector3& rotation) { m_rotLocal = rotation; }
	const Vector3 *GetRotationLocal(void) const { return &m_rotLocal; }
	const char *GetFileName(void) const { return m_sFileName.c_str(); }
	void SetCustomMat(const D3DMATERIAL9 &customMat = INVALID_MAT) { m_customMat = customMat; }
	CModel *CreateCopy(void) const;

private:
	LPD3DXMESH m_pMesh;			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat;	// マテリアルへのポインタ
	std::string m_sFileName;	// ファイル名
	std::vector<int> m_vIdx;	// テクスチャインデックスの配列
	DWORD m_dwNumMat;		// マテリアルの数
	Vector3 m_pos;			// 位置
	Vector3 m_posLocal;		// オフセット座標
	Vector3 m_rot;			// 角度
	Vector3 m_rotLocal;		// オフセット角度
	Matrix m_mtxWorld;		// ワールドマトリックス
	CModel *m_pParent;		// 親モデルへのポインタ
	D3DMATERIAL9 m_customMat;	// カスタムマテリアル
};
#endif