//==================================================================================
// 
// スキップ表示クラスのヘッダーファイル [skip.h]
// Author : TENMA SAITO
// Date   : 2026/9/25
// 
//==================================================================================
#ifndef _SKIP_H_		// インクルードガード
#define _SKIP_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** チュートリアルスキップ表示クラス ***
//**********************************************************************************
class CSkip : public CObject
{
public:
	CSkip();
	~CSkip();

	static CSkip *Create(const Vector3 &pos, const Vector2 &size);

	HRESULT Init(const Vector3 &pos, const Vector2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindSkipTexture(const int nIdxTexture);

private:
	void UpdateInput(void);
	void UpdatePolygon(void);

	std::unique_ptr<CPolygon2D> m_pSkip;		// スキップ表示ポリゴン
	std::unique_ptr<CPolygon2D> m_pGauge;		// 長押しゲージポリゴン
	Vector3 m_pos;		// 位置
	Vector2 m_size;		// サイズ
	int m_nFramePress;	// 長押ししたフレーム数
};
#endif