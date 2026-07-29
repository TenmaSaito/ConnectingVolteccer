//==================================================================================
// 
// 画面キャプチャクラスのヘッダーファイル [windowCapture.h]
// Author : TENMA SAITO
// Date   : 2026/6/16
// 
//==================================================================================
#ifndef _WINDOWCAPTURE_H_
#define _WINDOWCAPTURE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** 画面キャプチャクラス ***
//**********************************************************************************
class CWindowCapture
{
public:
	CWindowCapture();
	~CWindowCapture();

	HRESULT Init(const char *pDirectlyName);
	void Uninit(void);
	void Update(void);
	HRESULT ScreenShot(D3DXIMAGE_FILEFORMAT format, const char *pFileName);
	
private:
	char m_aDirectly[MAX_PATH];		// ディレクトリ名
	int m_nNumAll;					// スクリーンショットの合計枚数
};
#endif
