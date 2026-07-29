//==================================================================================
// 
// 数字表示クラスのヘッダーファイル [number.h]
// Author : TENMA SAITO
// Date   : 2026/5/11
// 
//==================================================================================
#ifndef _NUMBER_H_		// インクルードガード
#define _NUMBER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
// *** 数値表示クラス ***
//**********************************************************************************
class CNumber
{
public:
	CNumber();
	~CNumber();

	static CNumber *Create(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nValue);

	HRESULT Init(void);
	HRESULT Init(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nValue);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 &position);
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	void SetRotation(const D3DXVECTOR3 &rotation);
	D3DXVECTOR3 GetRotation(void) const { return m_rot; }
	void SetPositionAndRotation(const D3DXVECTOR3 &position, const D3DXVECTOR3 &rotation);
	void SetSize(const D3DXVECTOR2 &size);
	D3DXVECTOR2 GetSize(void) const { return m_size; }
	float GetLength(void) const { return m_fLength; }
	void SetNumber(const int nNumber);
	int GetNumber(void) const { return m_nNumber; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファ
	D3DXVECTOR3 m_pos;		// オブジェクトの位置
	D3DXVECTOR3 m_rot;		// オブジェクトの角度
	D3DXVECTOR2 m_size;		// オブジェクトのサイズ
	float m_fLength;		// 対角線の長さ
	float m_fAngle;			// 対角線の角度
	int m_nNumber;			// 値
	int m_nIdxTexture;		// texindex
};
#endif