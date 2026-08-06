//==================================================================================
// 
// D3DXMATRIXの計算関連関数をまとめたヘッダーファイル [matrix.h]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
#ifndef _MATRIX_H_
#define _MATRIX_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** D3DXMATRIX関連関数名前空間 ***
//**********************************************************************************
namespace Mtx
{
	//******************************************************************************
	// *** プロトタイプ宣言 ***
	//******************************************************************************
	D3DXMATRIX *CalcPosition(D3DXMATRIX *pMtx, const D3DXVECTOR3 &position);
	D3DXMATRIX *CalcRotation(D3DXMATRIX *pMtx, const D3DXVECTOR3 &rotation);
	D3DXMATRIX *CalcRotation(D3DXMATRIX *pMtx, const D3DXQUATERNION &quaternion);
	D3DXMATRIX *CalcScale(D3DXMATRIX *pMtx, const D3DXVECTOR3 &scale);

	D3DXMATRIX *CalcWorld(D3DXMATRIX *pMtx, 
		const D3DXVECTOR3 &position, 
		const D3DXVECTOR3 &rotation);

	D3DXMATRIX *CalcWorld(D3DXMATRIX *pMtx, 
		const D3DXVECTOR3 &position, 
		const D3DXQUATERNION &quaternion);

	D3DXMATRIX *CalcWorld(D3DXMATRIX *pMtx,
		const D3DXMATRIX *pParent, 
		const D3DXVECTOR3 &position,
		const D3DXVECTOR3 &rotation);

	D3DXMATRIX *CalcWorld(D3DXMATRIX *pMtx,
		const D3DXMATRIX *pParent, 
		const D3DXVECTOR3 &position,
		const D3DXQUATERNION &quaternion);

	D3DXMATRIX *CalcWorld(D3DXMATRIX *pMtx,
		const D3DXMATRIX *pParent,
		const D3DXVECTOR3 &scale,
		const D3DXVECTOR3 &position,
		const D3DXQUATERNION &quaternion);

	D3DXMATRIX *CreateShadow(const D3DXMATRIX *pMtxWorld,
		const D3DXVECTOR3 &pos, 
		const D3DXVECTOR3 &nor,
		const D3DLIGHT9 &light,
		D3DXMATRIX *pOut);
}
#endif