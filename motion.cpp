//==================================================================================
// 
// モーションクラスのソースファイル [motion.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/8
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "motion.h"
#include "model.h"
#include "vec3math.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMotion::CMotion()
{ // メンバ変数をクリア
	ZeroMemory(m_aInfo, sizeof(m_aInfo));
	m_nNumAll = 0;
	m_nNumModel = 0;
	m_nType = 0;
	m_bLoop = false;
	m_nNumKey = 0;
	m_nKey = 0;
	m_nCounter = 0;
	m_bFinish = false;
	m_bBlend = false;
	m_nTypeBlend = 0;
	m_bLoopBlend = false;
	m_nNumKeyBlend = 0;
	m_nKeyBlend = 0;
	m_nFrameBlend = 0;
	m_nCounterBlend = 0;
	m_nCounterMotionBlend = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMotion::~CMotion()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
void CMotion::Init(void)
{
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMotion::Uninit(void)
{ // 一部変数をクリア
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMotion::Update(void)
{ // モーションの更新
	// 既にモーションが終了していればスキップ
	if (m_bFinish == true) return;

	INFO *pInfo = &m_aInfo[m_nType];				// モーション情報へのポインタ
	INFO *pInfoBlend = &m_aInfo[m_nTypeBlend];		// ブレンドモーション情報へのポインタ
	KEY_INFO *pKeyInfo = &pInfo->aKeyInfo[m_nKey];					// キー情報へのポインタ
	KEY_INFO *pKeyBlendInfo = &pInfoBlend->aKeyInfo[m_nKeyBlend];	// ブレンドキー情報へのポインタ

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{ // 全パーツの更新
		int nNext = (m_nKey + 1) % pInfo->nNumKey;			// 次のキーの値
		float fRateKey = (float)m_nCounter / (float)pKeyInfo->nFrame;	// モーションカウンター / 再生フレーム数
		KEY *pKey = &pKeyInfo->aKey[nCntModel];							// 現在のキー
		KEY *pKeyNext = &pInfo->aKeyInfo[nNext].aKey[nCntModel];		// 次のキー
		Vector3 diffPos = {};		// 位置の差分
		Vector3 UpdatePos = {};		// 更新する位置
		Vector3 diffRot = {};		// 角度の差分
		Vector3 UpdateRot = {};		// 更新する角度

		if (m_bBlend == false)
		{ // ブレンド無しの場合
			// 位置
			diffPos = pKeyNext->pos - pKey->pos;			// 位置の差分
			UpdatePos = pKey->pos + (diffPos * fRateKey);	// 更新後の位置

			// 角度
			diffRot = Vec3::FixedRotation(pKeyNext->rot - pKey->rot);			// 差分角度
			UpdateRot = Vec3::FixedRotation(pKey->rot + (diffRot * fRateKey));	// 更新後の角度
		}
		else
		{ // ブレンドありの場合
			Vector3 diffKeyPosCurrent = {};	// 現在のモーションの位置の差分
			Vector3 diffKeyRotCurrent = {};	// 現在のモーションの角度の差分
			Vector3 diffKeyPosBlend = {};	// ブレンドモーションの位置の差分
			Vector3 diffKeyRotBlend = {};	// ブレンドモーションの角度の差分
			Vector3 diffPosBlend = {};		// 位置の最終差分
			Vector3 diffRotBlend = {};		// 角度の最終差分
			float fRateKeyBlend = (float)m_nCounterMotionBlend / (float)pKeyBlendInfo->nFrame;		// モーションカウンター / 再生フレーム数
			int nNextBlend = (m_nKeyBlend + 1) % pInfoBlend->nNumKey;					// 次のキーの値
			KEY* pKeyNextBlend = &pInfoBlend->aKeyInfo[nNextBlend].aKey[nCntModel];		// 次のキー
			KEY* pKeyBlend = &pKeyBlendInfo->aKey[nCntModel];							// 現在のキー
			float fRateBlend = (float)m_nCounterBlend / (float)m_nFrameBlend;			// ブレンドの相対量

			// 現在のモーションの位置計算
			diffPos = pKeyNext->pos - pKey->pos;						// 位置の差分
			diffKeyPosCurrent = pKey->pos + (diffPos * fRateKey);		// 現在のモーションの差分位置

			// ブレンドモーションの位置計算
			diffPos = pKeyNextBlend->pos - pKeyBlend->pos;					// 位置の差分
			diffKeyPosBlend = pKeyBlend->pos + (diffPos * fRateKeyBlend);	// ブレンドモーションの差分位置

			// 二つの結果から最終的なブレンド位置を計算
			diffPosBlend = diffKeyPosBlend - diffKeyPosCurrent;				// モーション間の位置の差分
			UpdatePos = diffKeyPosCurrent + (diffPosBlend * fRateBlend);	// 最終的なモデルの位置

			// 現在のモーションの角度計算
			diffRot = Vec3::FixedRotation(pKeyNext->rot - pKey->rot);					// 差分角度
			diffKeyRotCurrent = Vec3::FixedRotation(pKey->rot + (diffRot * fRateKey));	// 現在のモーションの差分角度

			// ブレンドモーションの角度計算
			diffRot = Vec3::FixedRotation(pKeyNextBlend->rot - pKeyBlend->rot);					// 差分角度
			diffKeyRotBlend = Vec3::FixedRotation(pKeyBlend->rot + (diffRot * fRateKeyBlend));	// ブレンドモーションの差分角度

			// 二つの結果から最終的なブレンド角度を計算
			diffRotBlend = Vec3::FixedRotation(diffKeyRotBlend - diffKeyRotCurrent);			// モーション間の角度の差分
			UpdateRot = Vec3::FixedRotation(diffKeyRotCurrent + (diffRotBlend * fRateBlend));	// 最終的なモデルの角度
		}

		// 各変数にオフセットを適用
		UpdatePos += *m_ppModel[nCntModel]->GetPositionLocal();
		UpdateRot = Vec3::FixedRotation(UpdateRot + *m_ppModel[nCntModel]->GetRotationLocal());

		// 位置と角度を設定
		m_ppModel[nCntModel]->SetPosition(UpdatePos);
		m_ppModel[nCntModel]->SetRotation(UpdateRot);
	}

	if (m_bBlend == false)
	{ // ブレンド無しの場合
		// モーションカウンタ―を増加
		m_nCounter++;
		if (m_nCounter >= pKeyInfo->nFrame)
		{ // 指定フレーム数を超えた場合、キーを進める
			m_nCounter = 0;
			m_nKey++;
			if (m_nKey >= pInfo->nNumKey)
			{ // キーの総数に達した場合
				if (pInfo->bLoop == true)
				{ // ループするなら、キーをリセット
					m_nKey = 0;
				}
				else
				{ // ループしないなら、終了フラグを立てる
					m_bFinish = true;
				}
			}
		}
	}
	else
	{ // ブレンドありの場合
		// ブレンドモーションのカウンターを増加
		m_nCounterMotionBlend++;
		if (m_nCounterMotionBlend >= pKeyBlendInfo->nFrame)
		{ // フレーム数を超えた場合、キーを進める
			m_nKeyBlend = (m_nKeyBlend + 1) % m_nNumKeyBlend;
			m_nCounterMotionBlend = 0;
		}

		// ブレンドカウンターを増加
		m_nCounterBlend++;
		if (m_nCounterBlend >= m_nFrameBlend)
		{ // ブレンドフレーム数を超えた場合、ブレンド先のモーションをセット
			SetByBlend();
		}
	}
}

//==================================================================================
// --- モーション情報の設定処理 ---
//==================================================================================
void CMotion::SetInfo(const INFO info)
{ // モーション情報の保存及び総数の増加	
	m_aInfo[m_nNumAll] = info;
	m_nNumAll++;
}

//==================================================================================
// --- モデルへのポインタの設定処理 ---
//==================================================================================
void CMotion::SetModel(ModelArray ppModel, const int nNumModel)
{ // 各変数を保存
	m_ppModel = ppModel;
	m_nNumModel = nNumModel;
}

//==================================================================================
// --- モーションタイプの設定処理 ---
//==================================================================================
void CMotion::Set(const int nType)
{
	INFO *pInfo = &m_aInfo[nType];		// モーションへのポインタ

	// 各変数を初期化
	m_nCounter = 0;
	m_nKey = 0;
	m_nNumKey = pInfo->nNumKey;
	m_bLoop = pInfo->bLoop;
	m_bFinish = false;
	m_bBlend = false;
	m_nType = nType;

	// 各パーツに位置を設定
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{ // 位置と角度を設定
		KEY *pKey = &pInfo->aKeyInfo[m_nKey].aKey[nCntModel];		// キー要素へのポインタ
		m_ppModel[nCntModel]->SetPosition(pKey->pos + *m_ppModel[nCntModel]->GetPositionLocal());
		m_ppModel[nCntModel]->SetRotation(pKey->rot + *m_ppModel[nCntModel]->GetRotationLocal());
	}
}

//==================================================================================
// --- モーションタイプのブレンディングあり設定処理 ---
//==================================================================================
void CMotion::Set(const int nType, const int nFrameBlend)
{
	INFO *pInfoNext = &m_aInfo[nType];		// ブレンド先のモーションへのポインタ

	// 各ブレンド用変数を初期化
	m_nCounterBlend = 0;
	m_nCounterMotionBlend = 0;
	m_nKeyBlend = 0;
	m_nFrameBlend = nFrameBlend;
	m_nNumKeyBlend = pInfoNext->nNumKey;
	m_bLoopBlend = pInfoNext->bLoop;
	m_bBlend = true;
	m_nTypeBlend = nType;
	m_bFinish = false;
}

//==================================================================================
// --- ブレンド後のモーション設定処理 ---
//==================================================================================
void CMotion::SetByBlend(void)
{
	// ブレンド用変数から値を移行して初期化
	m_nCounter = m_nCounterMotionBlend;
	m_nKey = m_nKeyBlend;
	m_nType = m_nTypeBlend;
	m_bLoop = m_bLoopBlend;
	m_nNumKey = m_nNumKeyBlend;
	m_bFinish = false;
	m_bBlend = false;

	INFO *pInfo = &m_aInfo[m_nType];					// モーション情報へのポインタ
	KEY_INFO *pKeyInfo = &pInfo->aKeyInfo[m_nKey];		// キー情報へのポインタ

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{ // 全パーツの更新
		int nNext = (m_nKey + 1) % pInfo->nNumKey;			// 次のキーの値
		float fRateKey = (float)m_nCounter / (float)pKeyInfo->nFrame;	// モーションカウンター / 再生フレーム数
		KEY* pKey = &pKeyInfo->aKey[nCntModel];							// 現在のキー
		KEY* pKeyNext = &pInfo->aKeyInfo[nNext].aKey[nCntModel];		// 次のキー
		Vector3 diffPos = {};		// 位置の差分
		Vector3 UpdatePos = {};		// 更新する位置
		Vector3 diffRot = {};		// 角度の差分
		Vector3 UpdateRot = {};		// 更新する角度

		// 位置
		diffPos = pKeyNext->pos - pKey->pos;			// 位置の差分
		UpdatePos = pKey->pos + (diffPos * fRateKey);	// 更新後の位置

		// 角度
		diffRot = Vec3::FixedRotation(pKeyNext->rot - pKey->rot);			// 差分角度
		UpdateRot = Vec3::FixedRotation(pKey->rot + (diffRot * fRateKey));	// 更新後の角度

		// 各変数にオフセットを適用
		UpdatePos += *m_ppModel[nCntModel]->GetPositionLocal();
		UpdateRot = Vec3::FixedRotation(UpdateRot + *m_ppModel[nCntModel]->GetRotationLocal());

		// 位置と角度を設定
		m_ppModel[nCntModel]->SetPosition(UpdatePos);
		m_ppModel[nCntModel]->SetRotation(UpdateRot);
	}
}