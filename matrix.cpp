//==================================================================================
// 
// D3DXMATRIXの計算関連関数をまとめたソースファイル [matrix.cpp]
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
D3DXMATRIX *Mtx::CalcPosition(D3DXMATRIX *pMtx, const D3DXVECTOR3 &position)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	D3DXMATRIX MtxTrans;		// 計算用変数

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
D3DXMATRIX *Mtx::CalcRotation(D3DXMATRIX *pMtx, const D3DXVECTOR3 &rotation)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	D3DXMATRIX MtxRot;		// 計算用変数

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
D3DXMATRIX *Mtx::CalcRotation(D3DXMATRIX *pMtx, const D3DXQUATERNION &quaternion)
{
	// 出力先がnullptrならnullptrを返す
	if (pMtx == nullptr) return nullptr;

	D3DXMATRIX MtxQua;		// 計算用変数

	// マトリックス計算
	D3DXMatrixRotationQuaternion(&MtxQua, &quaternion);

	// マトリックスを掛け合わせる
	D3DXMatrixMultiply(pMtx, pMtx, &MtxQua);

	// 計算後のマトリックスへのポインタを返す
	return pMtx;
}

//==================================================================================
// --- ワールドマトリックスの計算処理 ---
//==================================================================================
D3DXMATRIX *Mtx::CalcWorld(D3DXMATRIX *pMtx, const D3DXVECTOR3 &position, const D3DXVECTOR3 &rotation)
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
D3DXMATRIX *Mtx::CalcWorld(D3DXMATRIX *pMtx, const D3DXVECTOR3 &position, const D3DXQUATERNION &quaternion)
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
D3DXMATRIX *Mtx::CalcWorld(D3DXMATRIX *pMtx,
	const D3DXMATRIX *pParent,
	const D3DXVECTOR3 &position,
	const D3DXVECTOR3 &rotation)
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
D3DXMATRIX *Mtx::CalcWorld(D3DXMATRIX *pMtx,
	const D3DXMATRIX *pParent,
	const D3DXVECTOR3 &position, 
	const D3DXQUATERNION &quaternion)
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
// --- シャドウマトリックスの作成処理 ---
//==================================================================================
D3DXMATRIX *Mtx::CreateShadow(const D3DXMATRIX *pMtxWorld,
	const D3DXVECTOR3 &pos,
	const D3DXVECTOR3 &nor,
	const D3DLIGHT9 &light,
	D3DXMATRIX *pOut)
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