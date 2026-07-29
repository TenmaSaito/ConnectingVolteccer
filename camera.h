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

	virtual void Init(const D3DXVECTOR3 &posV);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void SetCamera(void);
	bool IsVisible(const D3DXVECTOR3 &pos);
	void SetPosV(const D3DXVECTOR3 &posV) { m_posV = posV; }
	const D3DXVECTOR3 *GetPosV(void) const { return &m_posV; }
	void SetPosR(const D3DXVECTOR3 &posR) { m_posR = posR; }
	const D3DXVECTOR3 *GetPosR(void) const { return &m_posR; }
	void SetVecU(const D3DXVECTOR3 &vecU) { m_vecU = vecU; }
	const D3DXVECTOR3 *GetVecU(void) const { return &m_vecU; }
	void SetRotate(const D3DXVECTOR3 &rotate) { m_rot = rotate; }
	const D3DXVECTOR3 *GetRotate(void) const { return &m_rot; }
	D3DXVECTOR3 GetRay(void) const;
	D3DXVECTOR3 GetRayToScreen(const D3DXVECTOR2 &screenPos);
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
	static CCamera *m_apCamera[TYPE_MAX];		// 現在登録されているカメラへのポインタ
	static D3DVIEWPORT9 m_vpDef;	// デフォルトのビューポート
	static TYPE m_currentType;		// 現在設置されているカメラのタイプ
	static TYPE m_focusType;		// フルスクリーンにフォーカスしているカメラのタイプ
	D3DXMATRIX m_mtxProjection;		// プロジェクションマトリックス
	D3DXMATRIX m_mtxView;	// ビューマトリックス
	D3DXVECTOR3 m_posV;		// 視点	
	D3DXVECTOR3 m_posR;		// 注視点
	D3DXVECTOR3 m_vecU;		// 上方向ベクトル
	D3DXVECTOR3 m_rot;		// 向き
	D3DVIEWPORT9 m_vp;		// ビューポート設定
	TYPE m_type;			// カメラの種類
	bool m_bEnableOrtho;	// 正射影投影の有効化
};
#endif