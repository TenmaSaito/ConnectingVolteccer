//==================================================================================
// 
// DirectXサブ描画クラスのヘッダーファイル [directXSubDrawer.h]
// Author : TENMA SAITO
// Date   : 2026/5/12
// 
//==================================================================================
#ifndef _DIRECTX_SUBDRAWER_H_		// インクルードガード
#define _DIRECTX_SUBDRAWER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "delegate_t.h"
#include <thread>

//**********************************************************************************
// *** DirectXクラス ***
//**********************************************************************************
class CDirectXSubDrawer
{
public:
	typedef enum
	{
		FUNCTION_INIT = 0,
		FUNCTION_UNINIT,
		FUNCTION_UPDATE,
		FUNCTION_DRAW,
		FUNCTION_MAX
	} FUNCTION;

	CDirectXSubDrawer();
	~CDirectXSubDrawer();

	void FunctionSetUp(const FUNCTION funcType, hyp::Action<> func);
	void DirectXSubDrawerProc(void);
	void Quit(void);

private:
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	hyp::Action<> m_aFunc[FUNCTION_MAX];		// 各関数デリゲーター
	bool m_aCallFunction[FUNCTION_MAX];				// 関数呼び出しがあるかのフラグ
	bool m_bThreadLoop;		// スレッドループフラグ
};
#endif