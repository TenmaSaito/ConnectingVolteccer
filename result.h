//==================================================================================
// 
// リザルトクラスのヘッダーファイル [result.h]
// Author : TENMA SAITO
// Date   : 2026/7/14
// 
//==================================================================================
#ifndef _RESULT_H_		// インクルードガード
#define _RESULT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlanet;

//**********************************************************************************
// *** タイトルクラス ***
//**********************************************************************************
class CResult : public CSceneBase<CResult, CScene::MODE_RESULT>
{
public:
	CResult();
	~CResult();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	constexpr CPlanet *GetPlanet(void) const { return m_pPlanet; }

	static constexpr MODE GetMyMode(void) { return MODE_RESULT; }

private:
	void Start(void);

	CPlanet *m_pPlanet;		// 惑星へのポインタ
};
#endif