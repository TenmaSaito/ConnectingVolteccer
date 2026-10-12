//==================================================================================
// 
// ランキングカメラクラスのヘッダーファイル [rankingCamera.h]
// Author : TENMA SAITO
// Date   : 2026/9/10
// 
//==================================================================================
#ifndef _RANKINGCAMERA_H_		// インクルードガード
#define _RANKINGCAMERA_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "camera.h"

//**********************************************************************************
// *** ランキングカメラクラス ***
//**********************************************************************************
class CRankingCamera : public CCamera
{
public:
	static CRankingCamera *Create(const Vector3 &posV, const Vector3 &posR);

	CRankingCamera();
	~CRankingCamera();

	void Init(const Vector3 &posV, const Vector3 &posR);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
};
#endif