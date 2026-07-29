//==================================================================================
// 
// メッシュクラスのヘッダーファイル [mesh.h]
// Author : TENMA SAITO
// Date   : 2026/5/30
// 
//==================================================================================
#ifndef _MESH_H_
#define _MESH_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
// *** メッシュクラス ***
//**********************************************************************************
class CMesh : public CObject
{
public:
	CMesh(const int nPriority = DEFAULT_OBJ_PRIORITY);
	virtual ~CMesh();

	virtual HRESULT Init(const int nNumVtx, const int nNumIdx, const int nNumPrim, const size_t vtxSize, const DWORD nFlags);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	template<class VERTEX> HRESULT LockVertex(VERTEX **pVtx);
	void UnlockVertex(void) { m_pVtxBuff->Unlock(); }
	HRESULT LockIndex(WORD **pIdx) { return m_pIdxBuff->Lock(0, 0, (void**)pIdx, 0); }
	void UnlockIndex(void) { m_pIdxBuff->Unlock(); }
	void BindTexture(const int nIdx) { m_nIdxTexture = nIdx; }
	int GetVertexNum(void) const { return m_nNumVtx; }
	int GetIndexNum(void) const { return m_nNumIdx; }
	int GetPrimNum(void) const { return m_nNumPrim; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;		// インデックスバッファへのポインタ
	int m_nIdxTexture;		// テクスチャインデックス
	int m_nNumVtx;			// 頂点数
	int m_nNumIdx;			// インデックス数
	int m_nNumPrim;			// 描画する三角ポリゴンの数
	size_t m_vtxSize;		// 頂点情報構造体のサイズ
	DWORD m_dwFlags;		// 頂点フラグ
};

//==================================================================================
// --- 頂点バッファの取得及びロック ---
//==================================================================================
template<class VERTEX> HRESULT CMesh::LockVertex(VERTEX **pVtx)
{ 
	return m_pVtxBuff->Lock(0, 0, (void**)pVtx, 0);
}
#endif