//==================================================================================
// 
// マップクラスのヘッダーファイル [map.h]
// Author : TENMA SAITO
// Date   : 2026/6/22
// 
//==================================================================================
#ifndef _MAP_H_		// インクルードガード
#define _MAP_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <string>
#include <vector>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_PATH_LEN		(260)		// 代入可能な文字列の長さ

//**********************************************************************************
// *** マップクラス ***
//**********************************************************************************
class CMap
{
public:
	typedef enum
	{
		BUILDING_0 = 0,		// 建物0
		BUILDING_1,			// 建物1
		BUILDING_2,			// 建物2
		BUILDING_3,			// 建物3
		BUILDING_MAX		
	} BUILDING;

	// 出力する情報
	struct IODATA;

	CMap();
	~CMap();

	void Resister(const BUILDING type, const char *pPath);
	void AddUtilityPole(const Vector3 &pos);
	void AddBulding(const BUILDING type, const Vector3 &pos);
	void Save(const char *pMapFile);
	void Load(const char *pMapFile);
	static CMap *GetInstance(void);

private:
	char m_aBuildingPath[BUILDING_MAX][MAX_PATH_LEN];
};
#endif