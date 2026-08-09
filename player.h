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
#include <memory>

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
		const Vector3 &pos,
		const Vector3 &rot);

	CPlayer(const int nPriority = DEFAULT_PLAYER_PRIORITY);
	~CPlayer();

	HRESULT Init(const char *pFilename,
		const Vector3 &pos,
		const Vector3 &rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	const Vector3 *GetPosition(void) const { return &m_pos; }
	const Vector3 *GetRotation(void) const { return &m_rot; }
	const Vector3 *GetRotationDest(void) const { return &m_rotDest; }
	const Vector3 *GetOffset(void) const { return &m_offset; }
	const Vector3 *GetMove(void) const { return &m_move; }
	const Matrix *GetMatrix(void) const { return &m_mtxWorld; }
	CUtilityPole *GetRidingPole(void) const { return m_pRidingPole; }
	bool IsShotLasso(void) const { return m_bShotLasso; }
	void SetLassoComplete(void) { m_bShotLasso = false; }
	void ChangeRidingPole(CUtilityPole *pNext) { m_pPoleNext = pNext; }

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

	Vector3 m_pos;		// 位置
	Vector3 m_offset;	// オフセット保存用
	Vector3 m_move;		// 移動量
	Vector3 m_rot;		// 角度
	Vector3 m_rotDest;	// 目標角度
	Matrix m_mtxWorld;	// ワールドマトリックス
	CUtilityPole *m_pRidingPole;		// 乗っている電柱	
	CUtilityPole *m_pPoleNext;			// 次に乗る電柱	
	std::unique_ptr<CModel> m_apModel[MAX_PLAYER_MODEL_NUM];		// モデル(パーツ)へのポインタ
	std::unique_ptr<CMotion> m_pMotion;					// モーションへのポインタ
	char m_aModelPath[MAX_PLAYER_MODEL_PATH][MAX_PATH];	// 各モデルのパス
	int m_nNumModel;		// モデルの総数
	CThunderEffect *m_pThunderEffect;		// 雷エフェクトへのポインタ
	bool m_bShotLasso;						// 投げ縄を投げたか
	float m_fAngle;			// 次の電柱への回転度合い
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