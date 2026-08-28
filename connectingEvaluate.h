//==================================================================================
// 
// 接続時評価表示クラスのヘッダーファイル [connectingEvaluate.h]
// Author : TENMA SAITO
// Date   : 2026/6/2
// 
//==================================================================================
#ifndef _CONNECTING_EVALUATE_H_		// インクルードガード
#define _CONNECTING_EVALUATE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"
#include <memory>
#include <vector>

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** 接続時評価表示クラス ***
//**********************************************************************************
class CConnectingEvaluate : public CObject
{
public:
	typedef enum
	{
		EVALUATE_NONE = 0,		// 評価無し
		EVALUATE_GOOD,			// GOOD判定
		EVALUATE_GREAT,			// GREAT判定
		EVALUATE_EXCELLENT,		// EXCELLENT判定
		EVALUATE_MAX
	} EVALUATE;

	// 出現させるポリゴンの情報構造体
	struct PolyInfo
	{
		std::unique_ptr<CPolygon2D> pPoly;		// ポリゴンへのポインタ
		int nLife;				// 寿命
	};

	static CConnectingEvaluate *Create(const Vector3 &pos, const Vector2 &scale);
	
	CConnectingEvaluate();
	~CConnectingEvaluate();

	HRESULT Init(const Vector3 &pos, const Vector2 &scale);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void AddEvaluate(const int nNumLightingHouse);

private:
	void CreatePolygon(const EVALUATE eval);
	void PolygonsUpdate(void);

	std::vector<PolyInfo> m_vPolyInfo;		// 各演出用ポリゴンの情報
	Vector3 m_pos;		// 発生座標
	Vector2 m_scale;	// 発生サイズ
};
#endif