//==================================================================================
// 
// ジョイパッドクラスのヘッダーファイル [joypad.h]
// Author : TENMA SAITO
// Date   : 2026/5/15
// 
//==================================================================================
#ifndef _JOYPAD_H_
#define _JOYPAD_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_JOYPAD			(2)			// ジョイパッドの最大数
#define STICK_DEADZONE		(0.05f)		// 動いたと感知するデッドゾーン

//**********************************************************************************
// *** ジョイパッドクラス ***
//**********************************************************************************
class CJoypad
{
public:
	//******************************************************************************
	// *** ジョイパッドのボタン入力一覧 ***
	//******************************************************************************
	typedef enum
	{
		KEY_UP = 0,				// [00] 十字キー(上)
		KEY_DOWN,				// [01] 十字キー(下)
		KEY_LEFT,				// [02] 十字キー(左)
		KEY_RIGHT,				// [03] 十字キー(右)
		KEY_START,				// [04] ボタンキー(START)
		KEY_BACK,				// [05] ボタンキー(BACK)
		KEY_LEFT_PUSH,			// [06] スティックキー(左押し込み)
		KEY_RIGHT_PUSH,			// [07] スティックキー(右押し込み)
		KEY_LB,					// [08] ボタンキー(LB)
		KEY_RB,					// [09] ボタンキー(RB)
		KEY_NONE1,				// [10] 予約済み
		KEY_NONE2,				// [11] 予約済み
		KEY_A,					// [12] ボタンキー(A)
		KEY_B,					// [13] ボタンキー(B)
		KEY_X,					// [14] ボタンキー(X)
		KEY_Y,					// [15] ボタンキー(Y)
		KEY_MAX
	} KEY;

	//******************************************************************************
	// *** ジョイスティック入力一覧 ***
	//******************************************************************************
	typedef enum
	{
		STICK_LEFT,				// [01] 左スティック(スティック取得) 
		STICK_LEFT_UP,			// [02] 左スティック(上)
		STICK_LEFT_DOWN,		// [03] 左スティック(下)
		STICK_LEFT_LEFT,		// [04] 左スティック(左)
		STICK_LEFT_RIGHT,		// [05] 左スティック(右)
		STICK_RIGHT,			// [06] 左スティック(スティック取得)
		STICK_RIGHT_UP,			// [07] 右スティック(上)
		STICK_RIGHT_DOWN,		// [08] 右スティック(下)
		STICK_RIGHT_LEFT,		// [09] 右スティック(左)
		STICK_RIGHT_RIGHT,		// [10] 右スティック(右)
		STICK_MAX
	} STICK;

	CJoypad();
	~CJoypad();

	void Init(void);
	void Uninit(void);
	void Update(void);
	
	bool GetPress(const KEY key, const int nPadID = 0);
	bool GetTrigger(const KEY key, const int nPadID = 0);
	bool GetRelease(const KEY key, const int nPadID = 0);
	bool GetRepeat(const KEY key, const int nWaitPress, const int nInterval, const int nPadID = 0);
	bool GetStick(const STICK stick, const float fRange, const int nPadID = 0);
	bool GetStick(const STICK stick, Vector3 *pOut, const int nPadID = 0);

private:
	//******************************************************************************
	// *** ジョイパッドの情報構造体 ***
	//******************************************************************************
	typedef struct
	{
		XINPUT_STATE joykeyState;				// ジョイパッドのプレス情報
		XINPUT_STATE joykeyStateTrigger;		// ジョイパッドのトリガー情報
		XINPUT_STATE joykeyStateRelease;		// ジョイパッドのリリース情報
		int	nJoykeyStateRepeat[KEY_MAX];		// ジョイパッドのリピート情報
		XINPUT_VIBRATION vibration;				// バイブレーションの情報
		int nCounterVibration;					// バイブレーションの時間
	} XINPUT_INFO;

	XINPUT_INFO m_aJoypad[MAX_JOYPAD];			// ジョイパッドの情報
};

#endif