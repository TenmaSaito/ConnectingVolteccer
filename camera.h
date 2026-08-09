//==================================================================================
// 
// カメラクラスのヘッダーファイル [camera.h]
// Author : TENMA SAITO
// Date   : 2026/5/8
// 
//==================================================================================
#ifndef _CAMERA_H_		// インクルードガード
#define _CAMERA_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_VP			D3DVIEWPORT9{0,0,SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f}	// デフォルトのビューポート

//**********************************************************************************
// *** カメラクラス ***
//**********************************************************************************
class CCamera
{
public:
	// カメラの種類
	typedef enum
	{
		TYPE_PLAYER = 0,		// プレイヤーが対象のカメラ
		TYPE_TITLE,				// タイトル画面のカメラ
		TYPE_GAME,				// ゲーム画面のカメラ(自由移動用)
		TYPE_GAMEOVER,			// ゲームオーバー画面のカメラ
		TYPE_GAMECLEAR,			// ゲームクリア画面のカメラ
		TYPE_MAX
	} TYPE;

	CCamera(const TYPE type);
	virtual ~CCamera();

	virtual void Init(const Vector3 &posV);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void SetCamera(void);
	bool IsVisible(const Vector3 &pos);
	void SetPosV(const Vector3 &posV) { m_posV = posV; }
	const Vector3 *GetPosV(void) const { return &m_posV; }
	void SetPosR(const Vector3 &posR) { m_posR = posR; }
	const Vector3 *GetPosR(void) const { return &m_posR; }
	void SetVecU(const Vector3 &vecU) { m_vecU = vecU; }
	const Vector3 *GetVecU(void) const { return &m_vecU; }
	void SetRotate(const Vector3 &rotate) { m_rot = rotate; }
	const Vector3 *GetRotate(void) const { return &m_rot; }
	Vector3 GetRay(void) const;
	Vector3 GetRayToScreen(const Vector2 &screenPos);
	void SetViewport(const D3DVIEWPORT9 &vp) { m_vp = vp; }
	const D3DVIEWPORT9 *GetViewport(void) const { return &m_vp; }
	void SetFocus(void) { m_focusType = m_type; }

	static void ReleaseAll(void);
	static void UpdateAll(void);
	static TYPE GetTarget(void);
	static void SetFocus(const TYPE type) { m_focusType = type; }
	static TYPE GetFocus(void) { return m_focusType; }
	static CCamera *Begin(const int nIdx);
	static void End(void);
	static CCamera *GetCamera(const TYPE type) { return m_apCamera[type]; }

protected:
	void Release(void);

private:
	inline static CCamera *m_apCamera[TYPE_MAX] = {};		// 現在登録されているカメラへのポインタ
	inline static D3DVIEWPORT9 m_vpDef = DEFAULT_VP;		// デフォルトのビューポート
	inline static TYPE m_currentType = CCamera::TYPE_PLAYER;	// 現在設置されているカメラのタイプ
	inline static TYPE m_focusType = CCamera::TYPE_MAX;			// フルスクリーンにフォーカスしているカメラのタイプ
	Matrix m_mtxProjection;		// プロジェクションマトリックス
	Matrix m_mtxView;	// ビューマトリックス
	Vector3 m_posV;		// 視点	
	Vector3 m_posR;		// 注視点
	Vector3 m_vecU;		// 上方向ベクトル
	Vector3 m_rot;		// 向き
	D3DVIEWPORT9 m_vp;		// ビューポート設定
	TYPE m_type;			// カメラの種類
	bool m_bEnableOrtho;	// 正射影投影の有効化
};
#endif