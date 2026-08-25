//==================================================================================
// 
// メッシュオービット2Dクラスのヘッダーファイル [meshOrbit2D.h]
// Author : TENMA SAITO
// Date   : 2026/5/18
// 
//==================================================================================
#ifndef _MESHORBIT_2D_H_
#define _MESHORBIT_2D_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CMesh2D;

//**********************************************************************************
// *** メッシュオービット2Dクラス ***
//**********************************************************************************
class CMeshOrbit2D : public CObject
{
public:
	static CMeshOrbit2D *Create(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &rot, const int nNumOrbit, const float fWidth);

	CMeshOrbit2D(const int nPriority = 2);
	~CMeshOrbit2D();

	HRESULT Init(const int nNumOrbit, const float fWidth);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 &pos);
	void ResetPosition(const D3DXVECTOR3 &pos);
	void SetRotation(const D3DXVECTOR3 &rot);
	void ResetRotation(const D3DXVECTOR3 &rot);
	void SetWidth(const float fWidth);
	void SetColor(const Color &col);

private:
	CMesh2D *m_pMesh;		// メッシュ2Dへのポインタ
	D3DXVECTOR3 *m_pPos;	// 位置へのポインタ
	D3DXVECTOR3 m_rot;		// 角度
	float m_fWidth;			// オービットの幅
	int m_nNumPosition;		// 座標数
};
#endif