//==================================================================================
// 
// チュートリアルクラスのヘッダーファイル [tutorial.h]
// Author : TENMA SAITO
// Date   : 2026/9/2
// 
//==================================================================================
#ifndef _TUTORIAL_H_		// インクルードガード
#define _TUTORIAL_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "scene.h"

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CPlayer;
class CCombo;
class CConnectingEvaluate;
class CThunderCamera;
class CTutorialManager;
class CPause;

//**********************************************************************************
// *** チュートリアルクラス ***
//**********************************************************************************
class CTutorial : public CSceneBase<CTutorial, CScene::MODE_TUTORIAL>
{
public:
	// チュートリアルの種類
	typedef enum
	{
		TYPE_CONNECT = 0,		// 繋げ方についてのチュートリアル
		TYPE_COMBO,				// コンボについてのチュートリアル
		TYPE_MAX
	} TYPE;

	CTutorial();
	~CTutorial();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Restart(std::string_view sMapPath);
	CPlayer *GetPlayer(void) const { return m_pPlayer; }
	CCombo *GetCombo(void) const { return m_pCombo; }
	CTutorialManager *GetTutorialManager(void) const { return m_pTutorialManager.get(); }

private:
	void Start(void);

	CPlayer *m_pPlayer;		// プレイヤーへのポインタ
	CCombo *m_pCombo;		// コンボ表示へのポインタ
	CThunderCamera *m_pThunderCam;			// 電流表示用カメラへのポインタ
	CConnectingEvaluate *m_pEvaluate;		// 接続時評価用クラスへのポインタ
	std::unique_ptr<CTutorialManager> m_pTutorialManager;		// チュートリアル管理インスタンスへのポインタ
	int m_nNumLightingHouse;			// 電線の接続で電気のついた家の総数
	int m_nCurrentConnectLighting;		// 今回の電線の接続で電気のついた家の数
	bool m_bCreateConnectEffect;		// 電気のついた家の数で表示が変わる演出を既に生成したか
};
#endif