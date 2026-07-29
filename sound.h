//==================================================================================
// 
// サウンドクラスのヘッダーファイル [sound.h]
// Author : TENMA SAITO
// Date   : 2026/5/17
// 
//==================================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define SOUND_VOLUME		(0.2f)		// 音量

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CSoundCallbacker;		// XAudio2のコールバッククラス

//**********************************************************************************
// *** サウンドクラス ***
//**********************************************************************************
class CSound
{
public:
	//******************************************************************************
	// *** サウンドの種類 ***
	//******************************************************************************
	typedef enum
	{
		LABEL_SAMPLE = 0,		// サンプル音源
		LABEL_MAX
	} LABEL;

	//*****************************************************************************
	// *** サウンド情報 ***
	//*****************************************************************************
	typedef struct
	{
		const char* pFilename;		// ファイル名
		int nCntLoop;				// ループカウント
	} INFO;

	CSound();
	~CSound();

	HRESULT Init(const HWND hWnd);
	void Uninit(void);
	void Play(const LABEL label);
	void Stop(const LABEL label);
	void Stop(void);
	bool GetPlay(const LABEL label) const;
	HRESULT GetError(const LABEL label) const;

private:
	HRESULT CheckChunk(const HANDLE hFile, const DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
	HRESULT ReadChunkData(const HANDLE hFile, void *pBuffer, const DWORD dwBuffersize, const DWORD dwBufferoffset);

	IXAudio2 *m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;			// マスターボイス
	IXAudio2SourceVoice *m_apSourceVoice[LABEL_MAX];	// ソースボイス
	CSoundCallbacker *m_apCallback[LABEL_MAX];			// コールバック
	BYTE *m_apDataAudio[LABEL_MAX];						// オーディオデータ
	DWORD m_aSizeAudio[LABEL_MAX];						// オーディオデータサイズ
	static const INFO m_aInfo[LABEL_MAX];				// サウンド情報
};
#endif