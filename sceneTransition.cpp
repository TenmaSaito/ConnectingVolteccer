//==================================================================================
// 
// 画面遷移クラスのソースファイル [sceneTransition.cpp]
// Author : TENMA SAITO
// Date   : 2026/7/22
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "sceneTransition.h"
#include "circleTransition.h"
#include "lineScaleUpTransition.h"

//==================================================================================
// --- トランジションの作成 ---
//==================================================================================
CSceneTransition *CSceneTransition::Create(const TYPE type)
{
	CSceneTransition *pTransition = nullptr;		// 生成したトランジションへのポインタ

	switch (type)
	{
	case TYPE_CIRCLE:				// 円状遷移演出
		pTransition = new CCircleTransition;
		break;

	case TYPE_LINE_SCALE_UP:		// 線拡大遷移演出
		pTransition = new CLineScaleUpTransition;
		break;

	default:
		break;
	}

	if (pTransition)
	{ // 生成に成功していれば、初期化
		pTransition->Init();
	}

	return pTransition;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CSceneTransition::CSceneTransition()
{ // メンバ変数の初期化
	m_modeNext = CScene::MODE_NONE;
	m_state = STATE_STAY;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CSceneTransition::~CSceneTransition()
{
}