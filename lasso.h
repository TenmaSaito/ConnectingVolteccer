//==================================================================================
// 
// 投げ縄クラスのヘッダーファイル [lasso.h]
// Author : TENMA SAITO
// Date   : 2026/7/10
// 
//==================================================================================
#ifndef _LASSO_H_		// インクルードガード
#define _LASSO_H_

//
// INFO : 生成時にプレイヤーから任意軸と角度を取得
// 取得した任意軸を取得した角度に加算していった角度で回す
// 当たり判定はマトリックスを適用
//
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectXQuaternion.h"
#include <variant>

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CUtilityPole;
class CPowerPlant;

//**********************************************************************************
// *** 投げ縄クラス ***
//**********************************************************************************
class CLasso : public CObjectXQuaternion
{
public:
	CLasso();
	~CLasso();

	static CLasso *Create(const Vector3 &pos, CUtilityPole *pStart, CUtilityPole *pEnd);
	static CLasso *Create(const Vector3 &pos, CPowerPlant *pStart, CUtilityPole *pEnd);

	HRESULT Init(const Vector3 &pos, CUtilityPole *pStart, CUtilityPole *pEnd);
	HRESULT Init(const Vector3 &pos, CPowerPlant *pStart, CUtilityPole *pEnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	void UpdateTransform(void);

	std::variant<CPowerPlant*, CUtilityPole*> m_pStart;		// 開始位置のオブジェクト (発電所 or 電柱)
	CUtilityPole *m_pEnd;	// 最終的に接続される電柱
	float m_fSlerpTime;		// 球面線形補間の値
};
#endif
