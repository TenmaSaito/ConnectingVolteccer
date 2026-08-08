//==================================================================================
// 
// ジョイパッドクラスのソースファイル [joypad.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/15
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "joypad.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CJoypad::CJoypad()
{
	// メンバ変数をクリア
	memset(&m_aJoypad[0], 0, sizeof(m_aJoypad));
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CJoypad::~CJoypad()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
void CJoypad::Init(void)
{
	// XInputを有効にする
	XInputEnable(TRUE);
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CJoypad::Uninit(void)
{
	// XInputを無効にする
	XInputEnable(FALSE);
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CJoypad::Update(void)
{
	XINPUT_STATE joykeyState;			// 入力情報

	for (int nCntJoypad = 0; nCntJoypad < MAX_JOYPAD; nCntJoypad++)
	{ // ジョイパッドの数分繰り返す
		if (XInputGetState(nCntJoypad, &joykeyState) == ERROR_SUCCESS)
		{ // ジョイパッドの状態取得成功時
			// 省略用変数
			XINPUT_STATE *pTrigger = &m_aJoypad[nCntJoypad].joykeyStateTrigger;		// トリガー情報
			XINPUT_STATE *pRelease = &m_aJoypad[nCntJoypad].joykeyStateRelease;		// リリース情報
			XINPUT_STATE *pPress = &m_aJoypad[nCntJoypad].joykeyState;				// プレス情報
			int *pRepeat = &m_aJoypad[nCntJoypad].nJoykeyStateRepeat[0];			// リピートカウント

			// トリガー情報を保存
			pTrigger->Gamepad.wButtons = ((joykeyState.Gamepad.wButtons ^ pPress->Gamepad.wButtons) & joykeyState.Gamepad.wButtons);

			// リリース情報を保存
			pRelease->Gamepad.wButtons = (pPress->Gamepad.wButtons & (pPress->Gamepad.wButtons ^ joykeyState.Gamepad.wButtons));
			
			// プレス情報を保存
			*pPress = joykeyState;		

			for (int nCntJoykey = 0; nCntJoykey < KEY_MAX; nCntJoykey++)
			{ // ジョイパッドのボタン数分繰り返す
				if ((pPress->Gamepad.wButtons & (0x01 << nCntJoykey)) == false)
				{ // ボタンが押されていなかった場合、リピートカウントリセット
					pRepeat[nCntJoykey] = 0;
				}
			}
		}
	}
}

//==================================================================================
// --- プレス判定処理 ---
//==================================================================================
bool CJoypad::GetPress(const KEY key, const int nPadID)
{
	return (m_aJoypad[nPadID].joykeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//==================================================================================
// --- トリガー判定処理 ---
//==================================================================================
bool CJoypad::GetTrigger(const KEY key, const int nPadID)
{
	return (m_aJoypad[nPadID].joykeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//==================================================================================
// --- リリース判定処理 ---
//==================================================================================
bool CJoypad::GetRelease(const KEY key, const int nPadID)
{
	return (m_aJoypad[nPadID].joykeyStateRelease.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//==================================================================================
// --- リピート判定処理 ---
//==================================================================================
bool CJoypad::GetRepeat(const KEY key, const int nWaitPress, const int nInterval, const int nPadID)
{
	// リピートカウント増加
	m_aJoypad[nPadID].nJoykeyStateRepeat[key]++;
	if (m_aJoypad[nPadID].nJoykeyStateRepeat[key] <= nWaitPress)
	{ // リピートカウントが移行待機時間内なら、トリガーの判定を返す
		return (m_aJoypad[nPadID].joykeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
	}
	else
	{ // リピートカウントがプレスへの移行待機時間を超えたなら
		if (m_aJoypad[nPadID].nJoykeyStateRepeat[key] % nInterval == 0)
		{ // インターバル分時間が経ったら、プレスの判定を返す
			return (m_aJoypad[nPadID].joykeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
		}
		else
		{ // インターバル中なら失敗
			return false;
		}
	}
}

//==================================================================================
// --- スティック判定処理 ---
//==================================================================================
bool CJoypad::GetStick(const STICK stick, const float fRange, const int nPadID)
{
	// 省略用変数
	XINPUT_GAMEPAD *pGamepad = &m_aJoypad[nPadID].joykeyState.Gamepad;		// ゲームパッドへのポインタ
	short sRange = (short)(SHRT_MAX * fRange);		// shortへの変換後の範囲
	short sThumb;									// 判定する値

	if (stick >= STICK_RIGHT)
	{ // 右スティックの場合
		// 上下判定ならLY,左右判定ならLXの値を取得
		sThumb = (stick == STICK_RIGHT_UP || stick == STICK_RIGHT_DOWN) ? pGamepad->sThumbRY : pGamepad->sThumbRX;

		if (stick == STICK_RIGHT_UP || stick == STICK_RIGHT_RIGHT)
		{ // 上もしくは右に倒されているか判定したい場合
			return (sThumb >= sRange) ? true : false;
		}
		else
		{ // 下もしくは左に倒されているか判定したい場合
			return (sThumb <= sRange) ? true : false;
		}
	}
	else
	{ // 左スティックの場合
		// 上下判定ならLY,左右判定ならLXの値を取得
		sThumb = (stick == STICK_LEFT_UP || stick == STICK_LEFT_DOWN) ? pGamepad->sThumbLY : pGamepad->sThumbLX;

		if (stick == STICK_LEFT_UP || stick == STICK_LEFT_RIGHT)
		{ // 上もしくは右に倒されているか判定したい場合
			return (sThumb >= sRange) ? true : false;
		}
		else
		{ // 下もしくは左に倒されているか判定したい場合
			return (sThumb <= sRange) ? true : false;
		}
	}
}

//==================================================================================
// --- スティック取得処理 ---
//==================================================================================
bool CJoypad::GetStick(const STICK stick, Vector3 *pOut, const int nPadID)
{
	// 省略用変数
	XINPUT_GAMEPAD *pGamepad = &m_aJoypad[nPadID].joykeyState.Gamepad;		// ゲームパッドへのポインタ
	Vector3 out = Vector3(0, 0, 0);		// 出力する変換先変数

	// 出力先がNULLの場合、失敗
	if (pOut == NULL) return false;

	// 列挙型の指定が間違っている場合、失敗
	if (stick != STICK_RIGHT && stick != STICK_LEFT) return false;

	// スティックの値をVECTOR3に変換
	if (stick == STICK_RIGHT)
	{ // 右スティックの場合
		out.x = (float)pGamepad->sThumbRX / (float)SHRT_MAX;
		out.y = (float)pGamepad->sThumbRY / (float)SHRT_MAX;
	}
	else
	{ // 左スティックの場合
		out.x = (float)pGamepad->sThumbLX / (float)SHRT_MAX;
		out.y = (float)pGamepad->sThumbLY / (float)SHRT_MAX;
	}

	// 出力先へ書き出し
	*pOut = out;

	return true;
}