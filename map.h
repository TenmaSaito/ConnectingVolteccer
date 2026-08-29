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
#include <string_view>
#include <vector>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_PATH_LEN		(260)		// 代入可能な文字列の長さ

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlanet;

//**********************************************************************************
// *** マップクラス ***
//**********************************************************************************
class CMap
{
public:
	// 出力する情報
	struct IODATA;

	CMap();
	~CMap();

	void Reset(void);
	void ReloadByConnectID(void);
	void Save(std::string_view sMapFile);
	void Load(std::string_view sMapFile);
	void AddUtilityPole(const Vector3 &pos);
	void AddBulding(const int nType, const Vector3 &pos);
	void AddPowerPlant(const Vector3 &pos);
	void LoadLatest(void);
	int GetNumBuilding(void) const { return m_nNumBuilding; }
	void SetCurrentScenePlanet(CPlanet *pPlanet) { m_pPlanet = pPlanet; }
	CPlanet *GetCurrentScenePlanet(void) const { return m_pPlanet; }
	void AddID(const int nID) { m_vCurrentID.push_back(nID); }
	void ConfirmID(void);
	void WithdrawalID(void);
	static CMap *GetInstance(void);

private:
	void ConnectByConnectID(void);

	int m_nNumBuilding;		// 建物の総数
	int m_nNumID;			// 各マップオブジェクトのIDの総数
	CPlanet *m_pPlanet;		// 惑星へのポインタ
	std::string m_currentFilePath;		// 現在読み込んだマップファイル名
	std::vector<int> m_vCurrentID;		// 現在投げている順番
	std::vector<std::vector<int>> m_vConnectID;		// プレイヤーの繋げた順番
};
#endif