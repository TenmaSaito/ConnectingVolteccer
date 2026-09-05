//==================================================================================
// 
// サウンドクラスのソースファイル [sound.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/17
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "sound.h"

//**********************************************************************************
// *** コールバッククラス ***
//**********************************************************************************
class CSoundCallbacker : public IXAudio2VoiceCallback
{
public:
	CSoundCallbacker();
	~CSoundCallbacker();

	void STDMETHODCALLTYPE OnBufferEnd(void *pBufferContext) override;
	void STDMETHODCALLTYPE OnBufferStart(void *pBufferContext) override;
	void STDMETHODCALLTYPE OnLoopEnd(void *pBufferContext) override;
	void STDMETHODCALLTYPE OnStreamEnd(void) override;
	void STDMETHODCALLTYPE OnVoiceError(void *pBufferContext, const HRESULT Error) override;
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd(void) override;
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(const UINT32 BytesRequired) override;
	bool GetPlay(void) const;
	HRESULT GetError(void) const;

private:
	bool m_bPlay;			// 再生状態
	HRESULT m_error;		// 最後に起きたエラー
};

//**********************************************************************************
// *** 静的メンバ変数 ***
//**********************************************************************************
CSound::INFO const CSound::m_aInfo[CSound::LABEL_MAX] =		// サウンド情報
{
	{"data/SOUND/BGM/title.wav", -1},			// タイトル画面のBGM
	{"data/SOUND/BGM/game.wav", -1},			// ゲーム画面のBGM
	{"data/SOUND/BGM/result.wav", -1},			// 結果画面のBGM
	{"data/SOUND/SE/select.wav", 0},			// 選択音
	{"data/SOUND/SE/swing.wav", 0},				// 投擲音
	{"data/SOUND/SE/electric.wav", 0},			// 通電音
	{"data/SOUND/SE/electricLong.wav", 0},		// 通電音 (long ver)
	{"data/SOUND/SE/electricShock.wav", 0},		// 感電音
	{"data/SOUND/SE/walk.wav", -1},				// 歩行音
	{"data/SOUND/SE/gaugeup.wav", -1},			// パーセント上昇音
};

//**********************************************************************************
// *** サウンドクラス ***
//**********************************************************************************
//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CSound::CSound()
{ // メンバ変数のクリア
	m_pXAudio2 = nullptr;
	m_pMasteringVoice = nullptr;
	ZeroMemory(&m_apSourceVoice[0], sizeof(m_apSourceVoice));
	ZeroMemory(&m_apCallback[0], sizeof(m_apCallback));
	ZeroMemory(&m_apDataAudio[0], sizeof(m_apDataAudio));
	ZeroMemory(&m_aSizeAudio[0], sizeof(m_aSizeAudio));
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CSound::~CSound()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CSound::Init(const HWND hWnd)
{
	HRESULT hr;

	// COMライブラリの初期化
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "COMライブラリの初期化に失敗！", "警告！", MB_ICONWARNING);

		return E_FAIL;
	}

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}

	// マスターボイスの生成
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		if (m_pXAudio2 != nullptr)
		{ // NULLでは無ければ
			// XAudio2オブジェクトの開放
			m_pXAudio2->Release();
			m_pXAudio2 = nullptr;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}

	// サウンドデータの初期化
	for (int nCntSound = 0; nCntSound < LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile(m_aInfo[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);

			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// ファイルポインタを先頭に移動
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);

			return HRESULT_FROM_WIN32(GetLastError());
		}

		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// チャンクデータの読み込み
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		if (dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// チャンクデータの読み込み
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// データチェック
		hr = CheckChunk(hFile, 'atad', &m_aSizeAudio[nCntSound], &dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// チャンクデータの読み込み
		m_apDataAudio[nCntSound] = new BYTE[m_aSizeAudio[nCntSound]];
		if (m_apDataAudio[nCntSound] == nullptr)
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2-1)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		hr = ReadChunkData(hFile, m_apDataAudio[nCntSound], m_aSizeAudio[nCntSound], dwChunkPosition);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2-2)", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// コールバックのインスタンス生成
		m_apCallback[nCntSound] = new CSoundCallbacker;
		if (m_apCallback[nCntSound] == nullptr)
		{
			MessageBox(hWnd, "コールバックの生成に失敗！", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// ソースボイスの生成
		hr = m_pXAudio2->CreateSourceVoice(&m_apSourceVoice[nCntSound], &(wfx.Format), 0, XAUDIO2_DEFAULT_FREQ_RATIO,
			m_apCallback[nCntSound]);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);

			return S_FALSE;
		}

		// バッファの値設定
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_aSizeAudio[nCntSound];
		buffer.pAudioData = m_apDataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = m_aInfo[nCntSound].nCntLoop;

		// オーディオバッファの登録
		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);

		// 音量設定
		m_apSourceVoice[nCntSound]->SetVolume(SOUND_VOLUME);

		// ファイルをクローズ
		CloseHandle(hFile);
	}

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CSound::Uninit(void)
{
	for (int nCntSound = 0; nCntSound < LABEL_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound] != nullptr)
		{
			// 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);

			// ソースボイスの破棄
			m_apSourceVoice[nCntSound]->DestroyVoice();
			m_apSourceVoice[nCntSound] = nullptr;

			// コールバックインスタンスの破棄
			delete m_apCallback[nCntSound];
			m_apCallback[nCntSound] = nullptr;

			// オーディオデータの開放
			delete[] m_apDataAudio[nCntSound];
			m_apDataAudio[nCntSound] = nullptr;
		}
	}

	// マスターボイスの破棄
	if (m_pMasteringVoice != nullptr)
	{
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = nullptr;
	}

	if (m_pXAudio2 != nullptr)
	{
		// XAudio2オブジェクトの開放
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}

	// COMライブラリの終了処理
	CoUninitialize();
}

//==================================================================================
// --- セグメント再生処理 (再生中なら停止) ---
//==================================================================================
void CSound::Play(const LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[label];
	buffer.pAudioData = m_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_aInfo[label].nCntLoop;

	// 状態取得
	m_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_apSourceVoice[label]->Start(0);
}

//==================================================================================
// --- セグメント停止処理 (ラベル指定) ---
//==================================================================================
void CSound::Stop(const LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	m_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[label]->FlushSourceBuffers();
	}
}

//==================================================================================
// --- セグメント停止処理 (全て) ---
//==================================================================================
void CSound::Stop(void)
{ // 停止
	for (int nCntSound = 0; nCntSound < LABEL_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound] != nullptr)
		{ // 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);

			// オーディオバッファの削除
			m_apSourceVoice[nCntSound]->FlushSourceBuffers();
		}
	}
}

//==================================================================================
// --- セグメントの一時停止処理 (全て) ---
//==================================================================================
void CSound::Pause(const LABEL label)
{
	if (m_apSourceVoice[label] != nullptr)
	{ // 一時停止
		m_apSourceVoice[label]->Stop(0);
	}
}

//==================================================================================
// --- セグメントの一時停止処理 ---
//==================================================================================
void CSound::Pause(void)
{ // 一時停止
	for (int nCntSound = 0; nCntSound < LABEL_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound] != nullptr)
		{ // 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);
		}
	}
}

//==================================================================================
// --- セグメントのピッチ変更処理 ---
//==================================================================================
void CSound::SetPitch(const LABEL label, const float fPitch)
{
	m_apSourceVoice[label]->SetFrequencyRatio(fPitch);
}

//==================================================================================
// --- 再生状態取得処理 ---
//==================================================================================
bool CSound::GetPlay(const LABEL label) const
{
	bool bPlay = false;		// 再生状態

	if (m_apCallback[label] != nullptr)
	{ // NULLでなければ
		bPlay = m_apCallback[label]->GetPlay();
	}

	return bPlay;
}

//==================================================================================
// --- エラー取得処理 ---
//==================================================================================
HRESULT CSound::GetError(const LABEL label) const
{
	HRESULT hr = S_OK;		// エラー

	if (m_apCallback[label] != nullptr)
	{ // NULLでなければ
		hr = m_apCallback[label]->GetError();
	}

	return hr;
}

//==================================================================================
// --- チャンクのチェック処理 ---
//==================================================================================
HRESULT CSound::CheckChunk(const HANDLE hFile, const DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//==================================================================================
// --- チャンクデータの読み込み処理 ---
//==================================================================================
HRESULT CSound::ReadChunkData(const HANDLE hFile, void *pBuffer, const DWORD dwBuffersize, const DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}

//**********************************************************************************
// *** コールバッククラス ***
//**********************************************************************************
//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CSoundCallbacker::CSoundCallbacker()
{
	// メンバ変数をクリア
	m_bPlay = false;
	m_error = S_OK;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CSoundCallbacker::~CSoundCallbacker()
{
}

//==================================================================================
// --- バッファ―処理終了時呼び出し処理 ---
//==================================================================================
void STDMETHODCALLTYPE CSoundCallbacker::OnBufferEnd(void *pBufferContext)
{
	m_bPlay = false;
}

//==================================================================================
// --- バッファ―処理開始時呼び出し処理 ---
//==================================================================================
void STDMETHODCALLTYPE CSoundCallbacker::OnBufferStart(void *pBufferContext)
{
	m_bPlay = true;
}

//==================================================================================
// --- ループ終了時呼び出し処理 ---
//==================================================================================
void STDMETHODCALLTYPE CSoundCallbacker::OnLoopEnd(void *pBufferContext)
{
	m_bPlay = false;
}

//==================================================================================
// --- 再生終了時呼び出し処理 ---
//==================================================================================
void STDMETHODCALLTYPE CSoundCallbacker::OnStreamEnd(void)
{
	m_bPlay = false;
}

//==================================================================================
// --- エラー発生時呼び出し処理 ---
//==================================================================================
void STDMETHODCALLTYPE CSoundCallbacker::OnVoiceError(void *pBufferContext, const HRESULT Error)
{
	// エラーを保存
	m_error = Error;
}

//==================================================================================
// --- 音声の処理パス終了直後の呼び出し処理 ---
//==================================================================================
void STDMETHODCALLTYPE CSoundCallbacker::OnVoiceProcessingPassEnd(void)
{

}

//==================================================================================
// --- データを読み取る直前の呼び出し処理 ---
//==================================================================================
void STDMETHODCALLTYPE CSoundCallbacker::OnVoiceProcessingPassStart(const UINT32 BytesRequired)
{

}

//==================================================================================
// --- 再生状態取得処理 ---
//==================================================================================
bool CSoundCallbacker::GetPlay(void) const
{
	return m_bPlay;
}

//==================================================================================
// --- 直近のエラー取得処理 ---
//==================================================================================
HRESULT CSoundCallbacker::GetError(void) const
{
	return m_error;
}