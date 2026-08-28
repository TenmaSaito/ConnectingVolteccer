//==================================================================================
// 
// オブジェクトクラスのヘッダーファイル [object.h]
// Author : TENMA SAITO
// Date   : 2026/5/8
// 
//==================================================================================
#ifndef _OBJECT_H_		// インクルードガード
#define _OBJECT_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_OBJPRIORITY			(8)			// 優先順位の総数
#define MAX_OBJECT				(2048)		// オブジェクトの最大数
#define DEFAULT_BG_PRIORITY		(0)			// 背景の優先順位
#define DEFAULT_OBJ_PRIORITY	(3)			// オブジェクトの優先順位
#define DEFAULT_EFFECT_PRIORITY	(4)			// エフェクト関連の優先順位
#define DEFAULT_ADD_PRIORITY	(5)			// 加算合成関連の優先順位
#define DEFAULT_UI_PRIORITY		(6)			// UIの基本優先順位
#define ENABLE_OBJECT_LIST		TRUE		// リスト構造での管理の有効化
#define DEBUG_ASSERT_TYPE_NONE				// オブジェクトにタイプが指定されていなかった場合、アサーション
#define OBJTYPE_TO_BITFLAG(type)	(1 << (type))								// ビットフラグ化
#define FIND_BITFLAG_BY_OBJTYPE(flag, type)		((flag) & (1i64 << (type)))		// ビットフラグの確認

//**********************************************************************************
// *** オブジェクトクラス ***
//**********************************************************************************
class CObject
{
public:
	// オブジェクトの種類
	typedef enum
	{
		TYPE_NONE = 0,		// 指定無し (アサーション対象)
		TYPE_OBJ_2D,		// Object2D
		TYPE_OBJ_3D,		// Object3D
		TYPE_OBJ_BILLBOARD_2D,	// ObjectBillboard2D
		TYPE_OBJ_BILLBOARD_3D,	// ObjectBillboard3D
		TYPE_OBJ_LINE,		// ObjectLine
		TYPE_OBJ_X,			// ObjectX
		TYPE_OBJ_X_QUA,		// ObjectXQuaternion
		TYPE_PARTICLE,		// パーティクル
		TYPE_PLAYER,		// プレイヤー
		TYPE_PLANET,		// 惑星
		TYPE_SCORE,			// スコア
		TYPE_TIMER,			// タイマー
		TYPE_TITLEMENU,		// タイトルメニュー
		TYPE_COMBO,			// コンボ表示
		TYPE_EVALUATE,		// 評価表示
		TYPE_EFFECT,		// エフェクト
		TYPE_POLE,			// 電柱
		TYPE_CABLE,			// 電線
		TYPE_THUNDER,		// 雷エフェクト
		TYPE_SPARK,			// スパークエフェクト
		TYPE_ELECTRICCURRENT,		// 電流
		TYPE_MESH2D,		// メッシュ2D
		TYPE_ORBIT2D,		// メッシュオービット3D
		TYPE_ORBIT3D,		// メッシュオービット3D
		TYPE_MESHFIELD,		// メッシュフィールド
		TYPE_MESHSPHERE,	// メッシュスフィア
		TYPE_BUILDING,		// 建物
		TYPE_POWERPLANT,	// 発電所
		TYPE_MAX
	} TYPE;

	CObject(const int nPriority = DEFAULT_OBJ_PRIORITY);
	virtual ~CObject();

	virtual HRESULT Init(void) { return S_OK; }
	virtual void Uninit(void) {}
	virtual void Update(void) {}
	virtual void Draw(void) {}

	void SetType(const TYPE type) { m_type = type; }
	TYPE GetType(void) const { return m_type; }
	static CObject* GetTop(const int nPriority) { return m_apTop[nPriority]; }
	CObject *GetNext(void) const { return m_pNext; }
	static int GetNumAll(void) { return m_nNumAll; }
	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static void FlagCheckAll(void);
#if ENABLE_OBJECT_LIST
#else
	static CObject *GetObject(const int nPriority, const int nIdx) { return m_apObject[nPriority][nIdx]; }
#endif
protected:
	void Release(void);
	bool IsDeath(void) const { return m_bDeath; }

private:
#if ENABLE_OBJECT_LIST
	void AddList(void);
	void RemoveList(void);

	static CObject *m_apTop[MAX_OBJPRIORITY];		// 先頭オブジェクトへのポインタ
	static CObject *m_apCur[MAX_OBJPRIORITY];		// 最後尾オブジェクトへのポインタ
	CObject *m_pPrev;			// 前のオブジェクトへのポインタ
	CObject *m_pNext;			// 次のオブジェクトへのポインタ
#else
	static CObject *m_apObject[MAX_OBJPRIORITY][MAX_OBJECT];		// オブジェクトへのポインタ
	int m_nID;			// 自分自身のID
#endif
	static int m_nNumAll;		// オブジェクトの総数
	int m_nPriority;	// 優先順位の位置
	TYPE m_type;		// オブジェクトタイプ
	bool m_bDeath;		// 死亡フラグ
};
#endif