//==================================================================================
// 
// 線拡大の遷移クラスのヘッダーファイル [lineScaleUpTransition.h]
// Author : TENMA SAITO
// Date   : 2026/7/22
// 
//==================================================================================
#ifndef _LINE_SCALE_UP_TRANSITION_H_
#define _LINE_SCALE_UP_TRANSITION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "sceneTransition.h"
#include <memory>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;
class CMeshOrbit2D;

//**********************************************************************************
// *** 線拡大の遷移クラス ***
//**********************************************************************************
class CLineScaleUpTransition : public CSceneTransition
{
public:
	CLineScaleUpTransition();
	~CLineScaleUpTransition();

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

	std::unique_ptr<CPolygon2D> m_pLine;		// 電線用ポリゴンへのポインタ
	std::unique_ptr<CPolygon2D> m_pLineOutLine;	// 電線用ポリゴンへのポインタ
	std::unique_ptr<CPolygon2D> m_pVolt;		// 電流用ポリゴンへのポインタ
	CMeshOrbit2D *m_pVoltOrbit2D;		// 電流オービットへのポインタ
	int m_nTransitionTime;		// 次の画面へ遷移するまでに待機するフレーム (即時終了防止)
};
#endif