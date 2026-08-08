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

	static CNumber *Create(const Vector3 &pos, const Vector2 &size, const int nValue);

	HRESULT Init(void);
	HRESULT Init(const Vector3 &pos, const Vector2 &size, const int nValue);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const Vector3 &position);
	Vector3 GetPosition(void) const { return m_pos; }
	void SetRotation(const Vector3 &rotation);
	Vector3 GetRotation(void) const { return m_rot; }
	void SetPositionAndRotation(const Vector3 &position, const Vector3 &rotation);
	void SetSize(const Vector2 &size);
	Vector2 GetSize(void) const { return m_size; }
	float GetLength(void) const { return m_fLength; }
	void SetNumber(const int nNumber);
	int GetNumber(void) const { return m_nNumber; }
	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファ
	Vector3 m_pos;		// オブジェクトの位置
	Vector3 m_rot;		// オブジェクトの角度
	Vector2 m_size;		// オブジェクトのサイズ
	float m_fLength;		// 対角線の長さ
	float m_fAngle;			// 対角線の角度
	int m_nNumber;			// 値
	int m_nIdxTexture;		// texindex
};
#endif