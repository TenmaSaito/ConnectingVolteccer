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
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <locale>

//**********************************************************************************
// *** マップ情報構造体 ***
//**********************************************************************************
#ifdef ENABLE_PLANET
struct CMap::IODATA
{
	int type;			// モデルの種類
	int nIdxModel;		// 建物のインデックス
	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 vecQua;	// 任意軸
	float fAngle;		// 角度
};
#else
struct CMap::IODATA
{
	int type;			// モデルの種類
	int nIdxModel;		// モデルのインデックス
	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 rot;	// 角度
};
#endif

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
{ // メンバ変数のクリア
	ZeroMemory(m_aBuildingPath, sizeof(m_aBuildingPath));
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMap::~CMap()
{
}

//==================================================================================
// --- ファイル名登録処理 ---
//==================================================================================
void CMap::Resister(const BUILDING type, const char *pPath)
{ // インデックス外ならアサーション
	assert(BUILDING_MAX > type && type >= 0);

	if (BUILDING_MAX > type && type >= 0)
	{ // パスをコピー
		strcpy_s(m_aBuildingPath[type], pPath);
	}
}

//==================================================================================
// --- 電柱設置処理 ---
//==================================================================================
void CMap::AddUtilityPole(const D3DXVECTOR3 &pos)
{
	CGame *pGame = CManager::GetInstance()->GetScene<CGame>();
	auto pPlanet = pGame->GetPlanet();
	D3DXVECTOR3 vecQua = VECTOR3_NULL;
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
void CMap::AddBulding(const BUILDING type, const D3DXVECTOR3 &pos)
{
	CGame *pGame = CManager::GetInstance()->GetScene<CGame>();
	auto pPlanet = pGame->GetPlanet();
	D3DXVECTOR3 vecQua = VECTOR3_NULL;
	float fAngle = 0.0f;

	// インデックス外ならアサーション
	assert(BUILDING_MAX > type && type >= 0);
	if ((BUILDING_MAX > type && type >= 0) == false) return;

#if 0
	// クォータニオンから軸と角度を求める
	D3DXQuaternionToAxisAngle(pPlanet->GetQuaternion(),
		&vecQua,
		&fAngle);

	// 角度反転
	fAngle *= -1;

	// 建物設置 + 親設定
	auto pObject = CObjectXQuaternion::Create(m_aBuildingPath[type],
		pos,
		vecQua,
		fAngle);
	pObject->SetParent(pPlanet->GetMatrix());
#endif
}

//==================================================================================
// --- マップ書き出し処理 ---
//==================================================================================
void CMap::Save(const char *pMapFile)
{
#ifdef ENABLE_PLANET
	std::vector<std::string> filepath;			// ファイルパス
	std::vector<CBuilding::TYPE> aIdxModel;		// モデルインデックス
	std::vector<IODATA> outData;			// 出力データ群
	long long flag = OBJTYPE_TO_BITFLAG(CObject::TYPE_POLE);
	flag |= OBJTYPE_TO_BITFLAG(CObject::TYPE_BUILDING);

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

	if (pFile->CreateFile(pMapFile, true, CFileStream::FLAG_OVERWRITE))
	{
		// 書き出し時に小数第2位まで書き出すことを指定
		(*pOfs) << std::fixed << std::setprecision(2);

#if 0
		// モデルの合計数を書き出し
		*pFile << outData.size() << '\n';
#else
		// モデルの合計数を書き出し
		*pFile << outData.size();
#endif

		for (const auto &out : outData)
		{ // モデルのデータ書き出し
#if 0
			*pFile << out.type << ' '
				<< out.nIdxModel << ' '
				<< out.pos.x << ' '
				<< out.pos.y << ' '
				<< out.pos.z << ' '
				<< out.vecQua.x << ' '
				<< out.vecQua.y << ' '
				<< out.vecQua.z << ' '
				<< out.fAngle << '\n';
#else
			*pFile << out.type
				<< out.nIdxModel
				<< out.pos.x
				<< out.pos.y
				<< out.pos.z
				<< out.vecQua.x
				<< out.vecQua.y
				<< out.vecQua.z
				<< out.fAngle;
#endif
		}

		// ファイルを閉じる
		pFile->CloseFile();
	}
#else
	std::vector<std::string> filepath;		// ファイルパス
	std::vector<IODATA> outData;			// 出力データ群
	int nIdxModel = 0;		// モデルのインデックス
	long long flag = OBJTYPE_TO_BITFLAG(CObject::TYPE_POLE);
	flag |= OBJTYPE_TO_BITFLAG(CObject::TYPE_XMODEL);

	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{
		CObject *pObject = CObject::GetTop(nCntPriority);		// 先頭
		while (pObject != nullptr)
		{ // nullptrになるまで走査
			CObject *pObjectNext = pObject->GetNext();		// 次のオブジェクトへのポインタ

			if (static_cast<bool>(FIND_BITFLAG_BY_OBJTYPE(flag, static_cast<long long>(pObject->GetType()))))
			{ // もし、書き出すタイプなら
				// ObjectXにキャスト
				CObjectX *pObjX = static_cast<CObjectX *>(pObject);

				// ファイルパス取得
				for (const auto &path : filepath)
				{
					if (strcmp(path.c_str(), pObjX->GetFileName()) == 0)
					{ // 保存済みならスキップ
						break;
					}

					nIdxModel++;		// インデックスを進める
				}

				if (nIdxModel == filepath.size())
				{ // 保存していないならパスを保存
					filepath.push_back(pObjX->GetFileName());
				}

				IODATA out = {};		// 出力データ

				// メモリクリア
				ZeroMemory(&out, sizeof(IODATA));

				// 情報設定
				out.type = pObject->GetType();
				out.nIdxModel = nIdxModel;
				out.pos = pObjX->GetPosition();
				out.rot = pObjX->GetRotation();

				// 書き出し情報に追加
				outData.push_back(out);
			}

			nIdxModel = 0;		// インデックスリセット

			pObject = pObjectNext;		// オブジェクトを進める
		}
	}

	// データ書き出し
	std::unique_ptr<CFileStream> pFile(new CFileStream);		// ファイルストリーム
	std::ofstream *pOfs = pFile->GetOutStream();		// 出力ストリームへのポインタ

	if (pFile->CreateFile(pMapFile, false, CFileStream::FLAG_OVERWRITE))
	{
		// 書き出し時に小数第2位まで書き出すことを指定
		(*pOfs) << std::fixed << std::setprecision(2);

		// パスの合計数を書き出し
		*pFile << filepath.size() << '\n';

		for (const auto &path : filepath)
		{ // パス書き出し
			*pFile << path << '\n';
		}

		// モデルの合計数を書き出し
		*pFile << outData.size() << '\n';

		for (const auto &out : outData)
		{ // モデルのデータ書き出し
			*pFile << out.type << ' '
				<< out.nIdxModel << ' '
				<< out.pos.x << ' '
				<< out.pos.y << ' '
				<< out.pos.z << ' '
				<< out.rot.x << ' '
				<< out.rot.y << ' '
				<< out.rot.z << '\n';
		}

		// ファイルを閉じる
		pFile->CloseFile();
	}
#endif
}

//==================================================================================
// --- マップ読み込み処理 ---
//==================================================================================
void CMap::Load(const char *pMapFile)
{
#ifdef ENABLE_PLANET
	int nNumModelIdx = 0;	// モデルのインデックスの数
	int nNumModel = 0;		// モデルの数
	int nIdxModel = 0;		// モデルのインデックス
	const D3DXMATRIX *pMtxPlanet = CManager::GetInstance()->GetScene<CGame>()->GetPlanet()->GetMatrix();

	// データ読み込み
	std::unique_ptr<CFileStream> pFile(new CFileStream);		// ファイルストリーム

	if (pFile->OpenFile(pMapFile, true))
	{ // ファイルが開けた場合
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
			if (type == CObject::TYPE_BUILDING)
			{ // 建物配置
				CBuilding::Create(static_cast<CBuilding::TYPE>(in.nIdxModel),
					in.pos,
					in.vecQua,
					in.fAngle);
			}
			else if (type == CObject::TYPE_POLE)
			{ // 電柱配置
				auto pPole = CUtilityPole::Create(in.pos,
					in.vecQua,
					in.fAngle);
				pPole->SetParent(pMtxPlanet);
			}
		}

		// ファイルを閉じる
		pFile->CloseFile();
	}
#else
	std::vector<std::string> filepath;		// ファイルパス
	int nNumModelIdx = 0;	// モデルのインデックスの数
	int nNumModel = 0;		// モデルの数
	int nIdxModel = 0;		// モデルのインデックス

	// データ読み込み
	std::unique_ptr<CFileStream> pFile(new CFileStream);		// ファイルストリーム

	if (pFile->OpenFile(pMapFile, false))
	{ // 開けた場合
		// ファイル数を読み込み
		pFile->Read(nNumModelIdx);

		// 改行文字を破棄
		(*pFile->GetInStream()).ignore(512, '\n');

		for (int nCntModelIdx = 0; nCntModelIdx < nNumModelIdx; nCntModelIdx++)
		{ // ファイル数分データを読み込み
			std::string path;
			pFile->Read(path);

			filepath.push_back(path);
		}

		// モデル数読み込み
		*pFile >> nNumModel;

		for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
		{ // モデル数分データ読み込み
			IODATA in = { 0 };		// 出力データ

			pFile->Read(in.type);
			pFile->Read(in.nIdxModel);
			pFile->Read(in.pos.x);
			pFile->Read(in.pos.y);
			pFile->Read(in.pos.z);
			pFile->Read(in.rot.x);
			pFile->Read(in.rot.y);
			pFile->Read(in.rot.z);

			// タイプ別でオブジェクトを配置
			CObject::TYPE type = static_cast<CObject::TYPE>(in.type);
			if (type == CObject::TYPE_XMODEL)
			{ // Xモデル配置
				CObjectX::Create(filepath[in.nIdxModel].c_str(),
					in.pos,
					in.rot);
			}
		}

		// ファイルを閉じる
		pFile->CloseFile();
	}
#endif
}