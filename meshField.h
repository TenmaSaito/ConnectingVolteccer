//==================================================================================
// 
// メッシュフィールドクラスのヘッダーファイル [meshField.h]
// Author : TENMA SAITO
// Date   : 2026/6/10
// 
//==================================================================================
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "mesh3D.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_MESHFIELD_XBLOCK		(2)		// メッシュフィールドのデフォルトのX分割数
#define DEFAULT_MESHFIELD_ZBLOCK		(2)		// メッシュフィールドのデフォルトのZ分割数

//**********************************************************************************
// *** メッシュフィールド ***
//**********************************************************************************
class CMeshField : public CMesh3D
{
public:
	static CMeshField *Create(const D3DXVECTOR3 &pos, 
		const D3DXVECTOR3 &rot,
		const D3DXVECTOR2 &size,
		const int nNumXBlock = DEFAULT_MESHFIELD_XBLOCK,
		const int nNumZBlock = DEFAULT_MESHFIELD_ZBLOCK);

	CMeshField(const int nPriority = DEFAULT_OBJ_PRIORITY);
	~CMeshField();

	HRESULT Init(const D3DXVECTOR3 &pos, 
		const D3DXVECTOR3 &rot,
		const D3DXVECTOR2 &size,
		const int nNumXBlock,
		const int nNumZBlock);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	float GetHeight(const D3DXVECTOR3 &pos);
	void SetHeight(const D3DXVECTOR3 &pos, const float fWidth, const float fPower);

private:
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 角度
	D3DXVECTOR2 m_size;			// サイズ
	int m_nNumXBlock;			// X分割数
	int m_nNumZBlock;			// Z分割数
	int m_aIdxFirstBlock[4];	// 最初のブロックのインデックス
	int m_nIdxControl;			// 操作している頂点インデックス
	D3DXMATRIX m_mtxWorld;		// マトリックス
};
#endif