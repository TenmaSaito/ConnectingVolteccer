//==================================================================================
//
// ファイル入出力クラス！ [fliestream.h]
// Author : TENMA SAITO
// DATE : 2026/04/28
//
//==================================================================================
#ifndef _FILESTREAM_H_
#define _FILESTREAM_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include <iostream>
#include <fstream>
#include <string>

#define _NO_THROW_STREAM_ERROR

//**********************************************************************************
// *** ファイル入出力クラス ***
//**********************************************************************************
class CFileStream
{
public:
	//******************************************************************************
	// *** ファイルが存在した場合のフラグの種類 ***
	//******************************************************************************
	typedef enum
	{
		FLAG_OVERWRITE = 0,		// ファイルを初期化し上書き
		FLAG_CONTINUE,			// ファイルの続きから記入
		FLAG_CANCEL,			// 処理を取り消し
		FLAG_MAX
	} FLAG;

	CFileStream();
	CFileStream(const CFileStream &file) = delete;
	~CFileStream();

	bool OpenFile(const char *pFilename, const bool bBinary);
	bool CreateFile(const char *pFilename, const bool bBinary, const FLAG flag);
	void CloseFile(void);
	bool IsOpen(void) const;
	bool CheckFile(void) const;
	bool IsInStream(void) const;
	size_t GetSize(void) const;
	const char *GetFileName(void) const;
	std::ifstream *GetInStream(void) { return &m_Infile; }
	std::ofstream *GetOutStream(void) { return &m_Outfile; }

	template<class T> bool Read(T &data);
	template<class T> bool Read(T *pData, const int nArray);
	bool Read(std::string &string);
	bool IsEoF(void) const;

	template<class T> bool Write(const T &data);
	template<class T> bool Write(const T *data, const int nArray);
	bool Write(const std::string &string);

	CFileStream &operator=(const CFileStream &file) = delete;
	template<class T> const CFileStream &operator >> (T &data);
	template<class T, size_t num> const CFileStream &operator >> (T (&data)[num]);
	template<class T> CFileStream &operator << (const T &data);
	template<class T, size_t num> CFileStream &operator << (const T (&data)[num]);
	operator bool(void) const;

private:
	void CloseFileInStream(void);
	void CloseFileOutStream(void);
	bool IsOpenInStream(void) const;
	bool IsOpenOutStream(void) const;
	bool CheckFileInStream(void) const;
	bool CheckFileOutStream(void) const;
	void ScanSizeInStream(void);

	std::ifstream m_Infile;		// ファイル入力ストリーム
	std::ofstream m_Outfile;	// ファイル出力ストリーム
	std::string m_filename;		// 開いているファイル名
	size_t m_size;				// ファイルのサイズ
	bool m_bBinary;				// バイナリかどうか
	bool m_bInStream;			// ファイル入力かどうか
};

//==================================================================================
// --- ファイルからの情報取得処理 ---
//==================================================================================
template<class T> bool CFileStream::Read(T &data)
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

	if (m_bBinary == true)
	{ // バイナリファイルの場合
		// バイナリデータを読み込み
		m_Infile.read((char*)&data, sizeof(T));
	}
	else
	{ // テキストファイルの場合
		// ストリーム演算子で読み込み
		m_Infile >> data;
	}

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
// --- ファイルからの情報取得処理 (配列) ---
//==================================================================================
template<class T> bool CFileStream::Read(T *pData, const int nArray)
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

	if (m_bBinary == true)
	{ // バイナリファイルの場合
		// バイナリデータを配列サイズ分読み込み
		m_Infile.read((char*)pData, sizeof(T) * nArray);
	}
	else
	{ // テキストファイルの場合
		// 配列分データ読み込み
		for(int nCntRead = 0; nCntRead < nArray; nCntRead++)
		{ // スペース区切りで読み込み
			// ストリーム演算子で読み込み
			m_Infile >> pData[nCntRead];

			// EOFだったら終了
			if (IsEoF() == true) break;
		}
	}

	try
	{ // CheckFileの例外スロー用
		// ファイル読み込みのチェック
		bool bCheck = CheckFile();
		if (bCheck == false)
		{ // 読み込み時に問題が発生したため、falseを返す
			return false;
		}
	}
	catch (const std::runtime_error& error)
	{ // 例外がスローされた場合
		// スローされた文を表示
		std::cerr << error.what() << std::endl;

		// ファイルを閉じる
		CloseFile();

		// 読み込み関数の外部にも例外をスロー
		throw;
	}

	// 読み込み成功
	return true;
}

//==================================================================================
// --- ファイルへの情報出力処理 ---
//==================================================================================
template<class T> bool CFileStream::Write(const T &data)
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
		m_Outfile.write((char*)&data, sizeof(T));
	}
	else
	{ // テキストファイルの場合
		// ストリーム演算子で出力
		m_Outfile << data;
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
	m_size += sizeof(T);

	// 書き出し成功
	return true;
}

//==================================================================================
// --- ファイルへの情報出力処理 (配列) ---
//==================================================================================
template<class T> bool CFileStream::Write(const T *data, const int nArray)
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
		m_Outfile.write(data, sizeof(T) * nArray);
	}
	else
	{ // テキストファイルの場合
		// 配列分データ書き出し
		for (int nCntRead = 0; nCntRead < nArray; nCntRead++)
		{
			// ストリーム演算子で出力
			m_Outfile << data[nCntRead];

			// データの書き出し
			m_Outfile.flush();
		}
	}

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
	m_size += sizeof(T) * nArray;

	// 書き出し成功
	return true;
}

//==================================================================================
// --- 演算子のオーバーロード (挿入) ---
//==================================================================================
template<class T> CFileStream &CFileStream::operator<<(const T &data)
{
	// 書き出し処理を行う
	Write(data);

	// 連続挿入用に自身を返す
	return *this;
}

//==================================================================================
// --- 演算子のオーバーロード (配列挿入) ---
//==================================================================================
template<class T, size_t num> CFileStream& CFileStream::operator<<(const T (&data)[num])
{
	// 書き出し処理を行う
	Write(data, num);

	// 連続挿入用に自身を返す
	return *this;
}

//==================================================================================
// --- 演算子のオーバーロード (抽出) ---
//==================================================================================
template<class T> const CFileStream &CFileStream::operator>>(T &data)
{
	// 読み込み処理を行う
	Read(data);

	// 連続抽出用に自身を返す
	return *this;
}

//==================================================================================
// --- 演算子のオーバーロード (配列抽出) ---
//==================================================================================
template<class T, size_t num> const CFileStream& CFileStream::operator>>(T (&data)[num])
{
	// 読み込み処理を行う
	Read(data, num);

	// 連続抽出用に自身を返す
	return *this;
}

#endif // !_FILESTREAM_H_