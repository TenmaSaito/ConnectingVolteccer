//==================================================================================
// 
// タイトルカメラクラスのヘッダーファイル [titleCamera.h]
// Author : TENMA SAITO
// Date   : 2026/9/5
// 
//==================================================================================
#ifndef _TITLECAMERA_H_		// インクルードガード
#define _TITLECAMERA_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include "camera.h"

//**********************************************************************************
// *** タイトルカメラクラス ***
//**********************************************************************************
class CTitleCamera : public CCamera
{
public:
	static CTitleCamera *Create(const Vector3 &posV, const Vector3 &posR);

	CTitleCamera();
	~CTitleCamera();

	void Init(const Vector3 &posV, const Vector3 &posR);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
};
#endif