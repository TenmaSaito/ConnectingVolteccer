//==================================================================================
// 
// マップクラスのソースファイル [map.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/22
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "map.h"
#include "object.h"
#include "objectX.h"
#include "objectXQuaternion.h"
#include "utilityPole.h"
#include "filestream.h"
#include "manager.h"
#include "game.h"
#include "result.h"
#include "planet.h"
#include "building.h"
#include "powerPlant.h"
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <locale>
#include <variant>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define LATEST_MAPFILE		"data/Maps/latestMapPath.bin"		// 直近のマップパスを書いてあるファイル

//**********************************************************************************
// *** マップ情報構造体 ***
//**********************************************************************************
struct CMap::IODATA
{
	int type;			// モデルの種類
	int nIdxModel;		// 建物のインデックス
	Vector3 pos;		// 位置
	Vector3 vecQua;		// 任意軸
	float fAngle;		// 角度
};

//**********************************************************************************
// *** モデルのタイプ保存用情報構造体 ***
//**********************************************************************************
struct THIS_FILE_OBJECT_TYPEINFO
{
	int nBuildingType;		// ファイル作成時の建物のタイプのインデックス
	int nPoleType;			// ファイル作成時の電柱のタイプのインデックス
	int nPowerPlantType;	// ファイル作成時の発電所のタイプのインデックス
};

//==================================================================================
// --- インスタンス取得処理 ---
//==================================================================================
CMap *CMap::GetInstance(void)
{
	static CMap instance;		// インスタンス
	return &instance;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMap::CMap()
{ // メンバ変数をクリア
	m_nNumBuilding = 0;
	m_nNumID = 0;
	m_pPlanet = nullptr;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMap::~CMap()
{
}

//==================================================================================
// --- 変数のリセット処理 ---
//==================================================================================
void CMap::Reset(void)
{ // メンバ変数をリセット
	m_nNumBuilding = 0;
	m_nNumID = 0;
	m_pPlanet = nullptr;
	m_vConnectID.clear();
	m_vCurrentID.clear();
	m_currentFilePath.clear();
}

//==================================================================================
// --- ゲームシーンの状態の再生処理 ---
//==================================================================================
void CMap::ReloadByConnectID(void)
{ // 建物を再出現させる
	m_nNumID = 0;
	Load(m_currentFilePath);

	// 電線を繋げ直す
	ConnectByConnectID();

	// 各変数をリセット
	Reset();
}

//==================================================================================
// --- 電柱設置処理 ---
//==================================================================================
void CMap::AddUtilityPole(const Vector3 &pos)
{
	Vector3 vecQua = VECTOR3_NULL;
	float fAngle = 0.0f;

	// クォータニオンから軸と角度を求める
	D3DXQuaternionToAxisAngle(m_pPlanet->GetQuaternion(),
		&vecQua,
		&fAngle);

	// 角度反転
	fAngle *= -1;

	// 電柱設置 + 親設定
	CUtilityPole *pPole = CUtilityPole::Create(pos,
		vecQua,
		fAngle,
		m_nNumID);
	pPole->SetParent(m_pPlanet->GetMatrix());
	m_nNumID++;
}

//==================================================================================
// --- 建造物設置処理 ---
//==================================================================================
void CMap::AddBulding(const int nType, const Vector3 &pos)
{ // 建造物設置
	CBuilding *pBuilding = CBuilding::Create(static_cast<CBuilding::TYPE>(nType), pos);
	pBuilding->SetParent(m_pPlanet->GetMatrix());

	m_nNumBuilding++;
}

//==================================================================================
// --- 発電所設置処理 ---
//==================================================================================
void CMap::AddPowerPlant(const Vector3 &pos)
{ // 発電所設置
	CPowerPlant::Create(pos, m_nNumID);
	m_nNumID++;
}

//==================================================================================
// --- マップ書き出し処理 ---
//==================================================================================
void CMap::Save(std::string_view sMapFile)
{
	std::vector<std::string> filepath;			// ファイルパス
	std::vector<CBuilding::TYPE> aIdxModel;		// モデルインデックス
	std::vector<IODATA> outData;			// 出力データ群
	constexpr long long flag = OBJTYPE_TO_BITFLAG(CObject::TYPE_POLE) 
		| OBJTYPE_TO_BITFLAG(CObject::TYPE_BUILDING) 
		| OBJTYPE_TO_BITFLAG(CObject::TYPE_POWERPLANT);

	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{
		CObject *pObject = CObject::GetTop(nCntPriority);		// 先頭
		while (pObject != nullptr)
		{ // nullptrになるまで走査
			CObject *pObjectNext = pObject->GetNext();		// 次のオブジェクトへのポインタ
			CObject::TYPE type = pObject->GetType();		// オブジェクトの種類
			if (static_cast<bool>(FIND_BITFLAG_BY_OBJTYPE(flag, static_cast<long long>(type))))
			{ // もし、書き出すタイプなら
				// CObjectXQuaternionにキャスト
				CObjectXQuaternion *pObjXQua = static_cast<CObjectXQuaternion *>(pObject);

				IODATA out;		// 出力データ

				// メモリクリア
				ZeroMemory(&out, sizeof(IODATA));

				// タイプを保存
				out.type = type;

				// 建物クラスの場合モデルインデックスを保存
				out.nIdxModel = (type == CObject::TYPE_BUILDING) ? static_cast<CBuilding*>(pObject)->GetType() : -1;
				
				// モデルの位置を保存
				out.pos = *pObjXQua->GetPosition();

				// クォータニオンから任意軸と角度を取得して保存
				D3DXQuaternionToAxisAngle(pObjXQua->GetQuaternion(),
					&out.vecQua,
					&out.fAngle);

				// 書き出し情報に追加
				outData.push_back(out);
			}

			pObject = pObjectNext;		// オブジェクトを進める
		}
	}

	// データ書き出し
	std::unique_ptr<CFileStream> pFile(new CFileStream);		// ファイルストリーム
	std::ofstream *pOfs = pFile->GetOutStream();		// 出力ストリームへのポインタ

	if (pFile->CreateFile(sMapFile, true, CFileStream::FLAG_OVERWRITE))
	{
		// 書き出し時に小数第2位まで書き出すことを指定
		(*pOfs) << std::fixed << std::setprecision(2);

		THIS_FILE_OBJECT_TYPEINFO objTypeInfo = {};		// タイプ情報

		// オブジェクトのタイプの定義が次回読み込み時に変わる恐れがあるため、
		// 今回のオブジェクトのタイプを事前に保存
		objTypeInfo.nBuildingType = CObject::TYPE_BUILDING;		// 建物のタイプを保存
		objTypeInfo.nPoleType = CObject::TYPE_POLE;				// 電柱のタイプを保存
		objTypeInfo.nPowerPlantType = CObject::TYPE_POWERPLANT;	// 発電所のタイプを保存
		
		// オブジェクトのタイプを書き出し
		pFile->Write(&objTypeInfo, sizeof(THIS_FILE_OBJECT_TYPEINFO));

		// モデルの合計数を書き出し
		*pFile << outData.size();

		for (const auto &out : outData)
		{ // モデルのデータ書き出し
			*pFile << out.type
				<< out.nIdxModel
				<< out.pos.x
				<< out.pos.y
				<< out.pos.z
				<< out.vecQua.x
				<< out.vecQua.y
				<< out.vecQua.z
				<< out.fAngle;
		}

		// ファイルを閉じる
		pFile->CloseFile();
	}
	else
	{ // ファイル作成失敗
		return;
	}

	if (pFile->CreateFile(LATEST_MAPFILE, true, CFileStream::FLAG_OVERWRITE))
	{ // ファイル作成成功
		// ファイル名を保存
		pFile->WriteString(sMapFile);
	}
	else
	{ // ファイル作成失敗
		return;
	}
}

//==================================================================================
// --- マップ読み込み処理 ---
//==================================================================================
void CMap::Load(std::string_view sMapFile)
{
	int nNumModel = 0;		// モデルの数
	const Matrix *pMatrix = nullptr;	// 惑星のマトリックスへのポインタ

	// 惑星とマトリックスのポインタを取得
	pMatrix = m_pPlanet->GetMatrix();

	// データ読み込み
	std::unique_ptr<CFileStream> pFile(new CFileStream);	// ファイルストリーム

	if (pFile->OpenFile(sMapFile, true))
	{ // ファイルが開けた場合
		m_currentFilePath = sMapFile;		// ファイル名を保存

		THIS_FILE_OBJECT_TYPEINFO fileTypeInfo = {};		// 読み込んだファイルのオブジェクトタイプ情報

		// ファイルに保存されたモデルのインデックスを取得
		pFile->Read(&fileTypeInfo, sizeof(THIS_FILE_OBJECT_TYPEINFO));

		// モデル数読み込み
		*pFile >> nNumModel;

		for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
		{ // モデル数分データ読み込み
			IODATA in = { 0 };		// 出力データ

			// データ読み込み
			pFile->Read(in.type);
			pFile->Read(in.nIdxModel);
			pFile->Read(in.pos.x);
			pFile->Read(in.pos.y);
			pFile->Read(in.pos.z);
			pFile->Read(in.vecQua.x);
			pFile->Read(in.vecQua.y);
			pFile->Read(in.vecQua.z);
			pFile->Read(in.fAngle);

			// タイプ別でオブジェクトを配置
			CObject::TYPE type = static_cast<CObject::TYPE>(in.type);
			if (type == fileTypeInfo.nBuildingType)
			{ // 建物配置
				CBuilding *pBuilding = CBuilding::Create(static_cast<CBuilding::TYPE>(in.nIdxModel),
					in.pos,
					in.vecQua,
					in.fAngle);
				pBuilding->SetParent(pMatrix);
				m_nNumBuilding++;
			}
			else if (type == fileTypeInfo.nPoleType)
			{ // 電柱配置
				CUtilityPole *pPole = CUtilityPole::Create(in.pos,
					in.vecQua,
					in.fAngle,
					m_nNumID);
				pPole->SetParent(pMatrix);
				m_nNumID++;
			}
			else if (type == fileTypeInfo.nPowerPlantType)
			{ // 発電所配置
				CPowerPlant *pPlant = CPowerPlant::Create(in.pos,
					in.vecQua,
					in.fAngle,
					m_nNumID);
				pPlant->SetParent(pMatrix);
				m_nNumID++;
			}
		}

		// ファイルを閉じる
		pFile->CloseFile();
	}
}

//==================================================================================
// --- プレイヤーの繋げた順番を基に各オブジェクトに電線を繋ぐ処理 ---
//==================================================================================
void CMap::ConnectByConnectID(void)
{ // 各建物へのポインタ
	std::vector<std::variant<CPowerPlant*, CUtilityPole*>> vpBuilding;
	CObject *pObject = CObject::GetTop(DEFAULT_OBJ_PRIORITY);		// オブジェクトへのポインタ

	// 配列サイズを拡張
	vpBuilding.resize(m_nNumID);

	while (pObject != nullptr)
	{ // オブジェクトへのポインタが存在する場合
		CObject *pObjectNext = pObject->GetNext();		// 次のオブジェクトへのポインタ
		CObject::TYPE type = pObject->GetType();		// オブジェクトのタイプ
		if (type != CObject::TYPE_POWERPLANT
			&& type != CObject::TYPE_POLE)
		{ // 建物でも電柱でも発電所でもない場合、スキップ
			pObject = pObjectNext;
			continue;
		}

		// 何かしらの建物へのポインタ
		std::variant<CPowerPlant*, CUtilityPole*> pBuilding;

		// タイプによってキャスト先を変更
		if (type == CObject::TYPE_POWERPLANT) pBuilding = static_cast<CPowerPlant*>(pObject);
		else if (type == CObject::TYPE_POLE) pBuilding = static_cast<CUtilityPole*>(pObject);

		// IDを取得
		int nID = std::visit([](auto &x) { return x->GetID(); }, pBuilding);

		// そのIDの位置にポインタを代入
		vpBuilding[nID] = pBuilding;

		// オブジェクトを進める
		pObject = pObjectNext;
	}

	for (const auto &vec : m_vConnectID)
	{ // プレイヤーの行動回数分繰り返し
		for (auto iter = vec.cbegin(); iter != vec.cend();)
		{
			const auto &value = *iter;		// 値

			// 次のインデックスが存在しないなら終了
			if ((iter + 1) == vec.cend()) break;

			std::visit([&](auto &x)
				{ // 次のインデックスのオブジェクトと接続
					auto next = vpBuilding.at(*(iter + 1));		// 次のインデックスのオブジェクト
					if (std::holds_alternative<CUtilityPole*>(next))
					{ // 電柱を保持しているなら、その電柱と接続
						CUtilityPole *pPole = std::get<CUtilityPole*>(next);
						x->Connect(pPole);
					}
				}, vpBuilding[value]);

			++iter;
		}

		if (std::holds_alternative<CPowerPlant*>(vpBuilding.at(vec.at(0))))
		{ // 発電所へのポインタなら、電気を流す！
			CPowerPlant *pPlant = std::get<CPowerPlant*>(vpBuilding.at(vec.at(0)));		// 発電所へのポインタ
			pPlant->InvokeElectric();
		}
	}
}

//==================================================================================
// --- 直近で作成されたマップの読み込み処理 ---
//==================================================================================
void CMap::LoadLatest(void)
{
	// データ読み込み
	std::unique_ptr<CFileStream> pFile(new CFileStream);	// ファイルストリーム
	std::string sPath;		// マップファイル名

	if (pFile->OpenFile(LATEST_MAPFILE, true))
	{ // ファイルオープン成功
		pFile->ReadString(sPath);

		// ファイルを閉じる
		pFile->CloseFile();
	}

	// ファイル読み込み
	Load(sPath);
}

//==================================================================================
// --- IDを登録していた配列の確定処理 ---
//==================================================================================
void CMap::ConfirmID(void)
{ // 配列を登録後、中身をリセット
	m_vConnectID.push_back(m_vCurrentID);
	m_vCurrentID.clear();
}

//==================================================================================
// --- IDを登録していた配列の取り消し処理 ---
//==================================================================================
void CMap::WithdrawalID(void)
{ // 配列を登録せず、中身をリセット
	m_vCurrentID.clear();
}