//==================================================================================
// 
// チュートリアル進行管理クラスのヘッダーファイル [tutorialManager.h]
// Author : TENMA SAITO
// Date   : 2026/9/2
// 
//==================================================================================
#ifndef _TUTORIAL_MANAGER_H_		// インクルードガード
#define _TUTORIAL_MANAGER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPolygon2D;

//**********************************************************************************
// *** チュートリアル進行管理クラス ***
//**********************************************************************************
class CTutorialManager
{
public:
	// チュートリアルの種類
	typedef enum
	{
		PHASE_REASON = 0,		// 理由1
		PHASE_REASON_2,			// 理由2
		PHASE_TUTORIAL_START,	// チュートリアル開始
		PHASE_FIND_POWERPLANT,	// 発電所を見つけよう (操作フォーカス終了)
		PHASE_RIDEON,			// 近づいて発電所に乗ってみよう
		PHASE_CONNECT,			// 接続方法のチュートリアル1 (繋げ方)
		PHASE_CONNECT_2,		// 接続方法のチュートリアル2 (選び方)
		PHASE_INVOKE_ELECTRIC,	// 通電のチュートリアル
		PHASE_AFTER_LIVELINESS,	// 通電による影響 (操作フォーカス開始)
		PHASE_TUTORIAL_END,		// チュートリアル終了！
		PHASE_MAX
	} PHASE;

	CTutorialManager();
	~CTutorialManager();

	static CTutorialManager *Create(void);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	std::string_view GetTutorialMapPath(void) const;
	void SetNextPhase(void);
	PHASE GetPhase(void) const { return m_phase; }
	bool GetFocus(void) const { return m_bInputFocus; }

private:
	PHASE m_phase;		// 現在のチュートリアルフェーズ
	bool m_bInputFocus;	// 入力をフォーカスしているか
	float m_fFogStart;	// フォグの開始位置
	float m_fPitch;		// ピッチ
	std::unique_ptr<CPolygon2D> m_pEffect;		// フェーズが進むたびに表示する褒めたたえポリゴンへのポインタ
	std::unique_ptr<CPolygon2D> m_pCommentBase;	// コメントの土台へのポインタ
	std::unique_ptr<CPolygon2D> m_pComment;		// コメントへのポインタ
};
#endif