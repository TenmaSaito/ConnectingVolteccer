//==================================================================================
// 
// プレイヤークラスのヘッダーファイル [player.h]
// Author : TENMA SAITO
// Date   : 2026/6/2
// 
//==================================================================================
#ifndef _PLAYER_H_		// インクルードガード
#define _PLAYER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"
#include <stdio.h>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_PLAYER_PRIORITY	DEFAULT_OBJ_PRIORITY			// プレイヤーの基本優先順位
#define DEFAULT_PLAYER_POS		VECTOR3_NULL					// プレイヤーの基本位置
#define DEFAULT_PLAYER_ROT		VECTOR3_NULL					// プレイヤーの基本角度
#define MAX_PLAYER_MODEL_NUM	(15)							// プレイヤーの確保可能なモデル数
#define MAX_PLAYER_MODEL_PATH	(50)							// 保存可能なモデルパス数
#define MAX_PLAYER_KEYINFO		(20)							// キー情報の最大数
#define MAX_PLAYER_MOTION		(10)							// モーション情報の最大数

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CModel;
class CFileStream;
class CMotion;
class CThunderEffect;
class CUtilityPole;
class CObjectLine;

//**********************************************************************************
// *** プレイヤークラス ***
//**********************************************************************************
class CPlayer : public CObject
{
public:
	// モーションの種類
	typedef enum
	{
		MOTIONTYPE_NEUTRAL = 0,		// 待機モーション
		MOTIONTYPE_MOVE,			// 移動モーション
		MOTIONTYPE_ACTION,			// 行動モーション
		MOTIONTYPE_JUMP,			// ジャンプモーション
		MOTIONTYPE_LANDING,			// 着地モーション
		MOTIONTYPE_MAX
	} MOTIONTYPE;

	static CPlayer *Create(const char *pFilename,
		const D3DXVECTOR3 &pos,
		const D3DXVECTOR3 &rot);

	CPlayer(const int nPriority = DEFAULT_PLAYER_PRIORITY);
	~CPlayer();

	HRESULT Init(const char *pFilename,
		const D3DXVECTOR3 &pos,
		const D3DXVECTOR3 &rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	const D3DXVECTOR3 *GetPosition(void) const { return &m_pos; }
	const D3DXVECTOR3 *GetRotation(void) const { return &m_rot; }
	const D3DXVECTOR3 *GetRotationDest(void) const { return &m_rotDest; }
	const D3DXVECTOR3 *GetOffset(void) const { return &m_offset; }
	const D3DXVECTOR3 *GetMove(void) const { return &m_move; }
	const D3DXMATRIX *GetMatrix(void) const { return &m_mtxWorld; }
	CUtilityPole *GetRidingPole(void) const { return m_pRidingPole; }

private:
	void InputAction(void);
	void InputMoving(void);
	void InputPole(void);
	void InputMap(void);
	void UpdatePotision(void);
	void UpdateRotateDest(void);
	void UpdatePole(void);
	void CollisionAction(void);
	void OtherUpdate(void);
	HRESULT LoadFile(const char *pFilename);
	void LoadCharactorData(CFileStream *pFile);
	void LoadPartsData(CFileStream *pFile, const int nCntModel);
	void LoadMotionData(CFileStream *pFile);
	bool DeleteComment(char *pStr);
	template<class... Args> void LoadData(const char *pStr, const char *pFormat, Args... args);

	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_offset;	// オフセット保存用
	D3DXVECTOR3 m_move;		// 移動量
	D3DXVECTOR3 m_rot;		// 角度
	D3DXVECTOR3 m_rotDest;	// 目標角度
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	CUtilityPole *m_pRidingPole;						// 乗っている電柱	
	CModel *m_apModel[MAX_PLAYER_MODEL_NUM];			// モデル(パーツ)へのポインタ
	char m_aModelPath[MAX_PLAYER_MODEL_PATH][MAX_PATH];	// 各モデルのパス
	int m_nNumModel;		// モデルの総数
	CMotion *m_pMotion;		// モーションへのポインタ
	CThunderEffect *m_pThunderEffect;		// 雷エフェクトへのポインタ
};

//==================================================================================
// --- データの読み取り処理 ---
//==================================================================================
template<class... Args> void CPlayer::LoadData(const char *pStr, const char *pFormat, Args... args)
{
	const char *pStart = nullptr;	// =の走査用ポインタ

	// =後のポインタを取得
	pStart = strchr(pStr, '=');

	// 読み込み
	(void)sscanf(pStart + 1, pFormat, args...);
}
#endif