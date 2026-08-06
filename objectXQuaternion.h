//==================================================================================
// 
// Xモデル(クォータニオン仕様)クラスのヘッダーファイル [objectXQuaternion.h]
// Author : TENMA SAITO
// Date   : 2026/7/3
// 
//==================================================================================
#ifndef _OBJECTXQUATERNION_H_		// インクルードガード
#define _OBJECTXQUATERNION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** Xモデル(クォータニオン仕様)クラス ***
//**********************************************************************************
class CObjectXQuaternion : public CObject
{
public:
	static CObjectXQuaternion *Create(const char *pXFileName,
		const D3DXVECTOR3 &pos,
		const D3DXVECTOR3 &vecQua,
		const float fAngle);

	CObjectXQuaternion(const int nPriority = DEFAULT_OBJ_PRIORITY);
	~CObjectXQuaternion();

	HRESULT Init(const char *pXFileName, const D3DXVECTOR3 &pos, const D3DXVECTOR3 &vecQua, const float fAngle);
	HRESULT Init(const char *pXFileName, const D3DXVECTOR3 &pos, const D3DXQUATERNION &qua);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 &position) { m_pos = position; }
	const D3DXVECTOR3 *GetPosition(void) const { return &m_pos; }
	D3DXVECTOR3 GetWorldPosition(void);
	void SetScale(const D3DXVECTOR3 &scale) { m_scale = scale; }
	const D3DXVECTOR3 *GetScale(void) const { return &m_scale; }
	void SetVecQua(const D3DXVECTOR3 &vecQua) { m_vecQua = vecQua; }
	const D3DXVECTOR3 *GetVecQua(void) const { return &m_vecQua; }
	void SetAngle(const float fAngle);
	void AddAngle(const float fAngle);
	float GetAngle(void) const { return m_fAngle; }
	const D3DXVECTOR3 *GetVtxMin(void) const { return &m_vtxMin; }
	const D3DXVECTOR3 *GetVtxMax(void) const { return &m_vtxMax; }
	const D3DXMATRIX *GetMatrix(void) const { return &m_mtxWorld; }
	void SetParent(const D3DXMATRIX *pParent) { m_pMtxParent = pParent; }
	const D3DXMATRIX *GetParent(void) const { return m_pMtxParent; }
	const char *GetFileName(void) const { return m_aFileName; }
	void CaluQuaternion(void);
	const D3DXQUATERNION *GetQuaternion(void) const { return &m_qua; }

protected:
	D3DXQUATERNION *GetQuaternionPtr(void) { return &m_qua; }

private:
	HRESULT	LoadXFile(const char *pXFileName);

	LPD3DXMESH m_pMesh;				// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat;		// マテリアルへのポインタ
	int *m_pIdx;					// テクスチャインデックスの配列
	DWORD m_dwNumMat;				// マテリアルの数
	D3DXVECTOR3 m_vtxMin, m_vtxMax;	// モデルの各最大最小頂点の位置
	D3DXVECTOR3 m_pos;				// 位置
	D3DXVECTOR3 m_scale;			// サイズ
	D3DXVECTOR3 m_vecQua;			// 任意軸
	float m_fAngle;					// 軸回転
	D3DXQUATERNION m_qua;			// クォータニオン
	D3DXMATRIX m_mtxWorld;			// ワールドマトリックス
	const D3DXMATRIX *m_pMtxParent;	// 親のワールドマトリックス
	char m_aFileName[MAX_PATH];		// ファイル名
	bool m_bHitByPlayerCamRay;		// プレイヤーカメラとのレイ判定
	bool m_bCalcMatrix;				// マトリックスが一度でも計算されたかを示すフラグ
};
#endif