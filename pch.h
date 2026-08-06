//==================================================================================
// 
// プリコンパイル済みヘッダーファイル [pch.h]
// Author : TENMA SAITO
// Date   : 2026/5/15
// 
//==================================================================================
#ifndef _PCH_H_
#define _PCH_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include <Windows.h>						// Windowsシステムに必要
#include <imm.h>							// IME無効化に必要
#include "d3dx9.h"							// 描画処理に必要
#define DIRECTINPUT_VERSION		(0x0800)	// dinputの警告対処マクロ
#include "dinput.h"							// 入力処理に必要(先に上のマクロを定義する)
#include "xinput.h"							// ジョイパッド処理に必要
#include "xaudio2.h"						// サウンド処理に必要
#include <assert.h>							// アサーションに必要
#include <time.h>							// 時刻取得に必要

//**********************************************************************************
// *** ライブラリのリンク ***
//**********************************************************************************
#pragma comment(lib,"winmm.lib")			// システムの時刻取得に必要
#pragma comment(lib,"imm32.lib")			// IME無効化に必要
#pragma comment(lib,"d3d9.lib")				// 描画処理に必要
#pragma comment(lib,"d3dx9.lib")			// d3d9.libの拡張ライブラリ
#pragma comment(lib,"dxguid.lib")			// DirectXコンポーネント(部品)使用に必要
#pragma comment(lib,"dinput8.lib")			// 入力処理に必要
#pragma comment(lib,"xinput.lib")			// ジョイパッド処理に必要
#pragma comment(lib,"xaudio2.lib")			// 3Dオーディオに必要

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define SCREEN_WIDTH		(1280)			// ウィンドウの横幅
#define SCREEN_HEIGHT		(720)			// ウィンドウの縦幅
#define FVF_VERTEX_2D		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)		// 2D頂点フォーマット
#define FVF_VERTEX_3D		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)						// 3D頂点フォーマット
#define FVF_VERTEX_3D_MULTI	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEX2)		// マルチテクスチャ3D頂点フォーマット
#define VECTOR2_NULL		D3DXVECTOR2(0.0f, 0.0f)			// D3DXVECTOR2の0クリア
#define VECTOR2_ONE			D3DXVECTOR2(1.0f, 1.0f)			// D3DXVECTOR2の1クリア
#define VECTOR3_NULL		D3DXVECTOR3(0.0f, 0.0f, 0.0f)	// D3DXVECTOR3の0クリア
#define VECTOR3_ONE			D3DXVECTOR3(1.0f, 1.0f, 1.0f)	// D3DXVECTOR3の1クリア
#define WINDOW_MIDDLE		D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f)		// ウィンドウの真ん中
#define DOUBLE_PI			(D3DX_PI * 2.0f)				// 2π
#define HALF_PI				(D3DX_PI * 0.5f)				// 1/2π
#define QUARTER_PI			(D3DX_PI * 0.25f)				// 1/4π

//**********************************************************************************
//*** 頂点情報(2D)の構造体を定義 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// 頂点情報(x,y,z)
	float rhw;				// 座標変換用係数(1.0fで固定)
	D3DCOLOR col;			// 頂点カラー(R,G,B,a)
	D3DXVECTOR2 tex;		// テクスチャ座標(x,y)
} VERTEX_2D;

//**********************************************************************************
//*** 頂点情報(3D)の構造体を定義 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// 頂点情報(x,y,z)
	D3DXVECTOR3 nor;		// 法線ベクトル
	D3DCOLOR col;			// 頂点カラー(R,G,B,a)
	D3DXVECTOR2 tex;		// テクスチャ座標(x,y)
} VERTEX_3D;

//**********************************************************************************
//*** 頂点情報(3D MULTI)の構造体を定義 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// 頂点情報(x,y,z)
	D3DXVECTOR3 nor;		// 法線ベクトル
	D3DCOLOR col;			// 頂点カラー(R,G,B,a)
	D3DXVECTOR2 tex;		// テクスチャ座標(x,y)
	D3DXVECTOR2 texM;		// マルチテクスチャ座標(x,y)
} VERTEX_3D_MULTI;
#endif