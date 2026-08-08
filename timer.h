//==================================================================================
// 
// タイマークラスのヘッダーファイル [timer.h]
// Author : TENMA SAITO
// Date   : 2026/5/20
// 
//==================================================================================
#ifndef _TIMER_H_
#define _TIMER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CNumber;		// 数値表示クラス

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_TIMERNUM				(8)						// タイマーの最大桁数
#define DEFAULT_TIMER_PRIORITY		(DEFAULT_UI_PRIORITY)	// タイマーの基本優先順位

//**********************************************************************************
// *** スコアクラス ***
//**********************************************************************************
class CTimer : public CObject
{
public:
	CTimer(const int nPriority = DEFAULT_TIMER_PRIORITY);
	~CTimer();

	static CTimer *Create(const Vector3 &pos, const Vector2 &size, const int nNumNumber);
	static CTimer *Create(const Vector3 &pos, const Vector2 &size, const int nNumNumber, const int nScore);

	HRESULT Init(void);
	HRESULT Init(const Vector3 &pos, const Vector2 &size, const int nNumNumber);
	HRESULT Init(const Vector3 &pos, const Vector2 &size, const int nNumNumber, const int nScore);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetUpdate(const bool bUpdate);
	bool GetUpdate(void) const;
	void SetTimer(const int nScore);
	void AddTimer(const int nValue);
	int GetTimer(void) const;

private:
	CNumber *m_apNumber[MAX_TIMERNUM];			// 数値表示オブジェクトへのポインタ
	int m_nNumTime;		// タイマーの桁数
	int m_nTime;		// 現在のタイマー
	int m_nCounter;		// フレームカウンター
	bool m_bUpdate;		// タイマーの自動更新フラグ
};

#endif