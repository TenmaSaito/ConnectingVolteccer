//==================================================================================
// 
// プレイヤーカメラクラスのヘッダーファイル [playerCamera.h]
// Author : TENMA SAITO
// Date   : 2026/5/8
// 
//==================================================================================
#ifndef _PLAYERCAMERA_H_		// インクルードガード
#define _PLAYERCAMERA_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "camera.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_RIDING_LENGTH		(250.0f)		// デフォルトの電柱登攀時のカメラ距離

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlayer;

//**********************************************************************************
// *** プレイヤーカメラクラス ***
//**********************************************************************************
class CPlayerCamera : public CCamera
{
public:
	// フォーカスの種類
	typedef enum
	{
		STATE_PLAYER = 0,		// プレイヤーフォーカス
		STATE_RIDING,			// 電柱に乗ったプレイヤーフォーカス
		STATE_MAX
	} STATE;

	static CPlayerCamera *Create(const Vector3 &rot,
		const Vector3 &rotRiding,
		const float fLengthPlayer,
		const float fLengthRiding = DEFAULT_RIDING_LENGTH);

	CPlayerCamera();
	~CPlayerCamera();

	void Init(const Vector3 &rot, 
		const Vector3 &rotRiding, 
		const float fLengthPlayer, 
		const float fLengthRiding);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
	float GetLengthPlayer(void) const { return m_fLengthPlayer; }
	float GetLengthRiding(void) const { return m_fLengthRiding; }
	void SetState(const STATE state);
	void BindPlayer(const CPlayer *pPlayer) { m_pPlayer = pPlayer; }

private:
	const CPlayer *m_pPlayer;		// プレイヤーへのポインタ
	Vector3 m_rotDefault;			// 通常のカメラ角度
	Vector3 m_rotRidingDefault;		// 電柱登攀時の通常のカメラ角度
	float m_fLengthPlayer;		// プレイヤーとの距離
	float m_fLengthRiding;		// 電柱登攀時の距離
	STATE m_state;				// 現在の状態
};
#endif