//==================================================================================
// 
// ストップウォッチクラスのヘッダーファイル [stopwatch.h]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <chrono>

//**********************************************************************************
// *** ストップウォッチクラス ***
//**********************************************************************************
class CStopWatch
{
public:
	CStopWatch();
	~CStopWatch();

	void Start(void);
	void Stop(void);
	void Reset(void);
	double GetSeconds(void) const;
	double GetMilliSeconds(void) const;
	long long GetNanoSeconds(void) const;
	template <class Unit, class Ratio> Unit GetElapsed(void) const;

private:
	std::chrono::system_clock::time_point m_begin;		// 計測開始時間
	std::chrono::system_clock::time_point m_end;		// 計測終了時間
	std::chrono::nanoseconds m_elapsed;					// 合計時間
	bool m_bRun;		// 計測しているか
};

//==================================================================================
// --- 任意の型での合計時間取得 ---
//==================================================================================
template <class Unit, class Ratio> Unit CStopWatch::GetElapsed(void) const
{
	using std::chrono::duration_cast;		// 名前空間省略
	using std::chrono::duration;			// 名前空間省略
	using std::chrono::system_clock;		// 名前空間省略

	if (m_bRun == true)
	{ // 計測中なら
		// 計測開始時間と現在時刻の差を返す
		return duration_cast<duration<Unit, Ratio>>(system_clock::now() - m_begin).count();
	}
	else
	{ // 計測停止中なら
		// 合計時間を返す
		return duration_cast<duration<Unit, Ratio>>(m_elapsed).count();
	}
}
#endif