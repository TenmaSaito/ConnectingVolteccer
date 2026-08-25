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
#include "planet.h"
#include "building.h"
#include "powerPlant.h"
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <locale>

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
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMap::~CMap()
{
}

//==================================================================================
// --- 電柱設置処理 ---
//==================================================================================
void CMap::AddUtilityPole(const Vector3 &pos)
{
	CGame *pGame = CManager::GetInstance()->GetScene<CGame>();
	auto pPlanet = pGame->GetPlanet();
	Vector3 vecQua = VECTOR3_NULL;
	float fAngle = 0.0f;

	// クォータニオンから軸と角度を求める
	D3DXQuaternionToAxisAngle(pPlanet->GetQuaternion(),
		&vecQua,
		&fAngle);

	// 角度反転
	fAngle *= -1;

	// 電柱設置 + 親設定
	CUtilityPole *pPole = CUtilityPole::Create(pos,
		vecQua,
		fAngle);
	pPole->SetParent(pPlanet->GetMatrix());
}

//==================================================================================
// --- 建造物設置処理 ---
//==================================================================================
void CMap::AddBulding(const int nType, const Vector3 &pos)
{ // 建造物設置
	CBuilding::Create(static_cast<CBuilding::TYPE>(nType), pos);
}

//==================================================================================
// --- 発電所設置処理 ---
//==================================================================================
void CMap::AddPowerPlant(const Vector3 &pos)
{ // 発電所設置
	CPowerPlant::Create(pos);
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
	const Matrix *pMtxPlanet = CManager::GetInstance()->GetScene<CGame>()->GetPlanet()->GetMatrix();

	// データ読み込み
	std::unique_ptr<CFileStream> pFile(new CFileStream);	// ファイルストリーム

	if (pFile->OpenFile(sMapFile, true))
	{ // ファイルが開けた場合
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
				CBuilding::Create(static_cast<CBuilding::TYPE>(in.nIdxModel),
					in.pos,
					in.vecQua,
					in.fAngle);
			}
			else if (type == fileTypeInfo.nPoleType)
			{ // 電柱配置
				auto pPole = CUtilityPole::Create(in.pos,
					in.vecQua,
					in.fAngle);
				pPole->SetParent(pMtxPlanet);
			}
			else if (type == fileTypeInfo.nPowerPlantType)
			{ // 発電所配置
				CPowerPlant::Create(in.pos,
					in.vecQua,
					in.fAngle);
			}
		}

		// ファイルを閉じる
		pFile->CloseFile();
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