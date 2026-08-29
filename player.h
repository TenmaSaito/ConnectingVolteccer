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
#include <memory>
#include <vector>
#include <variant>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_PLAYER_PRIORITY	DEFAULT_OBJ_PRIORITY			// プレイヤーの基本優先順位
#define DEFAULT_PLAYER_POS		VECTOR3_NULL					// プレイヤーの基本位置
#define DEFAULT_PLAYER_ROT		VECTOR3_NULL					// プレイヤーの基本角度
#define MAX_PLAYER_MODEL_PATH	(50)							// 保存可能なモデルパス数

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CModel;
class CMotion;
class CThunderEffect;
class CObjectXQuaternion;
class CUtilityPole;
class CPowerPlant;
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

	// std::variantに含まれている変数の種類
	typedef enum
	{
		CPOWERPLANT_PTR = 0,	// 発電所へのポインタ
		CUTILITYPOLE_PTR,		// 電柱へのポインタ
		PTRTYPE_MAX
	} PTRTYPE;

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
	const std::variant<CPowerPlant*, CUtilityPole*> *GetRidingObject(void) const { return &m_pRidingObject; }
	CPowerPlant *GetStartPlant(void) const { return m_pStartPlant; }
	int GetNumLightingHouse(void) const { return m_nNumLightingHouse; }
	bool IsShotLasso(void) const { return m_bShotLasso; }
	void ChangeRidingPole(CUtilityPole *pNext);
	void AddLightingHouse(void) { m_nCurrentConnectLighting++; }
	void FailedShot(void);

private:
	void InputAction(void);
	void InputMoving(void);
	void InputPole(void);
	void UpdatePotision(void);
	void UpdateRotateDest(void);
	void UpdatePole(void);
	void MoveToNextPole(void);
	void CheckRidingRight(void);
	void FindNearestPole(void);
	void DismountPole(void);
	void CollisionAction(void);
	void OtherUpdate(void);
	CObjectXQuaternion *GetRidingObjectX(void);

	Vector3 m_pos;		// 位置
	Vector3 m_offset;	// オフセット保存用
	Vector3 m_move;		// 移動量
	Vector3 m_rot;		// 角度
	Vector3 m_rotDest;	// 目標角度
	Matrix m_mtxWorld;	// ワールドマトリックス
	CThunderEffect *m_pThunderEffect;	// 雷エフェクトへのポインタ
	std::variant<CPowerPlant*, CUtilityPole*> m_pRidingObject;		// 乗っているオブジェクト (発電所 or 電柱)
	CPowerPlant *m_pStartPlant;		// 初めに乗った発電所へのポインタ
	CUtilityPole *m_pPoleNext;	// 次に乗る電柱	
	std::vector<std::unique_ptr<CModel>> m_vpModel;		// 各モデル(パーツ)へのポインタ
	std::unique_ptr<CMotion> m_pMotion;					// モーションへのポインタ
	char m_aModelPath[MAX_PLAYER_MODEL_PATH][MAX_PATH];	// 各モデルのパス
	int m_nNumModel;		// モデルの総数
	bool m_bShotLasso;		// 投げ縄を投げたか
	Vector3 m_vecQua;		// 回転の任意軸
	float m_fAngleRest;		// 次の電柱への残りの角度
	int m_nNumLightingHouse;			// 電線の接続で電気のついた家の総数
	int m_nCurrentConnectLighting;		// 今回の電線の接続で電気のついた家の数
	bool m_bCreateConnectEffect;		// 電気のついた家の数で表示が変わる演出を既に生成したか
};
#endif