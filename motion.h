//==================================================================================
// 
// モーションクラスのヘッダーファイル [motion.h]
// Author : TENMA SAITO
// Date   : 2026/6/8
// 
//==================================================================================
#ifndef _MOTION_H_		// インクルードガード
#define _MOTION_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <memory>
#include <span>
#include <vector>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_MOTION_NUM		(15)	// モーションの最大総数
#define MAX_KEY				(15)	// キーの最大数
#define MAX_KEY_INFO		(15)	// キー情報の最大数

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CModel;

//**********************************************************************************
// *** モーションクラス ***
//**********************************************************************************
class CMotion
{
public:
	// エイリアス宣言
	using ModelArray = std::span<std::unique_ptr<CModel>>;				// モデルのポインタ配列へのビュー

	// キー要素構造体の定義 
	typedef struct
	{
		Vector3 pos;	// 位置
		Vector3 rot;	// 角度
	} KEY;

	// キー情報構造体の定義
	typedef struct
	{
		int nFrame;					// 再生フレーム
		std::vector<KEY> vKey;		// 各パーツのキー要素(パーツ数分だけ用意)
	} KEY_INFO;

	// モーション情報構造体の定義
	typedef struct
	{
		bool bLoop;							// ループするかどうか
		int nNumKey;						// キーの総数
		std::vector<KEY_INFO> vKeyInfo;		// キー情報
	} INFO;

	CMotion();
	~CMotion();

	void Init(void);
	void Uninit(void);
	void Update(void);
	void SetInfo(const INFO info);
	void SetModel(ModelArray ppModel);
	void Set(const int nType);
	void Set(const int nType, const int nBlendFrame);
	int GetType(void) const { return m_nType; }
	int GetBlendType(void) const { return m_nTypeBlend; }
	bool IsFinish(void) const { return m_bFinish; }

private:
	void SetByBlend(void);

	std::vector<INFO> m_vInfo;		// モーション情報
	ModelArray m_ppModel;	// モデルの配列へのアクセス
	int m_nType;			// モーションタイプ
	bool m_bLoop;			// 現在のモーションのループの有無
	int m_nNumKey;			// 現在のキーの総数
	int m_nKey;				// 現在の現在のキーNo (基本始まりは0)
	int m_nCounter;			// 現在のモーションのカウンター
	bool m_bFinish;			// モーションが終了しているか

	bool m_bBlend;			// ブレンドモーションがあるか
	int m_nTypeBlend;		// ブレンドモーションのモーションの種類
	bool m_bLoopBlend;		// ブレンドモーションがループするかどうか
	int m_nNumKeyBlend;		// ブレンドモーションのキーの総数
	int m_nKeyBlend;		// ブレンドモーションの現在のキーNo (基本始まりは0)
	int m_nFrameBlend;		// ブレンドフレーム数
	int m_nCounterBlend;	// ブレンドカウンター数
	int m_nCounterMotionBlend;		// ブレンドモーションのカウンター
};
#endif