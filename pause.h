//==================================================================================
// 
// ポーズ画面クラスのヘッダーファイル [pause.h]
// Author : TENMA SAITO
// Date   : 2026/9/3
// 
//==================================================================================
#ifndef _PAUSE_H_		// インクルードガード
#define _PAUSE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** ポーズクラス ***
//**********************************************************************************
class CPause
{
public:
	// 現在選ばれている選択肢の種類
	typedef enum
	{
		TYPE_CONTINUE = 0,		// 続行
		TYPE_RESTART,			// やり直し
		TYPE_EXIT,				// 終了
		TYPE_MAX
	} TYPE;

	CPause();
	~CPause();

	static CPause *Create(void);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	TYPE GetType(void) const { return m_type; }

private:
	void UpdateInput(void);
	void ActionType(void);

	std::array<std::unique_ptr<CPolygon2D>, TYPE_MAX> m_apMenu;		// 選択肢ポリゴンへのポインタ
	std::unique_ptr<CPolygon2D> m_pBack;		// 背景ポリゴン
	TYPE m_type;		// 現在のタイプ
	bool m_bDisp;		// 描画フラグ
};
#endif