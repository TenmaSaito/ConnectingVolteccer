//==================================================================================
// 
// シーンクラスのヘッダーファイル [scene.h]
// Author : TENMA SAITO
// Date   : 2026/6/24
// 
//==================================================================================
#ifndef _SCENE_H_		// インクルードガード
#define _SCENE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <memory>

//**********************************************************************************
// *** モードクラス ***
//**********************************************************************************
class CScene
{
public:
	// モードの種類
	typedef enum
	{
		MODE_TITLE = 0,		// タイトル
		MODE_GAME,			// ゲーム
		MODE_GAMEOVER,		// ゲームオーバー
		MODE_GAMECLEAR,		// ゲームクリア
		MODE_MAX
	} MODE;

	CScene(const MODE mode);
	virtual ~CScene();

	static CScene *Create(const MODE mode);
	static CScene *Create(const MODE mode, std::unique_ptr<CScene> &rpOut);

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	MODE GetMode(void) const { return m_mode; }

private:
	MODE m_mode;		// 現在のモード
};
#endif