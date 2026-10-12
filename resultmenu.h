//==================================================================================
// 
// リザルト画面のメニュークラスのヘッダーファイル [resultmenu.h]
// Author : TENMA SAITO
// Date   : 2026/8/31
// 
//==================================================================================
#ifndef _RESULTMENU_H_
#define _RESULTMENU_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;
class CPercent;

//**********************************************************************************
// *** リザルト画面のメニュークラス ***
//**********************************************************************************
class CResultMenu : public CObject
{
public:
	// メニューの種類
	typedef enum
	{ 
		TYPE_REPLAY = 0,	// 再度プレイ (チュートリアルはスキップ)
		TYPE_EXIT,			// 終了
		TYPE_MAX
	} TYPE;

	// スコアの前後に存在するポリゴン種類
	typedef enum
	{
		SCORE_BEFORE = 0,
		SCORE_AFTER,
		SCORE_MAX
	} SCORE;

	CResultMenu();
	~CResultMenu();

	static CResultMenu *Create(const float fValue);
	HRESULT Init(const float fValue);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	TYPE GetCurrentType(void) const { return m_currentType; }

private:
	void UpdateInput(void);
	void ActionType(void);

	std::array<std::unique_ptr<CPolygon2D>, TYPE_MAX> m_apMenu;		// メニューの各ポリゴンへのポインタ
	std::array<std::unique_ptr<CPolygon2D>, SCORE_MAX> m_apScore;	// スコアを説明する各ポリゴンへのポインタ
	std::unique_ptr<CPolygon2D> m_pResult;		// リザルトポリゴンへのポインタ
	std::unique_ptr<CPolygon2D> m_pLighting;	// パーセント下のポリゴンへのポインタ
	CPercent *m_pPercent;		// パーセント表示へのポインタ
	float m_fValue;				// 最終的なパーセント
	float m_fCurrentValue;		// 現在の表示しているパーセント
	TYPE m_currentType;			// 現在のメニュータイプ
};
#endif