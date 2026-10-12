//==================================================================================
// 
// 電流追従カメラクラスのヘッダーファイル [thunderCamera.h]
// Author : TENMA SAITO
// Date   : 2026/8/29
// 
//==================================================================================
#ifndef _THUNDERCAMERA_H_		// インクルードガード
#define _THUNDERCAMERA_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "camera.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CElectricCurrent;

//**********************************************************************************
// *** 電流追従カメラクラス ***
//**********************************************************************************
class CThunderCamera : public CCamera
{
public:
	CThunderCamera();
	~CThunderCamera();

	static CThunderCamera *Create(const float fLength);

	HRESULT Init(const float fLength);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
	void ChangeTarget(const CElectricCurrent *pTarget);

private:
	const CElectricCurrent *m_pTarget;		// 現在ターゲットしている電流
	float m_fLength;		// カメラと電流の距離
	D3DLIGHT9 m_light;		// 電流カメラ用ライト
};
#endif