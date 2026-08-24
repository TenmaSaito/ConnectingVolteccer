//==================================================================================
// 
// モーション読み込みクラスのソースファイル [motionLoader.cpp]
// Author : TENMA SAITO
// Date   : 2026/8/18
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "motionLoader.h"
#include "motion.h"
#include "model.h"
#include "filestream.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_CAPACITY		(64)		// インスタンス生成時確保する配列の初期サイズ

//**********************************************************************************
// *** プロトタイプ宣言 ***
//**********************************************************************************
void LoadMotion(std::unique_ptr<CFileStream> &pFile, std::unique_ptr<CMotion> &pMotion);
void LoadKey(std::unique_ptr<CFileStream> &pFile,
	std::unique_ptr<CMotion> &pMotion,
	CMotion::INFO *pInfo,
	const int nKeyInfo);

//==================================================================================
// --- インスタンス取得処理 ---
//==================================================================================
CMotionLoader *CMotionLoader::GetInstance(void)
{
	static CMotionLoader instance;		// インスタンス
	return &instance;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMotionLoader::CMotionLoader()
{ // メンバ変数のクリア + バッファの事前確保
	m_vBuffer.reserve(DEFAULT_CAPACITY);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMotionLoader::~CMotionLoader()
{
}

//==================================================================================
// --- モーションの登録処理 ---
//==================================================================================
UINT CMotionLoader::Register(const std::string_view path)
{ // ファイル名がnullもしくはインデックスが無効値手前の場合失敗
	if (path.empty() == true) return INVALID_MOTION_ID;
	if (m_vBuffer.size() == INVALID_MOTION_ID - 1U) return INVALID_MOTION_ID;

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
UINT CMotionLoader::Register(const char *pPath)
{ // nullptrの場合、無効値を渡す
	if(pPath == nullptr) return INVALID_MOTION_ID;

	// 存在すれば正式に処理を呼び出す
	return Register(std::string_view(pPath));
}

//==================================================================================
// --- 読み込んだモーションの生成処理 ---
//==================================================================================
std::unique_ptr<CMotion> CMotionLoader::CreateMotion(const UINT uIdx) const
{ // 無効なインデックスならnullptrを返す
	if (uIdx >= m_vBuffer.size() || uIdx == INVALID_MOTION_ID) return nullptr;

	std::unique_ptr<CMotion> pMotion(new CMotion);		// 新規で作成されたモーションインスタンスへのポインタ
	if (pMotion != nullptr)
	{ // 生成されていれば、初期化後、値をコピー
		pMotion->Init();
		*pMotion = *m_vBuffer.at(uIdx).pMotion;
	}

	// 生成したモーションを返す
	return pMotion;
}

//==================================================================================
// --- 読み込んだモーションの破棄処理 ---
//==================================================================================
void CMotionLoader::Unload(void)
{
	for (auto &buf : m_vBuffer)
	{ // 読み込んだバッファの解放
		// モーションをクリア
		buf.pMotion->Uninit();
		buf.pMotion.reset();

		// ファイル名をクリア
		buf.sFilename.clear();
	}

	// 配列をクリア
	m_vBuffer.clear();
}

//==================================================================================
// --- モーションスクリプトの読み込み処理 ---
//==================================================================================
UINT CMotionLoader::Load(const std::string_view path)
{
	std::unique_ptr<CFileStream> pFile(new CFileStream);		// ファイルストリーム
	std::string line;					// 読み取った一行
	UINT uIdx = m_vBuffer.size();		// 今回のインデックス
	BUFFER buf;			// 読み込んだスクリプトの情報

	// 文字列にあらかじめバッファを確保しておく
	line.reserve(MAX_PATH);

	if (pFile == nullptr)
	{ // 生成失敗
		return INVALID_MOTION_ID;
	}

	// ファイルオープン
	if (pFile->OpenFile(path, false) == false)
	{ // ファイルオープン失敗
		return INVALID_MOTION_ID;
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
			return INVALID_MOTION_ID;
		}
	}

	// バッファ作成
	buf.sFilename = path;
	buf.pMotion = std::make_unique<CMotion>();

	while (1)
	{ // SCRIPTの走査ループ
		// 一行読み込み
		pFile->ReadString(line);
		if (line.empty() == true) continue;

		// コメント消去
		if(line.find('#') != std::string::npos) line.erase(line.begin() + line.find('#'), line.end());
		if (CFileStream::FindString(line, "MOTIONSET"))
		{ // MOTION読み込み開始位置を見つけた場合、ループ終了
			LoadMotion(pFile, buf.pMotion);
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
// --- モーションの読み込み処理 ---
//==================================================================================
void LoadMotion(std::unique_ptr<CFileStream> &pFile, std::unique_ptr<CMotion> &pMotion)
{
	std::string line;					// 読み取った一行
	CMotion::INFO info = {};			// モーション情報
	size_t strPos;			// 読み込み開始するオフセット
	int nKeyInfo = 0;		// 現在設定しているキー情報の番号
	int nLoop = 0;			// 読み込んだループの有無

	while (1)
	{ // モーションの読み込み
		// 一行読み取る
		pFile->ReadString(line);
		if (line.empty() == true) continue;

		// コメント消去
		if (line.find('#') != std::string::npos) line.erase(line.begin() + line.find('#'), line.end());

		if (CFileStream::FindString(line, "END_MOTIONSET"))
		{ // モーション情報の読み込み終了
			// モーション情報の設定及び番号の初期化
			pMotion->SetInfo(info);
			nKeyInfo = 0;
			break;
		}
		else if (CFileStream::FindString(line, "KEYSET"))
		{ // キー情報の設定
			LoadKey(pFile, pMotion, &info, nKeyInfo);

			// キー情報の番号を進める
			nKeyInfo++;
		}
		else if (CFileStream::FindString(line, "LOOP = ", &strPos, true))
		{ // ループの有無の読み込み
			nLoop = CFileStream::ToInt(&line.at(strPos));

			// 1以上ならループ有
			info.bLoop = nLoop > 0;
		}
		else if (CFileStream::FindString(line, "NUM_KEY = ", &strPos, true))
		{ // キー情報の総数のの読み込み
			info.nNumKey = CFileStream::ToInt(&line.at(strPos));
		}
	}
}

//==================================================================================
// --- キー情報の読み込み処理 ---
//==================================================================================
void LoadKey(std::unique_ptr<CFileStream> &pFile,
	std::unique_ptr<CMotion> &pMotion,
	CMotion::INFO *pInfo,
	const int nKeyInfo)
{
	std::string line;					// 読み取った一行
	size_t strPos;			// 読み込み開始するオフセット
	int nKey = 0;			// 現在設定しているキー要素の番号

	while (1)
	{ // 読み込みループ
		// 一行読み取る
		pFile->ReadString(line);
		if (line.empty() == true) continue;

		// コメント消去
		if (line.find('#') != std::string::npos) line.erase(line.begin() + line.find('#'), line.end());

		if (CFileStream::FindString(line, "END_KEYSET"))
		{ // キー情報の読み込み終了
			break;
		}
		else if (CFileStream::FindString(line, "KEY"))
		{ // キー情報の設定
			while (1)
			{ // 読み込みループ
				// 一行読み取る
				pFile->ReadString(line);
				if (line.empty() == true) continue;

				// コメント消去
				if (line.find('#') != std::string::npos) line.erase(line.begin() + line.find('#'), line.end());

				if (CFileStream::FindString(line, "END_KEY"))
				{ // キー要素の読み込み終了
					// キー要素の番号を進める
					nKey++;
					break;
				}
				else if (CFileStream::FindString(line, "POS =", &strPos, true))
				{ // 位置の読み込み
					pInfo->aKeyInfo[nKeyInfo].aKey[nKey].pos = CFileStream::ToVector3(&line.at(strPos));
				}
				else if (CFileStream::FindString(line, "ROT =", &strPos, true))
				{ // 角度の読み込み
					pInfo->aKeyInfo[nKeyInfo].aKey[nKey].rot = CFileStream::ToVector3(&line.at(strPos));
				}
			}
		}
		else if (CFileStream::FindString(line, "FRAME =", &strPos, true))
		{ // 再生フレーム数の読み込み
			pInfo->aKeyInfo[nKeyInfo].nFrame = CFileStream::ToInt(&line.at(strPos));
		}
	}
}