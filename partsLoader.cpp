//==================================================================================
// 
// パーツ読み込みクラスのソースファイル [partsLoader.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/19
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "partsLoader.h"
#include "model.h"
#include "filestream.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_CAPACITY		(64)		// インスタンス生成時確保する配列の初期サイズ

//**********************************************************************************
// *** プロトタイプ宣言 ***
//**********************************************************************************
void LoadCharactor(std::unique_ptr<CFileStream> &rpFile, 
	CPartsLoader::BUFFER &rBuffer,
	const std::vector<std::string> &rsPathParts);

//==================================================================================
// --- インスタンス取得処理 ---
//==================================================================================
CPartsLoader *CPartsLoader::GetInstance(void)
{
	static CPartsLoader instance;		// インスタンス
	return &instance;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CPartsLoader::CPartsLoader()
{ // メンバ変数のクリア + バッファの事前確保
	m_vBuffer.reserve(DEFAULT_CAPACITY);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CPartsLoader::~CPartsLoader()
{
}

//==================================================================================
// --- モーションの登録処理 ---
//==================================================================================
UINT CPartsLoader::Register(const std::string_view path)
{ // ファイル名がnullもしくはインデックスが無効値手前の場合失敗
	if (path.empty() == true) return INVALID_PATRS_ID;
	if (m_vBuffer.size() == INVALID_PATRS_ID - 1U) return INVALID_PATRS_ID;

	// 既に読み込んでいないかを確認
	for (UINT uCntTexture = 0; uCntTexture < m_vBuffer.size(); uCntTexture++)
	{
		if (m_vBuffer.at(uCntTexture).sFilename == path)
		{ // 既に読み込み済みのテクスチャなら、そのインデックスを返す
			return uCntTexture;
		}
	}

	// 新規で読み込み
	return Load(path);
}

//==================================================================================
// --- モーションの登録処理 (nullptr対策) ---
//==================================================================================
UINT CPartsLoader::Register(const char *pPath)
{ // nullptrの場合、無効値を渡す
	if (pPath == nullptr) return INVALID_PATRS_ID;

	// 存在すれば正式に処理を呼び出す
	return Register(std::string_view(pPath));
}

//==================================================================================
// --- 読み込んだパーツの生成処理 ---
//==================================================================================
std::vector<std::unique_ptr<CModel>> CPartsLoader::CreateParts(const UINT uIdx) const
{ // 無効なインデックスならnullptrを返す
	if (uIdx >= m_vBuffer.size() || uIdx == INVALID_PATRS_ID) return std::vector<std::unique_ptr<CModel>>();

	auto &buffer = m_vBuffer.at(uIdx);			// 保存済みのバッファへの参照
	auto &source = buffer.vpParts;				// 保存済みのパーツ配列への参照
	std::vector<std::unique_ptr<CModel>> ret;	// 各パーツのポインタ

	// サイズを配列のサイズに拡張
	ret.reserve(source.size());

	for (const auto &pSourceParts : source)
	{ // 各パーツのコピーを生成
		std::unique_ptr<CModel> pParts(pSourceParts->CreateCopy());		// パーツのコピーを生成

		// 配列に追加
		ret.emplace_back(std::move(pParts));
	}

	// 各パーツの親パーツを設定
	for (UINT uCntParts = 0U; uCntParts < ret.size(); uCntParts++)
	{ // -1の場合スキップ
		UINT uIdxParent = buffer.vParentIdx.at(uCntParts);		// 親パーツのインデックス
		if (uIdxParent == -1) continue;

		// 親パーツを設定
		ret.at(uCntParts)->SetParent(ret.at(uIdxParent).get());
	}

	// 生成したパーツ配列のコピーを返す
	return ret;
}

//==================================================================================
// --- 読み込んだモデルの破棄処理 ---
//==================================================================================
void CPartsLoader::Unload(void)
{
	for (auto &buf : m_vBuffer)
	{ // 読み込んだバッファの解放
		for (auto &pParts : buf.vpParts)
		{ // パーツ配列の破棄
			pParts->Uninit();
			pParts.reset();
		}

		// パーツ配列クリア
		buf.vpParts.clear();

		// ファイル名をクリア
		buf.sFilename.clear();
	}

	// 配列をクリア
	m_vBuffer.clear();
}

//==================================================================================
// --- モーションスクリプトの読み込み処理 ---
//==================================================================================
UINT CPartsLoader::Load(const std::string_view path)
{
	std::unique_ptr<CFileStream> pFile(new CFileStream);		// ファイルストリーム
	std::string line;						// 読み取った一行
	UINT uIdx = m_vBuffer.size();			// 今回のインデックス
	std::vector<std::string> vPathParts;	// 各パーツのパス
	size_t strPos;		// 読み込み開始するオフセット
	BUFFER buf;			// 読み込んだスクリプトの情報

	// 文字列にあらかじめバッファを確保しておく
	line.reserve(MAX_PATH);

	if (pFile == nullptr)
	{ // 生成失敗
		return INVALID_PATRS_ID;
	}

	// ファイルオープン
	if (pFile->OpenFile(path, false) == false)
	{ // ファイルオープン失敗
		return INVALID_PATRS_ID;
	}

	while (1)
	{ // SCRIPTの走査ループ
		// 一行読み込み
		pFile->ReadString(line);
		if (line.empty() == true) continue;

		// コメント消去
		if (line.find('#') != std::string::npos) line.erase(line.begin() + line.find('#'), line.end());
		if (CFileStream::FindString(line, "SCRIPT"))
		{ // SCRIPT開始位置を見つけた場合、ループ終了
			break;
		}
		else if (pFile->IsEoF() == true)
		{ // ファイルを閉じて、無効値を返す
			pFile->CloseFile();
			return INVALID_PATRS_ID;
		}
	}

	// 一列消去
	line.clear();

	// バッファ作成
	buf.sFilename = path;

	while (1)
	{ // SCRIPTの走査ループ
		// 一行読み込み
		pFile->ReadString(line);
		if (line.empty() == true) continue;

		// コメント消去
		if (line.find('#') != std::string::npos) line.erase(line.begin() + line.find('#'), line.end());
		while (line.find('\t') != std::string::npos) line.erase(line.find('\t'), 1U);

		if (CFileStream::FindString(line, "MODEL_FILENAME = ", &strPos, true))
		{ // モデルパス読み込み
			vPathParts.push_back(&line.at(strPos));
		}
		else if (CFileStream::FindString(line, "CHARACTERSET"))
		{ // キャラクター読み込み開始位置を見つけた場合、読み込み開始
			LoadCharactor(pFile, buf, vPathParts);
		}
		else if (pFile->IsEoF() == true || CFileStream::FindString(line, "END_SCRIPT"))
		{ // ループ終了
			break;
		}
	}

	// 配列に追加
	m_vBuffer.push_back(std::move(buf));

	// ファイルを閉じて、インデックスを返す
	pFile->CloseFile();
	return uIdx;
}

//==================================================================================
// --- CHARACTOR部分の読み込み処理 ---
//==================================================================================
void LoadCharactor(std::unique_ptr<CFileStream> &rpFile, 
	CPartsLoader::BUFFER &rBuffer,
	const std::vector<std::string> &rsPathParts)
{
	std::string line;	// 読み取った一行
	size_t strPos;		// 読み込み開始するオフセット
	Vector3 pos = VECTOR3_NULL;		// オフセット位置
	Vector3 rot = VECTOR3_NULL;		// 角度
	int nIdxParent = -1;	// 親モデルのインデックス
	int nIdxModel = -1;		// モデルのインデックス
	int nCntModel = 0;		// 読み込んだモデルパス数

	while (1)
	{ // キャラクター読み込み
		// 一行読み込み
		rpFile->ReadString(line);
		if (line.empty() == true) continue;

		// コメント消去
		if (line.find('#') != std::string::npos) line.erase(line.begin() + line.find('#'), line.end());

		if (CFileStream::FindString(line, "END_CHARACTERSET"))
		{ // 読み込み終了
			break;
		}
		else if (CFileStream::FindString(line, "PARTSSET"))
		{ // パーツ読み込み開始
			while (1)
			{ // パーツ読み込み
				// 一行読み込み
				rpFile->ReadString(line);
				if (line.empty() == true) continue;

				// コメント消去
				if (line.find('#') != std::string::npos) line.erase(line.begin() + line.find('#'), line.end());

				if (CFileStream::FindString(line, "END_PARTSSET"))
				{ // 読み込み終了
					// パーツを生成
					rBuffer.vpParts.emplace_back(std::move(std::unique_ptr<CModel>(CModel::Create(rsPathParts[nIdxModel].c_str(),
						pos,
						rot))));

					// 親パーツのインデックスを保存
					rBuffer.vParentIdx.push_back(nIdxParent);

					// 生成したモデルの総数を増やして終了
					nCntModel++;
					break;
				}
				else if (CFileStream::FindString(line, "INDEX = ", &strPos, true))
				{ // モデルのインデックス読み込み
					nIdxModel = CFileStream::ToInt(&line.at(strPos));
				}
				else if (CFileStream::FindString(line, "PARENT = ", &strPos, true))
				{ // 親モデルのインデックス読み込み
					nIdxParent = CFileStream::ToInt(&line.at(strPos));
				}
				else if (CFileStream::FindString(line, "POS = ", &strPos, true))
				{ // オフセット座標読み込み
					pos = CFileStream::ToVector3(&line.at(strPos));
				}
				else if (CFileStream::FindString(line, "ROT = ", &strPos, true))
				{ // 角度読み込み
					rot = CFileStream::ToVector3(&line.at(strPos));
				}
			}
		}
	}
}