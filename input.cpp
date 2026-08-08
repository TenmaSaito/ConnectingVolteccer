//==================================================================================
// 
// 入力クラスのソースファイル [input.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/15
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "input.h"
#include "manager.h"
#include "vec3math.h"

//**********************************************************************************
// *** 静的メンバ変数 ***
//**********************************************************************************
LPDIRECTINPUT8 CInput::m_pInput = nullptr;			// 入力オブジェクト

//**********************************************************************************
// *** 入力クラス ***
//**********************************************************************************

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CInput::CInput()
{
	// メンバ変数のクリア
	m_pDevice = nullptr;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CInput::~CInput()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CInput::Init(const HINSTANCE hInstance, const HWND hWnd)
{
	HRESULT hr = S_OK;		// 関数の返り値

	if (m_pInput == nullptr)
	{ // 入力オブジェクトが生成されていなければ
		// dinputオブジェクトを生成
		hr = DirectInput8Create(hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&m_pInput,
			NULL);
	}

	return hr;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CInput::Uninit(void)
{
	// 入力デバイスの破棄
	if (m_pDevice != nullptr)
	{ // NULLではなかった場合
		// アクセス権を開放後、破棄
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	// 入力オブジェクトの破棄
	if (m_pInput != nullptr)
	{ // NULLではなかった場合
		m_pInput->Release();
		m_pInput = nullptr;
	}
}

//**********************************************************************************
// *** キーボードクラス ***
//**********************************************************************************
//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CInputKeyboard::CInputKeyboard()
{
	// メンバ変数のクリア
	memset(&m_aKeyState, 0, sizeof(m_aKeyState));
	memset(&m_aKeyStateTrigger, 0, sizeof(m_aKeyStateTrigger));
	memset(&m_aKeyStateRelease, 0, sizeof(m_aKeyStateRelease));
	memset(&m_nKeyStateRepeat, 0, sizeof(m_nKeyStateRepeat));
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CInputKeyboard::~CInputKeyboard()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CInputKeyboard::Init(const HINSTANCE hInstance, const HWND hWnd)
{
	HRESULT hr = S_OK;			// 処理結果

	// 親クラスの初期化処理
	hr = CInput::Init(hInstance, hWnd);
	if (FAILED(hr))
	{ // 入力オブジェクトの生成失敗
		MessageBox(hWnd, "入力オブジェクトの生成に失敗しました！", "Failed", MB_ICONERROR);
		return E_FAIL;
	}

	// キーボードデバイス作成
	hr = m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL);
	if (FAILED(hr))
	{ // 入力デバイスの生成失敗
		MessageBox(hWnd, "キーボードデバイスの生成に失敗しました！", "Failed", MB_ICONERROR);
		return E_FAIL;
	}

	// データフォーマットを設定
	hr = m_pDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	hr = m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	//キーボードへのアクセス権を獲得
	m_pDevice->Acquire();

	// 処理成功
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CInputKeyboard::Uninit(void)
{
	// 親クラスの終了処理
	CInput::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX];		// キーボードの入力情報
	HRESULT hr = S_OK;					// 処理結果
	int nCntKey;

	// 入力デバイスからデータを取得
	hr = m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0]);
	if (SUCCEEDED(hr))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{ // キーボード数分読み取り
			// キーボードのリリース情報を保存
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] & (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]));

			// キーボードのトリガー情報を保存
			m_aKeyStateTrigger[nCntKey] = ((aKeyState[nCntKey] ^ m_aKeyState[nCntKey]) & aKeyState[nCntKey]);
			if (m_aKeyState[nCntKey] != aKeyState[nCntKey])
			{ // 以前の状態と異なる場合は、リピートカウントリセット
				m_nKeyStateRepeat[nCntKey] = 0;
			}

			// キーボードのプレス情報を保存
			m_aKeyState[nCntKey] = aKeyState[nCntKey];
		}
	}
	else
	{ // データ取得失敗時
		// キーボードへのアクセス権を取得
		m_pDevice->Acquire();
	}
}

//==================================================================================
// --- プレス判定処理 ---
//==================================================================================
bool CInputKeyboard::GetPress(const int nKey)
{ // プレス状態を返す
	return (m_aKeyState[nKey] & 0x80) ? true : false;
}

//==================================================================================
// --- トリガー判定処理 ---
//==================================================================================
bool CInputKeyboard::GetTrigger(const int nKey)
{ // トリガー状態を返す
	return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//==================================================================================
// --- リリース判定処理 ---
//==================================================================================
bool CInputKeyboard::GetRelease(const int nKey)
{ // リリース状態を返す
	return (m_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//==================================================================================
// --- リピート判定処理 ---
//==================================================================================
bool CInputKeyboard::GetRepeat(const int nKey, const int nWaitPress, const int nInterval)
{
	// リピートカウントを増加
	m_nKeyStateRepeat[nKey]++;
	if (m_nKeyStateRepeat[nKey] < nWaitPress)
	{ // リピートカウントが移行待機時間内なら、トリガーの判定を返す
		return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
	}
	else
	{ // リピートカウントがプレスへの移行待機時間を超えたなら
		if (m_nKeyStateRepeat[nKey] % nInterval == 0)
		{ // インターバル分時間が経ったら、プレスの判定を返す
			return (m_aKeyState[nKey] & 0x80) ? true : false;
		}
		else
		{ // インターバル中なら失敗
			return false;
		}
	}
}

//**********************************************************************************
// *** マウスクラス ***
//**********************************************************************************

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CInputMouse::CInputMouse()
{
	// メンバ変数のクリア
	memset(&m_CurrentMouseState, 0, sizeof(m_CurrentMouseState));
	memset(&m_PrevMouseState, 0, sizeof(m_PrevMouseState));
	memset(&m_aMouseState, 0, sizeof(m_aMouseState));
	memset(&m_aMouseStateTrigger, 0, sizeof(m_aMouseStateTrigger));
	memset(&m_aMouseStateRelease, 0, sizeof(m_aMouseStateRelease));
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CInputMouse::~CInputMouse()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CInputMouse::Init(const HINSTANCE hInstance, const HWND hWnd)
{
	HRESULT hr = S_OK;			// 処理結果

	// 親クラスの初期化処理
	hr = CInput::Init(hInstance, hWnd);
	if (FAILED(hr))
	{ // 入力オブジェクトの生成失敗
		MessageBox(hWnd, "入力オブジェクトの生成に失敗しました！", "Failed", MB_ICONERROR);
		return E_FAIL;
	}

	// マウスデバイス作成
	hr = m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, NULL);
	if (FAILED(hr))
	{ // 入力デバイスの生成失敗
		MessageBox(hWnd, "マウスデバイスの生成に失敗しました！", "Failed", MB_ICONERROR);
		return E_FAIL;
	}

	// データフォーマットを設定
	hr = m_pDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	hr = m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	//キーボードへのアクセス権を獲得
	m_pDevice->Acquire();

	// 処理成功
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CInputMouse::Uninit(void)
{
	// 親クラスの終了処理
	CInput::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CInputMouse::Update(void)
{
	BYTE aMouseState[KEY_MAX];		// マウスの入力情報
	HRESULT hr = S_OK;				// 処理結果

	// 更新前に過去の入力情報(マウス)を保存
	m_PrevMouseState = m_CurrentMouseState;

	// 現在のマウスの入力情報を取得
	hr = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_CurrentMouseState);
	if (SUCCEEDED(hr))
	{ // 取得成功
		for (int nCntMouse = 0; nCntMouse < KEY_MAX; nCntMouse++)
		{ // マウスの数分読み込み
			// 現在の情報を取得
			aMouseState[nCntMouse] = m_CurrentMouseState.rgbButtons[nCntMouse];

			// マウスのトリガー情報を取得
			m_aMouseStateTrigger[nCntMouse] = ((aMouseState[nCntMouse] ^ m_aMouseState[nCntMouse]) & aMouseState[nCntMouse]);
			
			// マウスのリリース情報を取得
			m_aMouseStateRelease[nCntMouse] = (m_aMouseState[nCntMouse] & (m_aMouseState[nCntMouse] ^ aMouseState[nCntMouse]));
			
			// マウスのプレス情報を取得
			m_aMouseState[nCntMouse] = aMouseState[nCntMouse];
		}
	}
	else
	{ // 取得失敗時
		// マウスへのアクセス権を取得
		m_pDevice->Acquire();
	}
}

//==================================================================================
// --- プレス判定処理 ---
//==================================================================================
bool CInputMouse::GetPress(const int nKey)
{ // プレス状態を返す
	return (m_aMouseState[nKey] & 0x80) ? true : false;
}

//==================================================================================
// --- トリガー判定処理 ---
//==================================================================================
bool CInputMouse::GetTrigger(const int nKey)
{ // トリガー状態を返す
	return (m_aMouseStateTrigger[nKey] & 0x80) ? true : false;
}

//==================================================================================
// --- リリース判定処理 ---
//==================================================================================
bool CInputMouse::GetRelease(const int nKey)
{ // リリース状態を返す
	return (m_aMouseStateRelease[nKey] & 0x80) ? true : false;
}

//==================================================================================
// --- マウスのスクリーン座標取得処理 ---
//==================================================================================
Vector3 CInputMouse::GetPositionInScreen(void)
{
	POINT position = {};		// マウスの座標

	// マウスのスクリーン座標を取得
	if (GetCursorPos(&position) != TRUE)
	{ // 取得失敗
		// マウスへのアクセス権を取得
		m_pDevice->Acquire();
	}
	
	return Vector3(static_cast<float>(position.x), static_cast<float>(position.y), 0.0f);
}

//==================================================================================
// --- マウスのクライアント座標取得処理 ---
//==================================================================================
Vector3 CInputMouse::GetPositionInClient(void)
{
	HWND hWnd = CManager::GetInstance()->GetWindowHandle();		// ウィンドウハンドルを取得
	RECT rectCurrent;	// スクリーンの大きさ
	POINT position;		// マウスの座標
	Vector3 posCursor;		// ウィンドウ上のマウス座標

	// マウスのスクリーン座標を取得
	if (GetCursorPos(&position) != TRUE)
	{ // 取得失敗
		// マウスへのアクセス権を取得
		m_pDevice->Acquire();
	}
	else
	{ // 取得成功
		// 取得した座標をウィンドウ内座標に変換
		ScreenToClient(hWnd, &position);
	}

	// 現在のウィンドウサイズを取得
	GetClientRect(hWnd, &rectCurrent);

	posCursor.x = SCREEN_WIDTH * ((float)position.x / (rectCurrent.right - rectCurrent.left));
	posCursor.y = SCREEN_HEIGHT * ((float)position.y / (rectCurrent.bottom - rectCurrent.top));

	// 座標を正規化
	posCursor = Vec3::Clamp(posCursor, VECTOR3_NULL, Vector3(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 0.0f));

	return posCursor;
}