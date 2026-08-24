//==================================================================================
// 
// オブジェクトビルボードクラスのヘッダーファイル [objectBillboard.h]
// Author : TENMA SAITO
// Date   : 2026/6/1
// 
//==================================================================================
#ifndef _OBJECTBILLBOARD_H_		// インクルードガード
#define _OBJECTBILLBOARD_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"
#include <functional>

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_OBJBILL_PRIORITY	(6)							// objBillの基本優先順位
#define DEFAULT_OBJBILL_SIZE		Vector2(100.0f, 100.0f)		// obj3Dの基本サイズ

//**********************************************************************************
// *** オブジェクトビルボードクラス ***
//**********************************************************************************
class CObjectBillboard : public CObject
{
public:
	static CObjectBillboard *Create(const Vector3 &pos = VECTOR3_NULL,
		const Vector3 &move = VECTOR3_NULL,
		const Vector2 &size = DEFAULT_OBJBILL_SIZE,
		const int nLife = INT_MAX);

	CObjectBillboard(const int nPriority = DEFAULT_OBJBILL_PRIORITY);
	~CObjectBillboard();

	HRESULT Init(const Vector3 &pos, 
		const Vector3 &move,
		const Vector2 &size,
		const int nLife);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetPosition(const Vector3 &position) { m_pos = position; }
	Vector3 GetPosition(void) const { return m_pos; }
	void SetSize(const Vector2 &size);
	Vector2 GetSize(void) const { return m_size; }
	void SetColor(const Color &color);
	const Color *GetColor(void) const { return &m_col; }
	void SetParent(const Matrix *pMtxParent) { m_pMtxParent = pMtxParent; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	void SetAlpha(const bool bAlpha) { m_bAlpha = bAlpha; }
	bool GetAlpha(void) const { return m_bAlpha; }
	void SetEnableScaleDown(const bool bEnable);
	void SetStateFunctionBeforeDraw(std::function<void(LPDIRECT3DDEVICE9)> before) { m_beforeDraw = before; }
	void SetStateFunctionAfterDraw(std::function<void(LPDIRECT3DDEVICE9)> after) { m_afterDraw = after; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	int m_nIdxTexture;		// テクスチャインデックス
	Matrix m_mtxWorld;		// ワールドマトリックス
	const Matrix *m_pMtxParent;		// 親ワールドマトリックスへのポインタ
	Vector3 m_pos;			// 位置
	Vector3 m_move;			// 移動量
	Vector2 m_size;			// サイズ
	Color m_col;			// 色
	float m_fScaleFrame;	// 一フレームで減少するサイズ
	int m_nLife;			// 体力
	bool m_bDisp;			// 描画するか
	bool m_bAlpha;			// αテストの有効化
	bool m_bScaleDown;		// スケール減少を行うか
	std::function<void(LPDIRECT3DDEVICE9)> m_beforeDraw;	// Draw関数前の呼び出し関数
	std::function<void(LPDIRECT3DDEVICE9)> m_afterDraw;		// Draw関数後の呼び出し関数
};
#endif