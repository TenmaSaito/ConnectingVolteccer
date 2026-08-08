//==================================================================================
// 
// 草原クラスのヘッダーファイル [grassland.h]
// Author : TENMA SAITO
// Date   : 2026/6/16
// 
//==================================================================================
#ifndef _GRASSLAND_H_		// インクルードガード
#define _GRASSLAND_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_GRASS_NUM			(256)		// 草原がはやせる草の最大数

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CGrass;

//**********************************************************************************
// *** 草原クラス ***
//**********************************************************************************
class CGrassLand : public CObject
{
public:
	// 草原生成時のプロパティ
	struct Propaty
	{
		Vector2 maxSize;	// 最大サイズ
		Vector2 minSize;	// 最小サイズ
		float fMaxSwaySpeed;	// 揺れる最大速度
		float fMinSwaySpeed;	// 揺れる最小速度
		float fMaxSwayWidth;	// 揺れる最大幅
		float fMinSwayWidth;	// 揺れる最小幅
	};

	static CGrassLand *Create(const Vector3 &pos, const Vector2 &size, const int nNumGrass, const Propaty &propaty = GetDefaultPropaty());
	static Propaty GetDefaultPropaty(void);

	CGrassLand();
	~CGrassLand();

	HRESULT Init(const Vector3 &pos, const Vector2 &size, const int nNumGrass, const Propaty &propaty);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool Collision(const Vector3 &pos, const float fRadius);

private:
	CGrass *m_apGrass[MAX_GRASS_NUM];		// 生やした草へのポインタ
	int m_nNumGrass;		// 生やした草の数
	Propaty m_propaty;		// 生成時のプロパティ
};
#endif