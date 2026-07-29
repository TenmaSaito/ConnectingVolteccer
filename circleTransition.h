//==================================================================================
// 
// 円状の遷移クラスのヘッダーファイル [circleTransition.h]
// Author : TENMA SAITO
// Date   : 2026/7/22
// 
//==================================================================================
#ifndef _CIRCLE_TRANSITION_H_
#define _CIRCLE_TRANSITION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "sceneTransition.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define CIRCLE_NUM		(10)		// 中心から広がる円の数

//**********************************************************************************
// *** 円状の遷移クラス ***
//**********************************************************************************
class CCircleTransition : public CSceneTransition
{
public:
	// 遷移エフェクト時に使用するポリゴンの情報
	struct Circle
	{
		LPDIRECT3DVERTEXBUFFER9 pVtxBuff;		// 頂点バッファ
		int nIdxTexture;	// テクスチャインデックス
		D3DXVECTOR3 pos;	// 位置
		D3DXVECTOR3 rot;	// 角度
		float fRadius;		// 半径
	};

	CCircleTransition();
	~CCircleTransition();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void StartTransition(const int nTransitionTime, const CScene::MODE modeNext);
	void EndTransition(const int nTransitionTime);

private:
	void UpdateTransitionIn(void);
	void UpdateTransitionOut(void);
	void UpdateVertex(void);

	Circle m_aCircle[CIRCLE_NUM];	// 画面に表示されるポリゴン情報
	int m_nCountTransition;			// 遷移時間
	int m_nTransitionTime;			// 遷移にかかる時間
	bool m_bStart;			// フェードインフラグ
};
#endif