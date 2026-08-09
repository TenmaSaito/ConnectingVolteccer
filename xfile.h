//==================================================================================
// 
// Xファイルクラスのヘッダーファイル [xfile.h]
// Author : TENMA SAITO
// Date   : 2026/6/22
// 
//==================================================================================
#ifndef _XFILE_H_		// インクルードガード
#define _XFILE_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "main.h"
#include <string>
#include <vector>

//**********************************************************************************
// *** Xファイルクラス ***
//**********************************************************************************
class CXFile
{
public:
	// XFileの情報構造体
	struct XDATA
	{
		std::string sXFileName;		// 読み込んだXファイルパス
		LPD3DXMESH pMesh;			// メッシュ(頂点情報)へのポインタ
		LPD3DXBUFFER pBuffMat;		// マテリアルへのポインタ
		std::vector<int> vIdx;		// テクスチャインデックスの配列
		DWORD dwNumMat;				// マテリアルの数
		Vector3 vtxMin, vtxMax;		// モデルの各最大最小頂点の位置
		int nId;					// 自身のID
	};

	CXFile();
	~CXFile();

	static CXFile *GetInstance(void);

	int Resister(const char *pXFileName, const bool bCopy = false);
	void Unload(void);
	bool GetAddress(const int nIdxXFile, XDATA **ppOut);

private:
	std::vector<XDATA> m_vXData;	// 読み込んだデータ
	int m_nNumAll;			// 読み込んだモデルの総数
};
#endif