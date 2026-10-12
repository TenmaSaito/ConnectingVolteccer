//==================================================================================
// 
// オブジェクトビルボード3D(自動拡縮)クラスのソースファイル [objectBillboard3DScaling.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/25
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "objectBillboard3DScaling.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CObjectBillboard3DScaling *CObjectBillboard3DScaling::Create(const Vector3 &pos,
	const Vector2 &size,
	const float fScalingVal,
	const int nLife)
{
	CObjectBillboard3DScaling *pBill3DScaling = new CObjectBillboard3DScaling;		// 生成したオブジェクトへのポインタ
	if (pBill3DScaling != nullptr)
	{ // 生成出来ていれば、初期化
		pBill3DScaling->Init(pos, size, fScalingVal, nLife);
	}

	return pBill3DScaling;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObjectBillboard3DScaling::CObjectBillboard3DScaling(const int nPriority) : CObjectBillboard3D(nPriority)
{ // メンバ変数をクリア
	m_defSize = VECTOR2_NULL;
	m_fScalingVal = 0.0f;
	m_nDefLife = 0;
	m_nCounterFrame = 0;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObjectBillboard3DScaling::~CObjectBillboard3DScaling()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CObjectBillboard3DScaling::Init(const Vector3 &pos,
	const Vector2 &size,
	const float fScalingVal,
	const int nLife)
{ // 引数の保存
	m_fScalingVal = fScalingVal;
	m_defSize = size;
	m_nDefLife = nLife;
	
	// 親クラスの初期化
	return CObjectBillboard3D::Init(pos, VECTOR3_NULL, size, nLife);
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CObjectBillboard3DScaling::Uninit(void)
{ // 親クラスの終了
	CObjectBillboard3D::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CObjectBillboard3DScaling::Update(void)
{ // ビルボードの拡縮を行う
	SetSize(m_defSize * (1.0f + (m_fScalingVal * m_nCounterFrame)));
	if ((1.0f + (m_fScalingVal * m_nCounterFrame)) <= 0.0f)
	{ // サイズが0.0fを下回ったら、終了
		Uninit();
	}
	else
	{ // 透明度変更
		const Color *pCol = GetColor();		// 色へのポインタ
		SetColor(Color(pCol->r, pCol->g, pCol->b, static_cast<float>(m_nDefLife - m_nCounterFrame) / static_cast<float>(m_nDefLife)));

		// 親クラスの更新
		CObjectBillboard3D::Update();
	}

	m_nCounterFrame++;		// フレームカウンタを増加
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CObjectBillboard3DScaling::Draw(void)
{ // ビルボード描画
	CObjectBillboard3D::Draw();
}