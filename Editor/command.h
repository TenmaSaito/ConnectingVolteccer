//==================================================================================
// 
// コマンドベースクラスのヘッダーファイル [command.h]
// Author : TENMA SAITO
// Date   : 2026/9/28
// 
//==================================================================================
#ifndef _COMMAND_BASE_H_
#define _COMMAND_BASE_H_
#include <Windows.h>

//**********************************************************************************
// *** コマンドベースクラス ***
//**********************************************************************************
class CCommandBase
{
public:
	CCommandBase() = default;
	virtual ~CCommandBase() = default;

	virtual void Do(void) = 0;
	virtual void Undo(void) = 0;
};
#endif