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
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_ALPHA		(-1.0f)		// α値適用の取り消し値

//**********************************************************************************
// *** Xモデル(クォータニオン仕様)クラス ***
//**********************************************************************************
class CObjectXQuaternion : public CObject
{
public:
	static CObjectXQuaternion *Create(const char *pXFileName,
		const Vector3 &pos,
		const Vector3 &vecQua,
		const float fAngle);

	CObjectXQuaternion(const int nPriority = DEFAULT_OBJ_PRIORITY);
	~CObjectXQuaternion();

	HRESULT Init(const char *pXFileName, const Vector3 &pos, const Vector3 &vecQua, const float fAngle);
	HRESULT Init(const char *pXFileName, const Vector3 &pos, const Quaternion &qua);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const Vector3 &position) { m_pos = position; }
	const Vector3 *GetPosition(void) const { return &m_pos; }
	Vector3 GetWorldPosition(void);
	void SetScale(const Vector3 &scale) { m_scale = scale; }
	const Vector3 *GetScale(void) const { return &m_scale; }
	void SetVecQua(const Vector3 &vecQua) { m_vecQua = vecQua; }
	const Vector3 *GetVecQua(void) const { return &m_vecQua; }
	void SetAngle(const float fAngle);
	void AddAngle(const float fAngle);
	float GetAngle(void) const { return m_fAngle; }
	void SetMaterial(const size_t pos, const D3DMATERIAL9 &mat) { m_vMat.at(pos) = mat; }
	const D3DMATERIAL9 *GetMaterial(const size_t pos) const { return &m_vMat.at(pos); }
	void SetAlpha(const float fAlpha) { m_fAlpha = fAlpha; }
	float GetAlpha(void) const { return m_fAlpha; }
	const Vector3 *GetVtxMin(void) const { return &m_vtxMin; }
	const Vector3 *GetVtxMax(void) const { return &m_vtxMax; }
	const Matrix *GetMatrix(void) const { return &m_mtxWorld; }
	const Matrix *CalcMatrix(void);
	Matrix *CalcMatrixUnaffect(Matrix *pOut) const;
	void SetParent(const Matrix *pParent) { m_pMtxParent = pParent; }
	const Matrix *GetParent(void) const { return m_pMtxParent; }
	const char *GetFileName(void) const { return m_aFileName; }
	void CaluQuaternion(void);
	const Quaternion *GetQuaternion(void) const { return &m_qua; }
	bool IsHitByRay(const Vector3 &start, const Vector3 &vec, const float fLength);
	bool IsHitByPlayerCamRay(void);

protected:
	Quaternion *GetQuaternionPtr(void) { return &m_qua; }

private:
	HRESULT	LoadXFile(const char *pXFileName);

	LPD3DXMESH m_pMesh;				// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat;		// マテリアルへのポインタ
	std::vector<int> m_vIdx;		// テクスチャインデックスの配列
	DWORD m_dwNumMat;				// マテリアルの数
	Vector3 m_vtxMin, m_vtxMax;		// モデルの各最大最小頂点の位置
	Vector3 m_pos;					// 位置
	Vector3 m_scale;				// サイズ
	Vector3 m_vecQua;				// 任意軸
	float m_fAngle;					// 軸回転
	Quaternion m_qua;				// クォータニオン
	Matrix m_mtxWorld;				// ワールドマトリックス
	const Matrix *m_pMtxParent;		// 親のワールドマトリックス
	std::vector<D3DMATERIAL9> m_vMat;	// モデルのマテリアル色
	float m_fAlpha;					// α値
	char m_aFileName[MAX_PATH];		// ファイル名
	bool m_bCalcMatrix;				// マトリックスが一度でも計算されたかを示すフラグ
};
#endif