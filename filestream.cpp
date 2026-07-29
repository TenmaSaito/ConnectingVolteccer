//==================================================================================
//
// ファイル入出力クラスの作成！ [fliestream.cpp]
// Author : TENMA SAITO
//
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "filestream.h"
#include <iostream>

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CFileStream::CFileStream()
{
	// メンバ変数をクリア
	m_size = 0;
	m_bBinary = false;
	m_bInStream = false;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CFileStream::~CFileStream()
{
	// ファイルクローズ
	CloseFile();
}

//==================================================================================
// --- ファイルオープン処理 ---
//==================================================================================
bool CFileStream::OpenFile(const char *pFilename, const bool bBinary)
{
	// ファイルを閉じる
	CloseFile();

	// ファイルを入力ストリームに設定
	m_bInStream = true;

	// バイナリフラグによってオープンの仕方を変える
	if (bBinary)
	{ // バイナリファイルのオープン
		m_Infile.open(pFilename, std::ios::binary);
		
		// バイナリフラグを立てる
		m_bBinary = true;
	}
	else
	{ // テキストファイルのオープン
		m_Infile.open(pFilename);

		// バイナリフラグを下す
		m_bBinary = false;
	}

	if (IsOpen() == false)
	{ // ファイルのオープンに失敗
		return false;
	}

	// ファイルのサイズを取得
	ScanSizeInStream();

	// ファイル名を保存
	m_filename = pFilename;

	// 成功
	return true;
}

//==================================================================================
// --- ファイル作成処理 ---
//==================================================================================
bool CFileStream::CreateFile(const char* pFilename, const bool bBinary, const FLAG flag)
{
	std::ios::openmode mode = std::ios::out;	// openフラグ
	size_t size = 0;							// ファイルサイズの一時保存先

	// ファイルを閉じる
	CloseFile();

	// ファイルのサイズをリセット
	m_size = 0;

	// FLAGによってmodeにフラグを追加
	switch (flag)
	{
	// 上書きの場合
	case FLAG_OVERWRITE:

		// 上書きフラグ追加
		mode |= std::ios::trunc;

		break;

	// 追記の場合
	case FLAG_CONTINUE:

		// 一度ファイルを入力ストリームで開いて、存在するか確認
		if (OpenFile(pFilename, bBinary) == true)
		{ // ファイルが開けた場合(ファイルが存在する場合)
			// ファイルのサイズを取得して、保存
			size = GetSize();

			// ファイルを閉じる
			CloseFile();
		}

		// 保存していたファイルサイズを代入
		m_size = size;

		// 追記フラグ追加
		mode |= std::ios::app;

		break;

	// 取り消しの場合
	case FLAG_CANCEL:

		// 一度ファイルを入力ストリームで開いて、存在するか確認
		if (OpenFile(pFilename, bBinary) == true)
		{ // ファイルが開けた場合(ファイルが存在する場合)
			// ファイルを閉じる
			CloseFile();

			// 処理を中断する
			return false;
		}

		break;

	default:
		break;
	}

	// ファイルを出力ストリームに設定
	m_bInStream = false;

	// バイナリフラグによって作成の仕方を変える
	if (bBinary)
	{ // バイナリファイルの作成
		// バイナリフラグ追加
		mode |= std::ios::binary;

		// バイナリフラグを立てる
		m_bBinary = true;
	}
	else
	{ // テキストファイルの作成
		// バイナリフラグを下す
		m_bBinary = false;
	}

	// ファイル作成
	m_Outfile.open(pFilename, mode);

	if (IsOpen() == false)
	{ // ファイルの作成に失敗
		return false;
	}

	// ファイル名を保存
	m_filename = pFilename;

	// 成功
	return true;
}

//==================================================================================
// --- ファイルクローズ処理 ---
//==================================================================================
void CFileStream::CloseFile(void)
{
	// ファイルのチェック
	if (IsOpen() == true)
	{ // ファイルが開かれていれば
		// ストリームの種類で分岐
		if (m_bInStream == true)
		{ // 入力ストリームの場合、ifStreamのクローズ処理を呼ぶ
			CloseFileInStream();
		}
		else
		{ // 出力ストリームの場合、ofStreamのクローズ処理を呼ぶ
			CloseFileOutStream();
		}

		// ファイルサイズをリセット
		m_size = 0;

		// ファイル名をリセット
		m_filename.clear();
	}
}

//==================================================================================
// --- ファイルのオープン確認処理 ---
//==================================================================================
bool CFileStream::IsOpen(void) const
{
	// ストリームの種類で分岐
	if (m_bInStream == true)
	{ // 入力ストリームの場合、ifStreamのファイル確認処理を呼ぶ
		return IsOpenInStream();
	}
	else
	{ // 出力ストリームの場合、ofStreamのファイル確認処理を呼ぶ
		return IsOpenOutStream();
	}
}

//==================================================================================
// --- ファイル状態の確認処理 ---
//==================================================================================
bool CFileStream::CheckFile(void) const
{
	try
	{
		bool bCheck = false;		// 勝っク人の結果
		// ストリームの種類で分岐
		if (m_bInStream == true)
		{ // 入力ストリームの場合、ifStreamのファイル状態確認処理を呼ぶ
			bCheck = CheckFileInStream();
		}
		else
		{ // 出力ストリームの場合、ofStreamのファイル状態確認処理を呼ぶ
			bCheck = CheckFileOutStream();
		}

		return bCheck;
	}
	catch (const std::runtime_error &error)
	{ // 受け取った例外を関数外へスロー
		throw error;
	}
}

//==================================================================================
// --- 開かれているストリームの取得処理 ---
//==================================================================================
bool CFileStream::IsInStream(void) const
{
	return m_bInStream;
}

//==================================================================================
// --- EOFの確認処理 ---
//==================================================================================
bool CFileStream::IsEoF(void) const
{
	// ファイルのオープン確認
	if (IsOpen() == false)
	{ // ファイルが開かれていなければ、EOF扱い
		return true;
	}

	// ファイルのEOFを返す
	return m_Infile.eof();
}

//==================================================================================
// --- ファイルのサイズ取得処理 ---
//==================================================================================
size_t CFileStream::GetSize(void) const
{
	return m_size;
}

//==================================================================================
// --- ファイル名取得処理 ---
//==================================================================================
const char *CFileStream::GetFileName(void) const
{
	return m_filename.c_str();
}

//==================================================================================
// --- 1行読み込み処理 ---
//==================================================================================
bool CFileStream::Read(std::string &string)
{
	// EOFの確認
	if (IsEoF() == true)
	{ // EOFの場合、処理失敗
		return false;
	}

	// 入力ストリームかどうか確認
	if (IsInStream() == false)
	{ // 出力ストリームだった場合、失敗
		return false;
	}

	// 文字列をクリア
	string.clear();

	// 1行読み取り
	std::getline(m_Infile, string);

	try
	{ // CheckFileの例外スロー用
		// ファイル読み込みのチェック
		bool bCheck = CheckFile();
		if (bCheck == false)
		{ // 読み込み時に問題が発生したため、falseを返す
			return false;
		}
	}
	catch (const std::runtime_error &error)
	{ // 例外がスローされた場合
		// スローされた文を表示
		std::cerr << error.what() << std::endl;

		// ファイルを閉じる
		CloseFile();

		// 読み込み関数の外部にも例外をスロー
		throw;
	}

	// 成功
	return true;
}

//==================================================================================
// --- 1行書き出し処理 ---
//==================================================================================
bool CFileStream::Write(const std::string& string)
{
	// ファイルが開かれているか確認
	if (IsOpen() == false)
	{ // 開かれていない場合、失敗
		return false;
	}

	// 出力ストリームかどうか確認
	if (IsInStream() == true)
	{ // 入力ストリームだった場合、失敗
		return false;
	}

	if (m_bBinary == true)
	{ // バイナリファイルの場合
		// バイナリデータとして書き出し
		m_Outfile.write(string.data(), string.size());
	}
	else
	{ // テキストファイルの場合
		// 一行書き出し
		m_Outfile << string;
	}

	// データの書き出し
	m_Outfile.flush();

	try
	{ // CheckFileの例外スロー用
		// ファイル書き出しのチェック
		bool bCheck = CheckFile();
		if (bCheck == false)
		{ // 書き込み時に問題が発生したため、falseを返す
			return false;
		}
	}
	catch (const std::runtime_error& error)
	{ // 例外がスローされた場合
		// スローされた文を表示
		std::cerr << error.what() << std::endl;

		// ファイルを閉じる
		CloseFile();

		// 書き込み関数の外部にも例外をスロー
		throw;
	}

	// 書き出したサイズ分加算
	m_size += string.size();

	return true;
}

//==================================================================================
// --- ファイルクローズ処理 (ifStream) ---
//==================================================================================
void CFileStream::CloseFileInStream(void)
{
	// ファイルをクローズ
	m_Infile.close();
}

//==================================================================================
// --- ファイルクローズ処理 (ofStream) ---
//==================================================================================
void CFileStream::CloseFileOutStream(void)
{
	// ファイルをクローズ
	m_Outfile.close();
}

//==================================================================================
// --- ファイルの確認処理 (ifStream) ---
//==================================================================================
bool CFileStream::IsOpenInStream(void) const
{ // ifStreamのファイル確認
	return m_Infile.is_open();
}

//==================================================================================
// --- ファイルの確認処理 (ofStream) ---
//==================================================================================
bool CFileStream::IsOpenOutStream(void) const
{ // ofStreamのファイル確認
	return m_Outfile.is_open();
}

//==================================================================================
// --- ファイル状態の確認処理 (ifStream) ---
//==================================================================================
bool CFileStream::CheckFileInStream(void) const
{
	if (IsOpen() == false)
	{ // ファイルが開かれていなければ、失敗
		return false;
	}

	// ファイル読み込み状況のチェック
	if (m_Infile.bad() == true)
	{ // ファイル読み込み時に致命的なエラーが発生した場合
#ifndef _NO_THROW_STREAM_ERROR		// CheckFileによる例外スローを無効にする
		throw std::runtime_error("ファイル読み込み時に致命的なエラーが発生しました！");
#else
		return false;
#endif
	}
	else if (m_Infile.fail() == true)
	{ // ファイル読み込み時に失敗した場合
		return false;
	}
	else
	{ // 問題ないなら成功
		return true;
	}
}

//==================================================================================
// --- ファイル状態の確認処理 (ofStream) ---
//==================================================================================
bool CFileStream::CheckFileOutStream(void) const
{
	if (IsOpen() == false)
	{ // ファイルが開かれていなければ、失敗
		return false;
	}

	// ファイル書き込み状況のチェック
	if (m_Outfile.bad() == true)
	{ // ファイル書き込み時に致命的なエラーが発生した場合
#ifndef _NO_THROW_STREAM_ERROR		// CheckFileによる例外スローを無効にする
		throw std::runtime_error("ファイル書き込み時に致命的なエラーが発生しました！");
#else
		return false;
#endif
	}
	else if (m_Outfile.fail() == true)
	{ // ファイル書き込み時に失敗した場合
		return false;
	}
	else if (m_Outfile.good() == true)
	{ // 問題ないなら成功
		return true;
	}
	else
	{ // 理由不明な失敗
		return false;
	}
}

//==================================================================================
// --- 読み込み時ファイルサイズ取得処理 ---
//==================================================================================
void CFileStream::ScanSizeInStream(void)
{
	// ファイルのオープン確認
	if (IsOpen() == false)
	{ // ファイルが開かれていなければ、サイズをリセット
		m_size = 0;
	}

	// ファイルの現在位置を取得
	auto filePos = m_Infile.tellg();

	// ファイルの位置を終端までずらす
	m_Infile.seekg(0, std::ios::end);

	// ファイルのサイズを求める
	m_size = static_cast<size_t>(m_Infile.tellg());

	// ファイルの位置を戻す
	m_Infile.seekg(filePos);
}

//==================================================================================
// --- 演算子のオーバーロード (bool値への変換) ---
//==================================================================================
CFileStream::operator bool(void) const
{
	return IsOpen() && IsEoF();
}