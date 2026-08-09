//==================================================================================
// 
// Vector2,3の計算関連関数をまとめたソースファイル [vmath.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "vec3math.h"
#ifndef VEC3_INLINE

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define SQUARE(x)		(x * x)			// 2乗

//**********************************************************************************
// *** Vector3計算関連名前空間の定義 ***
//**********************************************************************************

//==================================================================================
// --- 3次元ベクトルの長さ取得処理 ---
//==================================================================================
float Vec3::Length(const Vector3 &vec)
{
	return sqrtf(SQUARE(vec.x) + SQUARE(vec.y) + SQUARE(vec.z));
}

//==================================================================================
// --- 3次元ベクトルの長さ取得処理 (2点間指定) ---
//==================================================================================
float Vec3::Length(const Vector3 &To, const Vector3 &From)
{
	return Length(To - From);
}

//==================================================================================
// --- 3次元ベクトルの長さの2乗取得処理 ---
//==================================================================================
float Vec3::LengthSq(const Vector3 &vec)
{
	return SQUARE(vec.x) + SQUARE(vec.y) + SQUARE(vec.z);
}

//==================================================================================
// --- 3次元ベクトルの長さの2乗取得処理 (2点間指定) ---
//==================================================================================
float Vec3::LengthSq(const Vector3 &To, const Vector3 &From)
{
	return LengthSq(To - From);
}

//==================================================================================
// --- 3次元ベクトル同士の内積処理 ---
//==================================================================================
float Vec3::Dot(const Vector3 &vec1, const Vector3 &vec2)
{
	return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
}

//==================================================================================
// --- 3次元ベクトル同士の垂直判定処理 ---
//==================================================================================
bool Vec3::IsVertical(const Vector3 &vec1, const Vector3 &vec2)
{
	return (Cross(vec1, vec2) == VECTOR3_NULL) ? true : false;
}

//==================================================================================
// --- 3次元ベクトル同士の平行判定処理 ---
//==================================================================================
bool Vec3::IsParallel(const Vector3 &vec1, const Vector3 &vec2)
{
	return (Dot(vec1, vec2) == 0.0f) ? true : false;
}

//==================================================================================
// --- 三角形の内側判定処理 ---
//==================================================================================
bool Vec3::IsInsideTriangle(const Vector3 &pos,
	const Vector3 *pVtx,
	const bool bInverse)
{
	Vector3 aVecLine[3];			// 三角形の内側判定に使う境界線ベクトル
	Vector3 aVecToPos[3];			// 各頂点とのベクトル
	float aVecPos[3];		// 外積結果

	// nullptrなら無視
	if (pVtx == nullptr) return false;

	// 境界線ベクトルを求める
	aVecLine[0] = pVtx[1] - pVtx[0];
	aVecLine[1] = pVtx[2] - pVtx[1];
	aVecLine[2] = pVtx[0] - pVtx[2];

	// 各頂点とのベクトルを求める
	aVecToPos[0] = pos - pVtx[0];
	aVecToPos[1] = pos - pVtx[1];
	aVecToPos[2] = pos - pVtx[2];

	// 外積を求める
	aVecPos[0] = Cross(aVecLine[0], aVecToPos[0]).y;
	aVecPos[1] = Cross(aVecLine[1], aVecToPos[1]).y;
	aVecPos[2] = Cross(aVecLine[2], aVecToPos[2]).y;

	// 判定結果を返す
	if (bInverse == false)
	{ // 判定の反転無しの場合、内側ならtrue
		return (aVecPos[0] > 0 && aVecPos[1] > 0 && aVecPos[2] > 0);
	}
	else
	{ // 判定の反転有りの場合,外側ならtrue
		return (aVecPos[0] < 0 && aVecPos[1] < 0 && aVecPos[2] < 0);
	}
}

//==================================================================================
// --- カメラの横幅の内側判定処理 ---
//==================================================================================
bool Vec3::IsInsideViewOfBeside(const Vector3 &pos,
	const Vector3 &posV,
	const Vector3 &posR,
	const float fovy)
{
	Vector3 vecView = posR - posV;		// 視線ベクトル
	Vector3 vecToPos = pos - posV;		// 判定する点へのベクトル
	float fLengthToPos = 0.0f;		// 判定点へのベクトルの長さ
	float fTheta = 0.0f;	// 基準値
	float fCos = 0.0f;		// 結果

	// 縦幅は関係ない為、0でクリア
	vecView.y = 0.0f;
	vecToPos.y = 0.0f;

	// ベクトルを正規化
	vecView = Normalize(vecView);
	vecToPos = Normalize(vecToPos);

	// 基準値を求める
	fTheta = cosf(fovy);

	// 視野内にいるか確かめる
	fCos = Dot(vecToPos, vecView);
	if (fCos >= fTheta)
	{
		return true;
	}

	return false;
}

//==================================================================================
// --- カメラの縦幅の内側判定処理 ---
//==================================================================================
bool Vec3::IsInsideViewOfVertical(const Vector3 &pos,
	const Vector3 &posV,
	const Vector3 &posR,
	const float fovy)
{
	Vector3 vecView = posR - posV;		// 視線ベクトル
	Vector3 vecToPos = pos - posV;		// 判定する点へのベクトル
	float fLengthToPos = 0.0f;		// 判定点へのベクトルの長さ
	float fTheta = 0.0f;	// 基準値
	float fCos = 0.0f;		// 結果

	// 横幅は関係ない為、0でクリア
	vecView.x = 0.0f;
	vecToPos.x = 0.0f;

	// ベクトルを正規化
	vecView = Normalize(vecView);
	vecToPos = Normalize(vecToPos);

	// 基準値を求める
	fTheta = cosf(fovy);

	// 視野内にいるか確かめる
	fCos = Dot(vecToPos, vecView);
	if (fCos >= fTheta)
	{
		return true;
	}

	return false;
}

//==================================================================================
// --- XZから求まる三角形のY座標の取得処理 ---
//==================================================================================
float Vec3::Height(const Vector3 &pos,
	const Vector3 &vtx, 
	const Vector3 &nor)
{
	float fHeight = 0.0f;		// 計算結果

	// 法線が垂直な場合スキップ
	if (nor.y == 0.0f) return fHeight;

	// 高さを求めて結果を返す
	fHeight = vtx.y - (((pos.x - vtx.x) * nor.x + (pos.z - vtx.z) * nor.z) / nor.y);
	return fHeight;
}

//==================================================================================
// --- 法線ベクトルの計算処理 ---
//==================================================================================
Vector3 Vec3::Nor(const Vector3 &origin, 
	const Vector3 *pVtx,
	const bool bInverse)
{
	Vector3 aVec[2];	// 各頂点の境界線ベクトル
	Vector3 nor;		// 頂点の法線

	// 各境界線ベクトルを求める
	aVec[0] = pVtx[0] - origin;
	aVec[1] = pVtx[1] - origin;

	// 法線を各ベクトルから求める
	if (bInverse == false)
	{ // 法線ベクトルを逆で求めない場合
		D3DXVec3Cross(&nor, &aVec[0], &aVec[1]);
	}
	else
	{ // 法線ベクトルを逆で求める場合
		D3DXVec3Cross(&nor, &aVec[1], &aVec[0]);
	}

	// 法線を正規化
	D3DXVec3Normalize(&nor, &nor);
	return nor;
}

//==================================================================================
// --- 3次元ベクトル同士の外積処理 ---
//==================================================================================
Vector3 Vec3::Cross(const Vector3 &vec1, const Vector3 &vec2)
{
	Vector3 closs;		// 外積結果

	// 外積を求める
	closs.x = (vec1.y * vec2.z) - (vec1.z * vec2.y);
	closs.y = (vec1.z * vec2.x) - (vec1.x * vec2.z);
	closs.z = (vec1.x * vec2.y) - (vec1.y * vec2.x);

	return closs;
}

//==================================================================================
// --- 3次元ベクトルの正規化処理 ---
//==================================================================================
Vector3 Vec3::Normalize(const Vector3 &vec)
{
	float fLength;			// ベクトルの長さ

	// ベクトルの長さを取得
	fLength = Length(vec);

	// ベクトルの長さで正規化
	return Vector3(vec.x / fLength, vec.y / fLength, vec.z / fLength);
}

//==================================================================================
// --- 3次元ベクトルの線形補間処理 ---
//==================================================================================
Vector3 Vec3::Lerp(const Vector3 &start,
	const Vector3 &end,
	const float t)
{
	Vector3 vec;		// 線形補間後のベクトル

	// 二点間の差分を求める
	vec = end - start;

	// 補間後の値を返す
	return start + (vec * t);
}

//==================================================================================
// --- 2点間の間の取得処理 ---
//==================================================================================
Vector3 Vec3::Middle(const Vector3 &p1, const Vector3 &p2)
{
	// 線形補間で求める
	return Lerp(p1, p2, 0.5f);
}

//==================================================================================
// --- 3次元単位ベクトル処理 ---
//==================================================================================
Vector3 Vec3::Direction(const Vector3 &To, const Vector3 &From)
{
	return Normalize(To - From);
}

//==================================================================================
// --- 角度の3次元単位ベクトル取得処理 ---
//==================================================================================
Vector3 Vec3::Direction(const Vector3 &angle)
{
	// 球面座標を返す
	return Vector3(sinf(angle.y) * sinf(angle.x), cosf(angle.x), cosf(angle.y) * sinf(angle.x));
}

//==================================================================================
// --- ランダムな3次元単位ベクトル取得処理 ---
//==================================================================================
Vector3 Vec3::Random(void)
{
	float fTheta = (D3DX_PI * 2) * (float)rand() / RAND_MAX;	// ランダムな角度1
	float fPhi = (D3DX_PI * 2) * (float)rand() / RAND_MAX;		// ランダムな角度2

	// 球面座標を返す
	return Vector3(sinf(fPhi) * sinf(fTheta), cosf(fPhi), cosf(fPhi) * sinf(fTheta));
}

//==================================================================================
// --- 3次元ベクトルの範囲内矯正処理 ---
//==================================================================================
Vector3 Vec3::Clamp(const Vector3 &vec,
	const Vector3 &min, 
	const Vector3 &max)
{
	Vector3 clamp;		// クランプ後の値

	// 最大値と最小値でクランプした値を代入
	clamp.x = (vec.x < min.x) ? min.x : ((vec.x > max.x) ? max.x : vec.x);
	clamp.y = (vec.y < min.y) ? min.y : ((vec.y > max.y) ? max.y : vec.y);
	clamp.z = (vec.z < min.z) ? min.z : ((vec.z > max.z) ? max.z : vec.z);

	return clamp;
}

//==================================================================================
// --- 角度の修正処理 ---
//==================================================================================
Vector3 Vec3::FixedRotation(const Vector3 &rot)
{
	Vector3 fixed;		// 修正後の角度
	auto fixedRot = [](float fRot) -> float	
	{ // 角度の修正式
		if (fRot > D3DX_PI)
		{ // 角度がPIを超えたとき
			return fRot - DOUBLE_PI;
		}
		else if (fRot <= -D3DX_PI)
		{ // 角度が-PIを以下になった時
			return fRot + DOUBLE_PI;
		}
		else
		{ // 変更の必要なし
			return fRot;
		}
	};

	// 各軸を修正
	fixed.x = fixedRot(rot.x);
	fixed.y = fixedRot(rot.y);
	fixed.z = fixedRot(rot.z);

	return fixed;
}

//==================================================================================
// --- 球面上の座標取得処理 ---
//==================================================================================
Vector3 Vec3::Arc(const float fRadius,
	const float fTheta, 
	const float fPhi, 
	const Vector3 &offset)
{
	Vector3 pos = offset;		// 円弧上の座標

	// 角度と半径から求めた値を加算
	pos.x += (sinf(fPhi) * sinf(fTheta)) * fRadius;
	pos.y += cosf(fPhi) * fRadius;
	pos.z += (sinf(fPhi) * cosf(fTheta)) * fRadius;

	return pos;
}

//==================================================================================
// --- 指定した値の代入処理 ---
//==================================================================================
Vector3 &Vec3::Fill(Vector3 &rVec, const float fValue)
{
	// VECTOR3に値を埋める
	rVec = Vector3(fValue, fValue, fValue);

	return rVec;
}

//==================================================================================
// --- 指定した値で埋めたVECTOR3取得処理 ---
//==================================================================================
Vector3 Vec3::Fill(const float fValue)
{
	// VECTOR3に値を埋めて、返す
	return Vector3(fValue, fValue, fValue);
}

//==================================================================================
// --- ラジアンへの変換処理 ---
//==================================================================================
Vector3 Vec3::ToRadian(const Vector3 &degree, const bool bFixed)
{
	Vector3 radian;		// 変換後の角度

	// ラジアン変換
	radian.x = D3DXToRadian(degree.x);
	radian.y = D3DXToRadian(degree.y);
	radian.z = D3DXToRadian(degree.z);

	// 元から角度が修正されているか確認
	if (bFixed == false)
	{ // 修正されていなければ、角度を修正
		radian = FixedRotation(radian);
	}

	return radian;
}

//==================================================================================
// --- 360°への変換処理 ---
//==================================================================================
Vector3 Vec3::ToDegree(const Vector3 &radian, const bool bFixed)
{
	Vector3 fixedRadian = radian;		// 修正後の角度
	Vector3 degree = radian;			// 変換後の角度

	// 元から角度が修正されているか確認
	if (bFixed == false)
	{ // 修正されていなければ、角度を修正
		fixedRadian = FixedRotation(radian);
	}

	// 360°変換
	degree.x = D3DXToDegree(fixedRadian.x);
	degree.y = D3DXToDegree(fixedRadian.y);
	degree.z = D3DXToDegree(fixedRadian.z);

	return degree;
}

//==================================================================================
// --- VECTOR2への変換処理処理 ---
//==================================================================================
Vector2 Vec3::ToVector2(const Vector3 &vec, const Axis less)
{
	float x = vec[(static_cast<UINT>(less) - 1U) % static_cast<UINT>(Axis::MAX)];
	float y = vec[(static_cast<UINT>(less) + 1U) % static_cast<UINT>(Axis::MAX)];

	return Vector2(x, y);
}

//==================================================================================
// --- VECTOR3の軸指定代入処理 ---
//==================================================================================
void Vec3::AssignAxis(Vector3 &vec, const Axis axis, const float fValue)
{
	if (axis == Axis::MAX) return;
	static_cast<float *>(vec)[static_cast<UINT>(axis)] = fValue;
}

//==================================================================================
// --- VECTOR3の軸指定取得処理 ---
//==================================================================================
float Vec3::GetAxis(Vector3 &vec, const Axis axis)
{
	if (axis == Axis::MAX) return NAN;
	return static_cast<float *>(vec)[static_cast<UINT>(axis)];
}

//==================================================================================
// --- 軸列挙変換処理 (Axis -> AxisEx) ---
//==================================================================================
AxisEx Vec3::ToAxisEx(const Axis axis)
{
	return static_cast<AxisEx>(axis);
}

//==================================================================================
// --- 軸列挙変換処理 (AxisEx -> Axis) ---
//==================================================================================
Axis Vec3::ToAxis(const AxisEx axisEx)
{
	if (axisEx >= static_cast<AxisEx>(Axis::MAX)) return Axis::MAX;
	return static_cast<Axis>(axisEx);
}
#endif