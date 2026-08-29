//==================================================================================
// 
// リザルトカメラクラスのヘッダーファイル [resultCamera.h]
// Author : TENMA SAITO
// Date   : 2026/8/30
// 
//==================================================================================
#ifndef _RESULTCAMERA_H_		// インクルードガード
#define _RESULTCAMERA_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "camera.h"

//**********************************************************************************
// *** リザルトカメラクラス ***
//**********************************************************************************
class CResultCamera : public CCamera
{
public:
	static CResultCamera *Create(const Vector3 &posV, const Vector3 &posR);

	CResultCamera();
	~CResultCamera();

	void Init(const Vector3 &posV, const Vector3 &posR);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
};
#endif