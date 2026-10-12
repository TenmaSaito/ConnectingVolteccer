//==================================================================================
// 
// Matrixの計算関連関数をまとめたヘッダーファイル [matrix.h]
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
// *** Matrix関連関数名前空間 ***
//**********************************************************************************
namespace Mtx
{
	//******************************************************************************
	// *** プロトタイプ宣言 ***
	//******************************************************************************
	__forceinline Matrix *Identity(Matrix *pMtx) { return D3DXMatrixIdentity(pMtx); }
	__forceinline Matrix *Inverse(Matrix *pMtx, const Matrix &mtx) { return D3DXMatrixInverse(pMtx, nullptr, &mtx); }
	Matrix *CalcPosition(Matrix *pMtx, const Vector3 &position);
	Matrix *CalcRotation(Matrix *pMtx, const Vector3 &rotation);
	Matrix *CalcRotation(Matrix *pMtx, const Quaternion &quaternion);
	Matrix *CalcScale(Matrix *pMtx, const Vector3 &scale);

	Matrix *CalcWorld(Matrix *pMtx, 
		const Vector3 &position, 
		const Vector3 &rotation);

	Matrix *CalcWorld(Matrix *pMtx, 
		const Vector3 &position, 
		const Quaternion &quaternion);

	Matrix *CalcWorld(Matrix *pMtx,
		const Matrix *pParent, 
		const Vector3 &position,
		const Vector3 &rotation);

	Matrix *CalcWorld(Matrix *pMtx,
		const Matrix *pParent, 
		const Vector3 &position,
		const Quaternion &quaternion);

	Matrix *CalcWorld(Matrix *pMtx,
		const Matrix *pParent,
		const Vector3 &scale,
		const Vector3 &position,
		const Quaternion &quaternion);

	Matrix *CreateShadow(const Matrix *pMtxWorld,
		const Vector3 &pos, 
		const Vector3 &nor,
		const D3DLIGHT9 &light,
		Matrix *pOut);
}
#endif