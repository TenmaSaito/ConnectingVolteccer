//==================================================================================
// 
// 草クラスのヘッダーファイル [grass.h]
// Author : TENMA SAITO
// Date   : 2026/6/16
// 
//==================================================================================
#ifndef _GRASS_H_		// インクルードガード
#define _GRASS_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** 草クラス ***
//**********************************************************************************
class CGrass
{
public:
	static CGrass *Create(const Vector3 &pos, const Vector2 &size, const float fSwaySpeed, const float fSwayWidth);

	CGrass();
	~CGrass();

	HRESULT Init(const Vector3 &pos, const Vector2 &size, const float fSwaySpeed, const float fSwayWidth);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool Collision(const Vector3 &pos, const float fRadius);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	int m_nIdxTexture;		// テクスチャインデックス
	Vector3 m_pos;			// 位置
	Vector3 m_rot;			// 角度
	Vector3 m_rotDest;		// 目標角度
	Vector2 m_size;			// サイズ
	Matrix m_mtxWorld;		// ワールドマトリックス
	float m_fSwaySpeed;		// 揺れる速度
	float m_fSwayWidth;		// 揺れる幅
	float m_fLerpCount;		// 線形補間用変数
	float m_fSpeed;			// 目標角度に近づく速さ
	bool m_bCollisionPlayer;	// プレイヤー
};
#endif