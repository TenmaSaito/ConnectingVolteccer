//==================================================================================
// 
// タイトルクラスのヘッダーファイル [title.h]
// Author : TENMA SAITO
// Date   : 2026/7/1
// 
//==================================================================================
#ifndef _TITLE_H_		// インクルードガード
#define _TITLE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CTitleCamera;
class CTitleMenu;

//**********************************************************************************
// *** タイトルクラス ***
//**********************************************************************************
class CTitle : public CSceneBase<CTitle, CScene::MODE_TITLE>
{
public:
	CTitle();
	~CTitle();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	CTitleMenu *GetMenu(void) const { return m_pMenu; }

	static constexpr MODE GetMyMode(void) { return CScene::MODE_TITLE; }

private:
	void Start(void);

	CTitleMenu *m_pMenu;	// メニュー
};
#endif