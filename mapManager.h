//==================================================================================
// 
// マップマネージャークラスのヘッダーファイル [mapManager.h]
// Author : TENMA SAITO
// Date   : 2026/6/22
// 
//==================================================================================
#ifndef _MAP_MANAGER_H_		// インクルードガード
#define _MAP_MANAGER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define MAX_PATH_LEN		(260)		// 代入可能な文字列の長さ
#define DEFAULT_MAP_CONNECT_PATH	"data/MAPS/ConnectID.bin"		// デフォルトの接続情報書き出し先

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlanet;
class CPlayer;
class CConnectingEvaluate;
class CBuilding;
class CUtilityPole;
class CPowerPlant;

//**********************************************************************************
// *** マップマネージャクラス ***
//**********************************************************************************
class CMapManager
{
public:
	// 出力する情報
	struct IODATA;

	CMapManager();
	~CMapManager();

	void SaveConnectID(std::string_view sConnectIDFile = DEFAULT_MAP_CONNECT_PATH);
	void LoadConnectID(std::string_view sConnectIDFile = DEFAULT_MAP_CONNECT_PATH);
	void Save(std::string_view sMapFile, const bool bLatest);
	void Load(std::string_view sMapFile);
	void Reload(std::string_view sMapFile);
	void Unload(void);
	void BindPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }
	void BindConnectingEvaluate(CConnectingEvaluate *pEvaluate) { m_pEvaluate = pEvaluate; }
	void AddUtilityPole(const Vector3 &pos);
	void AddBulding(const int nType, const Vector3 &pos);
	void AddPowerPlant(const Vector3 &pos);
	void LoadLatest(void);
	int GetNumBuilding(void) const { return m_nNumBuilding; }
	int GetNumPole(void) const { return m_nNumPole; }
	int GetNumPowerPlant(void) const { return m_nNumPowerPlant; }
	constexpr CPlanet *GetPlanet(void) const { return m_pPlanet; }
	constexpr std::span<CBuilding* const> GetBuilding(void) const { return m_vpBuilding; }
	constexpr std::span<CUtilityPole* const> GetUtilityPole(void) const { return m_vpPole; }
	constexpr std::span<CPowerPlant* const> GetPowerPlant(void) const { return m_vpPlant; }
	void AddID(const int nID) { m_vCurrentID.push_back(nID); }
	void ConfirmID(void);
	void WithdrawalID(void);
	static CMapManager *GetInstance(void);

private:
	void ConnectByConnectID(void);

	int m_nNumBuilding;		// 建物の総数
	int m_nNumPole;			// 電柱の総数
	int m_nNumPowerPlant;	// 発電所の総数
	int m_nNumID;			// 各マップオブジェクトのIDの総数
	CPlanet *m_pPlanet;		// 惑星へのポインタ
	CPlayer *m_pPlayer;		// プレイヤーへのポインタ
	CConnectingEvaluate *m_pEvaluate;	// 評価表示へのポインタ
	std::string m_currentFilePath;		// 現在読み込んだマップファイル名
	std::vector<int> m_vCurrentID;		// 現在投げている順番
	std::vector<std::vector<int>> m_vConnectID;		// プレイヤーの繋げた順番
	std::vector<CBuilding*> m_vpBuilding;			// 設置した建物へのポインタ
	std::vector<CUtilityPole*> m_vpPole;			// 設置した電柱へのポインタ
	std::vector<CPowerPlant*> m_vpPlant;			// 設置した発電所へのポインタ
};
#endif