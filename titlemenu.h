//==================================================================================
// 
// タイトル画面のメニュークラスのヘッダーファイル [titlemenu.h]
// Author : TENMA SAITO
// Date   : 2026/8/20
// 
//==================================================================================
#ifndef _TITLEMENU_H_
#define _TITLEMENU_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** タイトルメニュークラス ***
//**********************************************************************************
class CTitleMenu : public CObject
{
public:
	// メニューの種類
	typedef enum
	{
		TYPE_START = 0,		// STARTボタン
		TYPE_UNKNOWN,		// 未設定
		TYPE_EXIT,			// 終了ボタン
		TYPE_MAX
	} TYPE;

	CTitleMenu();
	~CTitleMenu();

	static CTitleMenu *Create(void);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	TYPE GetCurrentType(void) const { return m_currentType; }

private:
	void ActionType(void);

	std::array<std::unique_ptr<CPolygon2D>, TYPE_MAX> m_apMenu;		// 各メニューのポリゴンへのポインタ
	std::unique_ptr<CPolygon2D> m_pCircle;	// メニュー選択肢の土台のポリゴンへのポインタ
	std::unique_ptr<CPolygon2D> m_pLogo;	// ロゴのポリゴンへのポインタ
	TYPE m_lastType = TYPE_START;			// 直前のメニュータイプ
	TYPE m_currentType = TYPE_START;		// 現在のメニュータイプ
	float m_fTypeDest = 0.0f;			// 目標タイプ
	float m_fTime = 0.0f;				// 線形補間の時間
};
#endif