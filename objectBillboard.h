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

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define DEFAULT_OBJBILL_PRIORITY	(6)								// objBillの基本優先順位
#define DEFAULT_OBJBILL_POS			VECTOR3_NULL					// obj3dの基本位置
#define DEFAULT_OBJBILL_SIZE		Vector2(100.0f, 100.0f)		// obj3Dの基本サイズ

//**********************************************************************************
// *** オブジェクトビルボードクラス ***
//**********************************************************************************
class CObjectBillboard : public CObject
{
public:
	static CObjectBillboard *Create(const Vector3 &pos = DEFAULT_OBJBILL_POS,
		const Vector2 &size = DEFAULT_OBJBILL_SIZE);

	CObjectBillboard(const int nPriority = DEFAULT_OBJBILL_PRIORITY);
	~CObjectBillboard();

	HRESULT Init(const Vector3 &pos, const Vector2 &size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }
	void SetPosition(const Vector3 &position) { m_pos = position; }
	Vector3 GetPosition(void) const { return m_pos; }
	void SetSize(const Vector2 &size);
	Vector2 GetSize(void) const { return m_size; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) const { return m_bDisp; }
	void SetAlpha(const bool bAlpha) { m_bAlpha = bAlpha; }
	bool GetAlpha(void) const { return m_bAlpha; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	int m_nIdxTexture;		// テクスチャインデックス
	Matrix m_mtxWorld;		// ワールドマトリックス
	Vector3 m_pos;			// 位置
	Vector2 m_size;			// サイズ
	bool m_bDisp;			// 描画するか
	bool m_bAlpha;			// αテストの有効化
};
#endif