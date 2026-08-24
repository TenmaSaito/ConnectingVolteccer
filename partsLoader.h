//==================================================================================
// 
// パーツ読み込みクラスのヘッダーファイル [partsLoader.h]
// Author : TENMA SAITO
// Date   : 2026/8/19
// 
//==================================================================================
#ifndef _PARTS_LOADER_H_		// インクルードガード
#define _PARTS_LOADER_H_

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
#define INVALID_PATRS_ID	((UINT)-1)	// パーツIDの無効値

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CModel;

//**********************************************************************************
// *** パーツ読み込みクラス ***
//**********************************************************************************
class CPartsLoader
{
public:
	// 一度読み込んだパーツの情報構造体
	struct BUFFER
	{
		std::vector<std::unique_ptr<CModel>> vpParts;	// ファイルから読み込んだパーツ情報
		std::string sFilename;			// ファイル名
		std::vector<UINT> vParentIdx;	// 親モデルのパーツのインデックス
	};

	static CPartsLoader *GetInstance(void);
	UINT Register(const std::string_view path);
	UINT Register(const char *pPath);
	std::vector<std::unique_ptr<CModel>> CreateParts(const UINT uIdx) const;
	void Unload(void);

private:
	CPartsLoader();
	~CPartsLoader();

	UINT Load(const std::string_view path);

	std::vector<BUFFER> m_vBuffer;		// 読み込んだモーション情報
};
#endif
