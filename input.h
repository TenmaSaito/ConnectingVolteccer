//==================================================================================
// 
// 入力クラスのヘッダーファイル [input.h]
// Author : TENMA SAITO
// Date   : 2026/5/11
// 
//==================================================================================
#ifndef _INPUT_H_
#define _INPUT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define NUM_KEY_MAX			(256)			// キーボードのキー数

//**********************************************************************************
// *** 入力クラス ***
//**********************************************************************************
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(const HINSTANCE hInstance, const HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;

protected:
	static LPDIRECTINPUT8 m_pInput;		// 入力オブジェクト
	LPDIRECTINPUTDEVICE8 m_pDevice;		// 入力デバイス
};

//**********************************************************************************
// *** キーボードクラス ***
//**********************************************************************************
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(const HINSTANCE hInstance, const HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(const int nKey);
	bool GetTrigger(const int nKey);
	bool GetRelease(const int nKey);
	bool GetRepeat(const int nKey, const int nWaitPress, const int nInterval);

private:
	BYTE m_aKeyState[NUM_KEY_MAX];				// キーボードのプレス情報
	BYTE m_aKeyStateTrigger[NUM_KEY_MAX];		// キーボードのトリガー情報
	BYTE m_aKeyStateRelease[NUM_KEY_MAX];		// キーボードのリリース情報
	int m_nKeyStateRepeat[NUM_KEY_MAX];			// キーボードのリピート情報
};

//**********************************************************************************
// *** マウスクラス ***
//**********************************************************************************
class CInputMouse : public CInput
{
public:
	//******************************************************************************
	// *** マウスのボタンの種類 ***
	//******************************************************************************
	typedef enum
	{
		KEY_LEFT = 0,		// 左クリック
		KEY_RIGHT,			// 右クリック
		KEY_WHEEL,			// 中クリック
		KEY_MAX
	} KEY;

	CInputMouse();
	~CInputMouse();

	HRESULT Init(const HINSTANCE hInstance, const HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(const int nKey);
	bool GetTrigger(const int nKey);
	bool GetRelease(const int nKey);
	D3DXVECTOR3 GetPositionInScreen(void);
	D3DXVECTOR3 GetPositionInClient(void);

private:
	DIMOUSESTATE2 m_CurrentMouseState;		// マウスの入力情報
	DIMOUSESTATE2 m_PrevMouseState;			// マウスの過去の入力情報
	BYTE m_aMouseState[KEY_MAX];			// マウスのプレス情報
	BYTE m_aMouseStateTrigger[KEY_MAX];		// マウスのトリガー情報
	BYTE m_aMouseStateRelease[KEY_MAX];		// マウスのリリース情報
};
#endif