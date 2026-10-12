//==================================================================================
// 
// 草(クォータニオン)クラスのヘッダーファイル [grassQuaternion.cpp]
// Author : TENMA SAITO
// Date   : 2026/9/18
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "grassQuaternion.h"
#include "matrix.h"

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CGrassQuaternion *CGrassQuaternion::Create(const Vector3 &offset,
	const Vector3 &vecQua,
	const float fAngle,
	const Vector2 &size)
{
	CGrassQuaternion *pGrassQua = new CGrassQuaternion;		// 生成した草へのポインタ
	if (pGrassQua != nullptr)
	{ // 生成できていれば、初期化
		pGrassQua->Init(offset, vecQua, fAngle, size);
	}

	return pGrassQua;
}

//==================================================================================
// --- 生成処理 (クォータニオン指定) ---
//==================================================================================
CGrassQuaternion *CGrassQuaternion::Create(const Vector3 &offset,
	const Quaternion &qua,
	const Vector2 &size)
{
	CGrassQuaternion *pGrassQua = new CGrassQuaternion;		// 生成した草へのポインタ
	if (pGrassQua != nullptr)
	{ // 生成できていれば、初期化
		pGrassQua->Init(offset, qua, size);
	}

	return pGrassQua;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CGrassQuaternion::CGrassQuaternion()
{ // メンバ変数をクリア
	D3DXQuaternionIdentity(&m_qua);
	m_pMtxParent = nullptr;
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CGrassQuaternion::~CGrassQuaternion()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CGrassQuaternion::Init(const Vector3 &offset,
	const Vector3 &vecQua,
	const float fAngle,
	const Vector2 &size)
{ // オフセットを保存
	m_offset = offset;
	
	// 親クラスの初期化
	CGrass::Init(VECTOR3_NULL, size, 0.0f, 0.0f);

	// クォータニオンを作成
	D3DXQuaternionRotationAxis(&m_qua,
		&vecQua,
		fAngle);

	// 親マトリックスに自身を設定
	CGrass::SetParent(&m_mtxWorld);

	return S_OK;
}

//==================================================================================
// --- 初期化処理 (クォータニオン指定) ---
//==================================================================================
HRESULT CGrassQuaternion::Init(const Vector3 &offset,
	const Quaternion &qua,
	const Vector2 &size)
{ // オフセットを保存
	m_offset = offset;

	// 親クラスの初期化
	CGrass::Init(VECTOR3_NULL, size, 0.0f, 0.0f);

	// クォータニオンを保存
	m_qua = qua;

	// 親クラスの親マトリックスに自身を設定
	CGrass::SetParent(&m_mtxWorld);

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CGrassQuaternion::Uninit(void)
{ // 親クラスの終了
	CGrass::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CGrassQuaternion::Update(void)
{ // 親クラスの更新
	CGrass::Update();
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CGrassQuaternion::Draw(void)
{ // マトリックスを初期化
	Mtx::Identity(&m_mtxWorld);

	// マトリックスを計算
	Mtx::CalcWorld(&m_mtxWorld, m_pMtxParent, m_offset, m_qua);

	// 親クラスの描画
	CGrass::Draw();
}