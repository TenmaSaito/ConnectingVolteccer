//==================================================================================
// 
// Matrixの計算関連関数をまとめたソースファイル [matrix.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "matrix.h"

//==================================================================================
// --- 位置によるマトリックス計算処理 ---
//==================================================================================
Matrix *Mtx::CalcPosition(Matrix *pMtx, const Vector3 &position)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	Matrix MtxTrans;		// 計算用変数

	// マトリックス計算
	D3DXMatrixTranslation(&MtxTrans,
		position.x,
		position.y,
		position.z);

	// マトリックスを掛け合わせる
	D3DXMatrixMultiply(pMtx, pMtx, &MtxTrans);

	// 計算後のマトリックスへのポインタを返す
	return pMtx;
}

//==================================================================================
// --- 角度によるマトリックス計算処理 ---
//==================================================================================
Matrix *Mtx::CalcRotation(Matrix *pMtx, const Vector3 &rotation)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	Matrix MtxRot;		// 計算用変数

	// マトリックス計算
	D3DXMatrixRotationYawPitchRoll(&MtxRot,
		rotation.y,
		rotation.x,
		rotation.z);

	// マトリックスを掛け合わせる
	D3DXMatrixMultiply(pMtx, pMtx, &MtxRot);

	// 計算後のマトリックスへのポインタを返す
	return pMtx;
}

//==================================================================================
// --- クォータニオンによるマトリックス計算処理 ---
//==================================================================================
Matrix *Mtx::CalcRotation(Matrix *pMtx, const Quaternion &quaternion)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	Matrix MtxQua;		// 計算用変数

	// マトリックス計算
	D3DXMatrixRotationQuaternion(&MtxQua, &quaternion);

	// マトリックスを掛け合わせる
	D3DXMatrixMultiply(pMtx, pMtx, &MtxQua);

	// 計算後のマトリックスへのポインタを返す
	return pMtx;
}

//==================================================================================
// --- スケーリングによるマトリックス計算処理 ---
//==================================================================================
Matrix *Mtx::CalcScale(Matrix *pMtx, const Vector3 &scale)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	Matrix MtxScale;		// 計算用変数

	// マトリックス計算
	D3DXMatrixScaling(&MtxScale, scale.x, scale.y, scale.z);

	// マトリックスを掛け合わせる
	D3DXMatrixMultiply(pMtx, pMtx, &MtxScale);

	return pMtx;
}

//==================================================================================
// --- ワールドマトリックスの計算処理 ---
//==================================================================================
Matrix *Mtx::CalcWorld(Matrix *pMtx, const Vector3 &position, const Vector3 &rotation)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	// 角度のマトリックス計算
	CalcRotation(pMtx, rotation);

	// 位置のマトリックス計算
	CalcPosition(pMtx, position);

	// 計算後のマトリックスへのポインタを返す
	return pMtx;
}

//==================================================================================
// --- ワールドマトリックスの計算処理 (クォータニオン) ---
//==================================================================================
Matrix *Mtx::CalcWorld(Matrix *pMtx, const Vector3 &position, const Quaternion &quaternion)
{ // ※クォータニオンの場合、先にオフセットのマトリックスを生成する
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	// 位置のマトリックス計算
	CalcPosition(pMtx, position);

	// 角度のマトリックス計算
	CalcRotation(pMtx, quaternion);

	// 計算後のマトリックスへのポインタを返す
	return pMtx;
}

//==================================================================================
// --- 親マトリックスを持つワールドマトリックスの計算処理 ---
//==================================================================================
Matrix *Mtx::CalcWorld(Matrix *pMtx,
	const Matrix *pParent,
	const Vector3 &position,
	const Vector3 &rotation)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	// 角度のマトリックス計算
	CalcRotation(pMtx, rotation);

	// 位置のマトリックス計算
	CalcPosition(pMtx, position);

	if (pParent != nullptr)
	{ // 親マトリックスへのポインタがnullptrではない場合
		// 親マトリックスと掛け合わせる
		D3DXMatrixMultiply(pMtx, pMtx, pParent);
	}

	// 計算後のマトリックスへのポインタを返す
	return pMtx;
}

//==================================================================================
// --- 親マトリックスを持つワールドマトリックスの計算処理 (クォータニオン) ---
//==================================================================================
Matrix *Mtx::CalcWorld(Matrix *pMtx,
	const Matrix *pParent,
	const Vector3 &position,
	const Quaternion &quaternion)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	// 位置のマトリックス計算
	CalcPosition(pMtx, position);

	// 角度のマトリックス計算
	CalcRotation(pMtx, quaternion);

	if (pParent != nullptr)
	{ // 親マトリックスへのポインタがnullptrではない場合
		// 親マトリックスと掛け合わせる
		D3DXMatrixMultiply(pMtx, pMtx, pParent);
	}

	// 計算後のマトリックスへのポインタを返す
	return pMtx;
}

//==================================================================================
// --- 親マトリックスを持つワールドマトリックスの計算処理 (Scaling + Quaternion) ---
//==================================================================================
Matrix *Mtx::CalcWorld(Matrix *pMtx,
	const Matrix *pParent,
	const Vector3 &scale,
	const Vector3 &position,
	const Quaternion &quaternion)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	// スケーリングのマトリックス計算
	CalcScale(pMtx, scale);

	// 位置のマトリックス計算
	CalcPosition(pMtx, position);

	// 角度のマトリックス計算
	CalcRotation(pMtx, quaternion);

	if (pParent != nullptr)
	{ // 親マトリックスへのポインタがnullptrではない場合
		// 親マトリックスと掛け合わせる
		D3DXMatrixMultiply(pMtx, pMtx, pParent);
	}

	// 計算後のマトリックスへのポインタを返す
	return pMtx;
}

//==================================================================================
// --- シャドウマトリックスの作成処理 ---
//==================================================================================
Matrix *Mtx::CreateShadow(const Matrix *pMtxWorld,
	const Vector3 &pos,
	const Vector3 &nor,
	const D3DLIGHT9 &light,
	Matrix *pOut)
{
	// ワールドマトリックスがnullptrならnullptrを返す
	if (pMtxWorld == nullptr) return nullptr;

	// 出力先がnullptrならnullptrを返す
	if (pOut == nullptr) return nullptr;

	D3DXVECTOR4 lightPos;		// ライトの位置
	D3DXPLANE plane;			// 平面

	// ライトの方向ベクトルの逆を取得
	lightPos = D3DXVECTOR4(-light.Direction.x, -light.Direction.y, -light.Direction.z, 0.0f);

	// 平面作成
	D3DXPlaneFromPointNormal(&plane, &pos, &nor);

	// シャドウマトリックスの初期化
	D3DXMatrixIdentity(pOut);

	// シャドウマトリックスの作成
	D3DXMatrixShadow(pOut, &lightPos, &plane);
	D3DXMatrixMultiply(pOut, pMtxWorld, pOut);

	// 作成したシャドウマトリックスを返す
	return pOut;
}