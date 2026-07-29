//==================================================================================
// 
// メッシュフィールドクラスのヘッダーファイル [meshField.h]
// Author : TENMA SAITO
// Date   : 2026/7/7
// 
//==================================================================================
#ifndef _MESHSPHERE_H_
#define _MESHSPHERE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "mesh3D.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_MESHSPHERE_XBLOCK		(16)		// メッシュスフィアのデフォルトのX分割数
#define DEFAULT_MESHSPHERE_ZBLOCK		(16)		// メッシュスフィアのデフォルトのZ分割数

//**********************************************************************************
// *** メッシュスフィアクラス ***
//**********************************************************************************
class CMeshSphere : public CObject
{
public:
	CMeshSphere(const int nPriority = DEFAULT_OBJ_PRIORITY);
	~CMeshSphere();

	static CMeshSphere *Create(const D3DXVECTOR3 &pos,
		const float fRadius,
		const int nNumXBlock = DEFAULT_MESHSPHERE_XBLOCK,
		const int nNumZBlock = DEFAULT_MESHSPHERE_ZBLOCK,
		const D3DXVECTOR3 &vecQua = VECTOR3_NULL,
		const float fAngle = 0.0f);

	HRESULT Init(const D3DXVECTOR3 &pos,
		const float fRadius,
		const int nNumXBlock,
		const int nNumZBlock,
		const D3DXVECTOR3 &vecQua,
		const float fAngle);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetTop(VERTEX_3D *pVtx, WORD *pIdx);
	void SetMiddle(VERTEX_3D *pVtx, WORD *pIdx);
	void SetUnder(VERTEX_3D *pVtx, WORD *pIdx);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;		// インデックスバッファ
	int m_nNumXBlock;			// Xの分割数
	int m_nNumZBlock;			// Zの分割数
	int m_nNumVtx;				// 頂点数
	int m_nNumIdx;				// インデックス数
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_vecQua;		// 任意軸
	float m_fAngle;				// 回転度数
	float m_fRadius;			// 半径
	D3DXQUATERNION m_qua;		// クォータニオン
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
};
#endif
