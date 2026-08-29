//==================================================================================
// 
// モーション読み込みクラスのヘッダーファイル [motionLoader.h]
// Author : TENMA SAITO
// Date   : 2026/8/18
// 
//==================================================================================
#ifndef _MOTION_LOADER_H_		// インクルードガード
#define _MOTION_LOADER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <vector>
#include <memory>
#include <string>
#include <string_view>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define INVALID_MOTION_ID	((UINT)-1)	// モーションIDの無効値

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CMotion;

//**********************************************************************************
// *** モーション読み込みクラス ***
//**********************************************************************************
class CMotionLoader
{
public:
	// 一度読み込んだモーションの情報構造体
	struct BUFFER
	{
		std::unique_ptr<CMotion> pMotion;	// ファイルから読み込んだモーション情報
		std::string sFilename;				// ファイル名
	};

	static CMotionLoader *GetInstance(void);
	UINT Register(std::string_view path);
	UINT Register(const char *pPath);
	std::unique_ptr<CMotion> CreateMotion(const UINT uIdx) const;
	void Unload(void);

private:
	CMotionLoader();
	~CMotionLoader();

	UINT Load(const std::string_view path);

	std::vector<BUFFER> m_vBuffer;		// 読み込んだモーション情報
};
#endif