//==================================================================================
// 
// メッシュフィールドクラスのソースファイル [meshField.cpp]
// Author : TENMA SAITO
// Date   : 2026/6/15
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "meshField.h"
#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "effect.h"
#include "input.h"
#include "vec3math.h"
#include "matrix.h"

//==================================================================================
// --- メッシュフィールドの作成 ---
//==================================================================================
CMeshField *CMeshField::Create(const Vector3 &pos,
	const Vector3 &rot,
	const Vector2 &size,
	const int nNumXBlock,
	const int nNumZBlock)
{
	CMeshField *pMeshField = nullptr;		// 生成したメッシュフィールドへのポインタ

	// メッシュフィールドの生成
	pMeshField = new CMeshField;
	if (pMeshField == nullptr)
	{ // 生成失敗時、nullを返す
		return nullptr;
	}

	// メッシュフィールドの初期化
	pMeshField->Init(pos, rot, size, nNumXBlock, nNumZBlock);

	// 生成したメッシュフィールドを返す
	return pMeshField;
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMeshField::CMeshField(const int nPriority) : CMesh3D(nPriority)
{ // 各メンバ変数をクリア
	m_pos = VECTOR3_NULL;
	m_rot = VECTOR3_NULL;
	m_size = VECTOR2_NULL;
	m_nNumXBlock = 0;
	m_nNumZBlock = 0;
	m_nIdxControl = 0;
	ZeroMemory(m_aIdxFirstBlock, sizeof(m_aIdxFirstBlock));

	// タイプの指定
	CObject::SetType(TYPE_MESHFIELD);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMeshField::~CMeshField()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CMeshField::Init(const Vector3 &pos, 
	const Vector3 &rot,
	const Vector2 &size,
	const int nNumXBlock,
	const int nNumZBlock)
{
	HRESULT hr = S_OK;			// 処理判定
	int nNumVtx = 0;			// 頂点数
	int nNumIdx = 0;			// インデックス数
	int nNumPrim = 0;			// 三角ポリゴンの数
	VERTEX_3D *pVtx = nullptr;	// 頂点バッファへのポインタ
	WORD *pIdx = nullptr;		// インデックスへのポインタ

	// 引数の値を保存
	m_pos = pos;
	m_rot = rot;
	m_size = size;
	m_nNumXBlock = nNumXBlock;
	m_nNumZBlock = nNumZBlock;

	// 頂点数を計算
	nNumVtx = (nNumXBlock + 1) * (nNumZBlock + 1);

	// インデックス数を計算
	nNumIdx = 2 * ((nNumXBlock * nNumZBlock) + (2 * nNumZBlock) - 1);

	// ポリゴンの数を計算
	nNumPrim = nNumIdx - 2;

	// メッシュの初期化
	hr = CMesh3D::Init(nNumVtx, nNumIdx, nNumPrim);
	if (FAILED(hr))
	{ // メッシュの初期化
		return E_FAIL;
	}

	// 生成したメッシュの頂点バッファを取得
	if (SUCCEEDED(CMesh::LockVertex(&pVtx)))
	{ // ロック成功
		float fOnceWidth = size.x / (nNumXBlock);		// 1ブロックの幅
		float fOnceDepth = size.y / (nNumZBlock);		// 1ブロックの奥行
		int nIdxVtx;		// 設定する頂点のインデックス

		// 法線以外の頂点設定
		for (int nCntZBlock = 0; nCntZBlock < (nNumZBlock + 1); nCntZBlock++)
		{
			for (int nCntXBlock = 0; nCntXBlock < (nNumXBlock + 1); nCntXBlock++)
			{ // インデックスを計算して情報を設定
				nIdxVtx = nCntXBlock + (nCntZBlock * (nNumXBlock + 1));

				pVtx[nIdxVtx].pos.x = pos.x - (size.x * 0.5f) + (fOnceWidth * nCntXBlock);
				pVtx[nIdxVtx].pos.y = pos.y;
				pVtx[nIdxVtx].pos.z = pos.z + (size.y * 0.5f) - (fOnceDepth * nCntZBlock);
				pVtx[nIdxVtx].tex.x = (nCntXBlock * 1.0f);
				pVtx[nIdxVtx].tex.y = (nCntZBlock * 1.0f);
				pVtx[nIdxVtx].col = Constant::White;
			}
		}

		// 法線の頂点設定
		for (int nCntZBlock = 0; nCntZBlock < (nNumZBlock + 1); nCntZBlock++)
		{
			for (int nCntXBlock = 0; nCntXBlock < (nNumXBlock + 1); nCntXBlock++)
			{ // インデックスを計算して情報を設定
				nIdxVtx = nCntXBlock + (nCntZBlock * (nNumXBlock + 1));		// 設定する頂点のインデックス

				// 初期法線
				pVtx[nIdxVtx].nor = Vector3(0.0f, 1.0f, 0.0f);

				if (nCntZBlock == 0)
				{ // 一列目の場合
					if (nCntXBlock == 0)
					{ // 初めの頂点の場合、二つの法線ベクトルの平均を計算
						Vector3 norA, norB;		// 各頂点の法線
						Vector3 norAB;			// 求める法線
						Vector3 aVec[4];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx].pos - pVtx[(m_nNumZBlock + 1)].pos;
						aVec[1] = pVtx[(m_nNumZBlock + 2)].pos - pVtx[(m_nNumZBlock + 1)].pos;

						aVec[2] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
						aVec[3] = pVtx[(m_nNumZBlock + 2)].pos - pVtx[nIdxVtx + 1].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norB, &aVec[3], &aVec[2]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);

						// 2つの法線から1,2番目の頂点の法線を求める
						norAB = (norA + norB) * 0.5f;
						D3DXVec3Normalize(&norAB, &norAB);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norAB;
					}
					else if (nCntXBlock == m_nNumXBlock)
					{ // 右上の頂点の場合、一つの法線ベクトルの計算
						Vector3 nor;			// 求める法線
						Vector3 aVec[2];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&nor, &aVec[1], &aVec[0]);

						// 法線を正規化
						D3DXVec3Normalize(&nor, &nor);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = nor;
					}
					else
					{ // それ以外の場合、三つの法線ベクトルの平均を計算
						Vector3 norA, norB, norC;		// 各頂点の法線
						Vector3 norBC;			// BCの法線
						Vector3 norABC;			// 求める法線
						Vector3 aVec[6];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;

						aVec[2] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;
						aVec[3] = pVtx[nIdxVtx + (m_nNumZBlock + 2)].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;

						aVec[4] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
						aVec[5] = pVtx[nIdxVtx + (m_nNumZBlock + 2)].pos - pVtx[nIdxVtx + 1].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[1], &aVec[0]);
						D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
						D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);
						D3DXVec3Normalize(&norC, &norC);

						// 2つの法線から法線を求める
						norBC = (norB + norC) * 0.5f;
						D3DXVec3Normalize(&norBC, &norBC);

						norABC = (norA + norBC) * 0.5f;
						D3DXVec3Normalize(&norABC, &norABC);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABC;
					}
				}
				else if (nCntZBlock == m_nNumZBlock)
				{ // 最後の列の場合
					if (nCntXBlock == 0)
					{ // 左下の頂点の場合、一つの法線ベクトルの計算
						Vector3 nor;			// 求める法線
						Vector3 aVec[2];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&nor, &aVec[0], &aVec[1]);

						// 法線を正規化
						D3DXVec3Normalize(&nor, &nor);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = nor;
					}
					else if (nCntXBlock == m_nNumXBlock)
					{ // 最後の頂点の場合、二つの法線ベクトルの平均を計算
						Vector3 norA, norB;		// 各頂点の法線
						Vector3 norAB;			// 求める法線
						Vector3 aVec[4];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - 1].pos;
						aVec[1] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

						aVec[2] = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;
						aVec[3] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norB, &aVec[3], &aVec[2]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);

						// 2つの法線から1,2番目の頂点の法線を求める
						norAB = (norA + norB) * 0.5f;
						D3DXVec3Normalize(&norAB, &norAB);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norAB;
					}
					else
					{ // それ以外の場合、三つの法線ベクトルの平均を計算
						Vector3 norA, norB, norC;		// 各頂点の法線
						Vector3 norBC;			// BCの法線
						Vector3 norABC;			// 求める法線
						Vector3 aVec[6];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

						aVec[2] = pVtx[nIdxVtx - 1 - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx - 1].pos;
						aVec[3] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

						aVec[4] = pVtx[nIdxVtx - 1 - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos;
						aVec[5] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
						D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);
						D3DXVec3Normalize(&norC, &norC);

						// 2つの法線から法線を求める
						norBC = (norB + norC) * 0.5f;
						D3DXVec3Normalize(&norBC, &norBC);

						norABC = (norA + norBC) * 0.5f;
						D3DXVec3Normalize(&norABC, &norABC);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABC;
					}
				}
				else
				{ // 間の列の場合
					if (nCntXBlock == 0)
					{ // 最初の頂点の場合、三つの法線の平均を計算
						Vector3 norA, norB, norC;		// 各頂点の法線
						Vector3 norBC;			// BCの法線
						Vector3 norABC;			// 求める法線
						Vector3 aVec[6];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

						aVec[2] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;
						aVec[3] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;

						aVec[4] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
						aVec[5] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + 1].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
						D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);
						D3DXVec3Normalize(&norC, &norC);

						// 2つの法線から法線を求める
						norBC = (norB + norC) * 0.5f;
						D3DXVec3Normalize(&norBC, &norBC);

						norABC = (norA + norBC) * 0.5f;
						D3DXVec3Normalize(&norABC, &norABC);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABC;
					}
					else if (nCntXBlock == m_nNumXBlock)
					{ // 右端の頂点の場合、三つの法線の平均を計算
						Vector3 norA, norB, norC;		// 各頂点の法線
						Vector3 norBC;			// BCの法線
						Vector3 norABC;			// 求める法線
						Vector3 aVec[6];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx].pos;

						aVec[2] = pVtx[nIdxVtx - 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - 1].pos;
						aVec[3] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

						aVec[4] = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;
						aVec[5] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[1], &aVec[0]);
						D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
						D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);
						D3DXVec3Normalize(&norC, &norC);

						// 2つの法線から法線を求める
						norBC = (norB + norC) * 0.5f;
						D3DXVec3Normalize(&norBC, &norBC);

						norABC = (norA + norBC) * 0.5f;
						D3DXVec3Normalize(&norABC, &norABC);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABC;
					}
					else
					{ // それ以外の頂点の場合、六つ(1は既に求まっている)の法線の平均を計算
						Vector3 norA, norB, norC, norD, norE;		// 各頂点の法線
						Vector3 norDE;			// BCの法線
						Vector3 norABCDE;		// 求める法線
						Vector3 aVec[8];		// 各頂点の境界線ベクトル

						// 左上の法線の取得
						norA = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].nor;

						// 各境界線ベクトルを求める
						// 右上の三角ポリゴン用のベクトル
						aVec[0] = pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

						// 左下の三角ポリゴン用のベクトル
						aVec[2] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
						aVec[3] = pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx].pos;

						// 右下の三角ポリゴン用のベクトル
						aVec[4] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;
						aVec[5] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;

						aVec[6] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
						aVec[7] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + 1].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norB, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norC, &aVec[3], &aVec[2]);
						D3DXVec3Cross(&norD, &aVec[4], &aVec[5]);
						D3DXVec3Cross(&norE, &aVec[7], &aVec[6]);

						// 2つの法線から法線を求める
						norDE = (norD + norE) * 0.5f;
						D3DXVec3Normalize(&norDE, &norDE);

						// 求まった全ての法線の平均を計算
						norABCDE = (norA + norB + norC + norDE) * 0.25f;
						D3DXVec3Normalize(&norABCDE, &norABCDE);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABCDE;
					}
				}
			}
		}

		// ロック解除
		CMesh::UnlockVertex();
	}

	// 生成したメッシュのインデックスを設定
	if (SUCCEEDED(CMesh::LockIndex(&pIdx)))
	{ // ロック成功
		int nIdx1 = 0;					// 一つ目の変数に加算する値
		int nIdx2 = 0;					// 二つ目の変数に加算する値
		int nOffset = nNumXBlock + 1;	// 変数に足す際にずらすオフセット
		int nCntUnswap = 0;			// スワップせずにインデックスを設定した回数
		bool bSwapping = false;		// スワップフラグ

		for (int nCntIdx = 0; nCntIdx < nNumIdx; nCntIdx++)
		{ // インデックス設定
			if ((nCntUnswap % nOffset != 0 || nCntUnswap == 0)
				&& bSwapping == false)
			{ // 通常のインデックス設定
				pIdx[nCntIdx] = nOffset + nIdx1;
				pIdx[nCntIdx + 1] = nIdx2;

				nIdx1++;
				nIdx2++;
				nCntUnswap++;
			}
			else if (nCntUnswap % nOffset == 0
				&& bSwapping == true)
			{ // 折り返しパート2
				pIdx[nCntIdx] = nOffset + nIdx1;
				pIdx[nCntIdx + 1] = (nIdx2 - 1);

				nIdx1++;
				bSwapping = false;
				nCntUnswap = 1;
			}
			else
			{ // 折り返しパート1
				pIdx[nCntIdx] = nIdx2 - 1;
				pIdx[nCntIdx + 1] = nOffset + nIdx1;

				nIdx2++;
				bSwapping = true;
			}

			nCntIdx++;
		}

		for (int nCntIdx = 0; nCntIdx < 4; nCntIdx++)
		{ // 最初の四角形のインデックスを保存
			m_aIdxFirstBlock[nCntIdx] = pIdx[nCntIdx];
		}

		// ロック解除
		CMesh::UnlockIndex();
	}

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CMeshField::Uninit(void)
{ // メッシュの終了
	CMesh3D::Uninit();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CMeshField::Update(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャーへのポインタ
	auto pKeyboard = pManager->GetInputKeyboard();		// キーボードへのポインタ
	auto pProc = pManager->GetDebugProc();				// デバッグ表示へのポインタ
	VERTEX_3D *pVtx = nullptr;				// 頂点バッファへのポインタ
	Vector3 posVtx;						// 頂点座標

	if (pKeyboard->GetPress(DIK_LCONTROL))
	{ // 左CTRLを押しながらで頂点編集モード
		if (pKeyboard->GetTrigger(DIK_D))
		{ // インデックスを進める
			m_nIdxControl = (m_nIdxControl + 1) % CMesh::GetVertexNum();
		}
		else if (pKeyboard->GetPress(DIK_A))
		{ // インデックスを戻す
			m_nIdxControl = (m_nIdxControl - 1 < 0) ? CMesh::GetVertexNum() - 1 : m_nIdxControl - 1;
		}

		// 頂点バッファをロック
		if (SUCCEEDED(CMesh::LockVertex(&pVtx)))
		{ // ロック成功時
			if (pKeyboard->GetPress(DIK_W))
			{ // 頂点のY座標を上げる
				pVtx[m_nIdxControl].pos.y += 1.0f;
			}
			else if (pKeyboard->GetPress(DIK_S))
			{ // 頂点のY座標を下げる
				pVtx[m_nIdxControl].pos.y -= 1.0f;
			}

			// 座標を保存
			posVtx = pVtx[m_nIdxControl].pos;

			// 法線の頂点設定
			for (int nCntZBlock = 0; nCntZBlock < (m_nNumZBlock + 1); nCntZBlock++)
			{
				for (int nCntXBlock = 0; nCntXBlock < (m_nNumXBlock + 1); nCntXBlock++)
				{ // インデックスを計算して情報を設定
					int nIdxVtx = nCntXBlock + (nCntZBlock * (m_nNumXBlock + 1));		// 設定する頂点のインデックス

					if (nCntZBlock == 0)
					{ // 一列目の場合
						if (nCntXBlock == 0)
						{ // 初めの頂点の場合、二つの法線ベクトルの平均を計算
							Vector3 norA, norB;		// 各頂点の法線
							Vector3 norAB;			// 求める法線
							Vector3 aVec[4];		// 各頂点の境界線ベクトル

							// 各境界線ベクトルを求める
							aVec[0] = pVtx[nIdxVtx].pos - pVtx[(m_nNumZBlock + 1)].pos;
							aVec[1] = pVtx[(m_nNumZBlock + 2)].pos - pVtx[(m_nNumZBlock + 1)].pos;

							aVec[2] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
							aVec[3] = pVtx[(m_nNumZBlock + 2)].pos - pVtx[nIdxVtx + 1].pos;

							// 法線を各ベクトルから求める
							D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
							D3DXVec3Cross(&norB, &aVec[3], &aVec[2]);

							// 法線を正規化
							D3DXVec3Normalize(&norA, &norA);
							D3DXVec3Normalize(&norB, &norB);

							// 2つの法線から1,2番目の頂点の法線を求める
							norAB = (norA + norB) * 0.5f;
							D3DXVec3Normalize(&norAB, &norAB);

							// 法線ベクトルの設定
							pVtx[nIdxVtx].nor = norAB;
						}
						else if (nCntXBlock == m_nNumXBlock)
						{ // 右上の頂点の場合、一つの法線ベクトルの計算
							Vector3 nor;			// 求める法線
							Vector3 aVec[2];		// 各頂点の境界線ベクトル

							// 各境界線ベクトルを求める
							aVec[0] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
							aVec[1] = pVtx[nIdxVtx + (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;

							// 法線を各ベクトルから求める
							D3DXVec3Cross(&nor, &aVec[1], &aVec[0]);

							// 法線を正規化
							D3DXVec3Normalize(&nor, &nor);

							// 法線ベクトルの設定
							pVtx[nIdxVtx].nor = nor;
						}
						else
						{ // それ以外の場合、三つの法線ベクトルの平均を計算
							Vector3 norA, norB, norC;		// 各頂点の法線
							Vector3 norBC;			// BCの法線
							Vector3 norABC;			// 求める法線
							Vector3 aVec[6];		// 各頂点の境界線ベクトル

							// 各境界線ベクトルを求める
							aVec[0] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
							aVec[1] = pVtx[nIdxVtx + (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;

							aVec[2] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;
							aVec[3] = pVtx[nIdxVtx + (m_nNumZBlock + 2)].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;

							aVec[4] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
							aVec[5] = pVtx[nIdxVtx + (m_nNumZBlock + 2)].pos - pVtx[nIdxVtx + 1].pos;

							// 法線を各ベクトルから求める
							D3DXVec3Cross(&norA, &aVec[1], &aVec[0]);
							D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
							D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

							// 法線を正規化
							D3DXVec3Normalize(&norA, &norA);
							D3DXVec3Normalize(&norB, &norB);
							D3DXVec3Normalize(&norC, &norC);

							// 2つの法線から法線を求める
							norBC = (norB + norC) * 0.5f;
							D3DXVec3Normalize(&norBC, &norBC);

							norABC = (norA + norBC) * 0.5f;
							D3DXVec3Normalize(&norABC, &norABC);

							// 法線ベクトルの設定
							pVtx[nIdxVtx].nor = norABC;
						}
					}
					else if (nCntZBlock == m_nNumZBlock)
					{ // 最後の列の場合
						if (nCntXBlock == 0)
						{ // 左下の頂点の場合、一つの法線ベクトルの計算
							Vector3 nor;			// 求める法線
							Vector3 aVec[2];		// 各頂点の境界線ベクトル

							// 各境界線ベクトルを求める
							aVec[0] = pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;
							aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

							// 法線を各ベクトルから求める
							D3DXVec3Cross(&nor, &aVec[0], &aVec[1]);

							// 法線を正規化
							D3DXVec3Normalize(&nor, &nor);

							// 法線ベクトルの設定
							pVtx[nIdxVtx].nor = nor;
						}
						else if (nCntXBlock == m_nNumXBlock)
						{ // 最後の頂点の場合、二つの法線ベクトルの平均を計算
							Vector3 norA, norB;		// 各頂点の法線
							Vector3 norAB;			// 求める法線
							Vector3 aVec[4];		// 各頂点の境界線ベクトル

							// 各境界線ベクトルを求める
							aVec[0] = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - 1].pos;
							aVec[1] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

							aVec[2] = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;
							aVec[3] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;

							// 法線を各ベクトルから求める
							D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
							D3DXVec3Cross(&norB, &aVec[3], &aVec[2]);

							// 法線を正規化
							D3DXVec3Normalize(&norA, &norA);
							D3DXVec3Normalize(&norB, &norB);

							// 2つの法線から1,2番目の頂点の法線を求める
							norAB = (norA + norB) * 0.5f;
							D3DXVec3Normalize(&norAB, &norAB);

							// 法線ベクトルの設定
							pVtx[nIdxVtx].nor = norAB;
						}
						else
						{ // それ以外の場合、三つの法線ベクトルの平均を計算
							Vector3 norA, norB, norC;		// 各頂点の法線
							Vector3 norBC;			// BCの法線
							Vector3 norABC;			// 求める法線
							Vector3 aVec[6];		// 各頂点の境界線ベクトル

							// 各境界線ベクトルを求める
							aVec[0] = pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;
							aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

							aVec[2] = pVtx[nIdxVtx - 1 - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx - 1].pos;
							aVec[3] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

							aVec[4] = pVtx[nIdxVtx - 1 - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos;
							aVec[5] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos;

							// 法線を各ベクトルから求める
							D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
							D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
							D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

							// 法線を正規化
							D3DXVec3Normalize(&norA, &norA);
							D3DXVec3Normalize(&norB, &norB);
							D3DXVec3Normalize(&norC, &norC);

							// 2つの法線から法線を求める
							norBC = (norB + norC) * 0.5f;
							D3DXVec3Normalize(&norBC, &norBC);

							norABC = (norA + norBC) * 0.5f;
							D3DXVec3Normalize(&norABC, &norABC);

							// 法線ベクトルの設定
							pVtx[nIdxVtx].nor = norABC;
						}
					}					
					else
					{ // 間の列の場合
						if (nCntXBlock == 0)
						{ // 最初の頂点の場合、三つの法線の平均を計算
							Vector3 norA, norB, norC;		// 各頂点の法線
							Vector3 norBC;			// BCの法線
							Vector3 norABC;			// 求める法線
							Vector3 aVec[6];		// 各頂点の境界線ベクトル

							// 各境界線ベクトルを求める
							aVec[0] = pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;
							aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

							aVec[2] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;
							aVec[3] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;

							aVec[4] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
							aVec[5] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + 1].pos;

							// 法線を各ベクトルから求める
							D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
							D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
							D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

							// 法線を正規化
							D3DXVec3Normalize(&norA, &norA);
							D3DXVec3Normalize(&norB, &norB);
							D3DXVec3Normalize(&norC, &norC);

							// 2つの法線から法線を求める
							norBC = (norB + norC) * 0.5f;
							D3DXVec3Normalize(&norBC, &norBC);

							norABC = (norA + norBC) * 0.5f;
							D3DXVec3Normalize(&norABC, &norABC);

							// 法線ベクトルの設定
							pVtx[nIdxVtx].nor = norABC;
						}
						else if (nCntXBlock == m_nNumXBlock)
						{ // 右端の頂点の場合、三つの法線の平均を計算
							Vector3 norA, norB, norC;		// 各頂点の法線
							Vector3 norBC;			// BCの法線
							Vector3 norABC;			// 求める法線
							Vector3 aVec[6];		// 各頂点の境界線ベクトル

							// 各境界線ベクトルを求める
							aVec[0] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
							aVec[1] = pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx].pos;

							aVec[2] = pVtx[nIdxVtx - 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - 1].pos;
							aVec[3] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

							aVec[4] = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;
							aVec[5] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;

							// 法線を各ベクトルから求める
							D3DXVec3Cross(&norA, &aVec[1], &aVec[0]);
							D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
							D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

							// 法線を正規化
							D3DXVec3Normalize(&norA, &norA);
							D3DXVec3Normalize(&norB, &norB);
							D3DXVec3Normalize(&norC, &norC);

							// 2つの法線から法線を求める
							norBC = (norB + norC) * 0.5f;
							D3DXVec3Normalize(&norBC, &norBC);

							norABC = (norA + norBC) * 0.5f;
							D3DXVec3Normalize(&norABC, &norABC);

							// 法線ベクトルの設定
							pVtx[nIdxVtx].nor = norABC;
						}
						else
						{ // それ以外の頂点の場合、六つ(1は既に求まっている)の法線の平均を計算
							Vector3 norA, norB, norC, norD, norE;		// 各頂点の法線
							Vector3 norDE;			// BCの法線
							Vector3 norABCDE;		// 求める法線
							Vector3 aVec[8];		// 各頂点の境界線ベクトル

							// 左上の法線の取得
							norA = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].nor;

							// 各境界線ベクトルを求める
							// 右上の三角ポリゴン用のベクトル
							aVec[0] = pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx].pos;
							aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

							// 左下の三角ポリゴン用のベクトル
							aVec[2] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
							aVec[3] = pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx].pos;

							// 右下の三角ポリゴン用のベクトル
							aVec[4] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;
							aVec[5] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;

							aVec[6] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
							aVec[7] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + 1].pos;

							// 法線を各ベクトルから求める
							D3DXVec3Cross(&norB, &aVec[0], &aVec[1]);
							D3DXVec3Cross(&norC, &aVec[3], &aVec[2]);
							D3DXVec3Cross(&norD, &aVec[4], &aVec[5]);
							D3DXVec3Cross(&norE, &aVec[7], &aVec[6]);

							// 2つの法線から法線を求める
							norDE = (norD + norE) * 0.5f;
							D3DXVec3Normalize(&norDE, &norDE);

							// 求まった全ての法線の平均を計算
							norABCDE = (norA + norB + norC + norDE) * 0.25f;
							D3DXVec3Normalize(&norABCDE, &norABCDE);

							// 法線ベクトルの設定
							pVtx[nIdxVtx].nor = norABCDE;
						}
					}
				}
			}

			// 頂点バッファをアンロック
			CMesh::UnlockVertex();
		}

		// エフェクトを生成
		CEffect::Create(posVtx, 10, 10.0f);

		// 情報表示
		pProc->Print("[操作中インデックス : %d (A/Dで変更)]\n", m_nIdxControl);
		pProc->Print("[頂点の座標 : %f %f %f] (W/SでY座標を変更)\n", posVtx.x, posVtx.y, posVtx.z);
	}
	else
	{ // ガイド表示
		pProc->Print("[CTRLを押すと頂点編集モード！]\n");
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CMeshField::Draw(void)
{
	CManager *pManager = CManager::GetInstance();		// マネージャへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pManager->GetRenderer()->GetDevice();		// デバイスへのポインタ
	VERTEX_3D *pVtx = nullptr;				// 頂点バッファへのポインタ
	int nNumVtx = CMesh::GetVertexNum();	// 頂点数

	// マトリックスを初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// マトリックスを計算
	Mtx::CalcWorld(&m_mtxWorld,
		m_pos,
		m_rot);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ワイヤーフレームに設定
	//pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// メッシュの描画
	CMesh3D::Draw();

	// 通常描画に設定
	//pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

//==================================================================================
// --- メッシュフィールドの高さ取得処理 ---
//==================================================================================
float CMeshField::GetHeight(const Vector3 &pos)
{
	CManager *pManager = CManager::GetInstance();	// マネージャーへのポインタ
	auto pProc = pManager->GetDebugProc();			// デバッグ表示へのポインタ
	Vector3 aVtx[4] = { VECTOR3_NULL };		// 当たり判定をする頂点座標
	Vector3 posStart = VECTOR3_NULL;		// 割合計算の開始地点
	float fHeight = 0.0f;			// 計算結果

	// スタート地点を指定 (左上)
	posStart.x = m_pos.x - (m_size.x * 0.5f);
	posStart.z = m_pos.z + (m_size.y * 0.5f);

	Vector3 posDiff = VECTOR3_NULL;			// スタート地点からの差分
	Vector3 norDiff = VECTOR3_NULL;			// 差分の正規化結果

	// スタート位置からどれだけ離れているかを求める
	posDiff.x = pos.x - posStart.x;
	posDiff.z = posStart.z - pos.z;

	// 0～1の間に変換
	norDiff.x = posDiff.x / m_size.x;
	norDiff.z = posDiff.z / m_size.y;

	if (norDiff.x < 0.0f || norDiff.x > 1.0f
		|| norDiff.z < 0.0f || norDiff.z > 1.0f)
	{ // メッシュフィールド外にいるならスキップ
		return fHeight;
	}

	float fXBlock = 1.0f / m_nNumXBlock;	// Xの一ブロック当たりの割合
	float fZBlock = 1.0f / m_nNumZBlock;	// Zの一ブロック当たりの割合
	int nIdxX = 0;		// Xのブロックインデックス
	int nIdxZ = 0;		// Zのブロックインデックス

	// どの四角形の中にいるのか判定
	nIdxX = static_cast<int>(norDiff.x / fXBlock);
	nIdxZ = static_cast<int>(norDiff.z / fZBlock);

	int nIdxBlock = (m_nNumZBlock * nIdxZ) + nIdxX;		// 現在いるブロックの番号
	int aIdx[4];		// 判定をするインデックス

	// 判定をするインデックスを求める
	aIdx[0] = m_aIdxFirstBlock[0] + (nIdxBlock + nIdxZ);
	aIdx[1] = m_aIdxFirstBlock[1] + (nIdxBlock + nIdxZ);
	aIdx[2] = m_aIdxFirstBlock[2] + (nIdxBlock + nIdxZ);
	aIdx[3] = m_aIdxFirstBlock[3] + (nIdxBlock + nIdxZ);

	VERTEX_3D *pVtx = nullptr;				// 頂点バッファへのポインタ

	// 頂点バッファをロック
	if (SUCCEEDED(CMesh::LockVertex(&pVtx)))
	{ // ロック成功時
		for (int nCntVtx = 0; nCntVtx < 4; nCntVtx++)
		{ // メッシュフィールドのインデックスの頂点座標を求める
			aVtx[nCntVtx] = pVtx[aIdx[nCntVtx]].pos;
		}

		// 頂点バッファをアンロック
		CMesh::UnlockVertex();
	}

	if (Vec3::IsInsideTriangle(pos, &aVtx[0]))
	{ // もし三角形の内側にいたら
		Vector3 vec1, vec2;		// 各頂点間のベクトル
		Vector3 nor;			// 法線ベクトル

		// 各ベクトルを求める
		vec1 = aVtx[1] - aVtx[0];
		vec2 = aVtx[2] - aVtx[0];

		// 法線を計算
		D3DXVec3Cross(&nor, &vec1, &vec2);
		D3DXVec3Normalize(&nor, &nor);

		// 高さを求める
		fHeight = Vec3::Height(pos, aVtx[0], nor);
	}
	else if (Vec3::IsInsideTriangle(pos, &aVtx[1], true))
	{ // もし三角形の内側にいたら
		Vector3 vec1, vec2;		// 各頂点間のベクトル
		Vector3 nor;			// 法線ベクトル

		// 各ベクトルを求める
		vec1 = aVtx[1] - aVtx[3];
		vec2 = aVtx[2] - aVtx[3];

		// 法線を計算
		D3DXVec3Cross(&nor, &vec2, &vec1);
		D3DXVec3Normalize(&nor, &nor);

		// 高さを求める
		fHeight = Vec3::Height(pos, aVtx[3], nor);
	}

	return fHeight;
}

//==================================================================================
// --- メッシュフィールドの高さ増減処理 ---
//==================================================================================
void CMeshField::SetHeight(const Vector3 &pos, const float fWidth, const float fPower)
{
	VERTEX_3D *pVtx = nullptr;				// 頂点バッファへのポインタ

	// 頂点バッファをロック
	if (SUCCEEDED(CMesh::LockVertex(&pVtx)))
	{ // ロック成功時
		for (int nCntVtx = 0; nCntVtx < CMesh::GetVertexNum(); nCntVtx++)
		{ // 頂点数分引数の座標と見比べる
			float fLength = Vec3::Length(pos, pVtx[nCntVtx].pos);		// 頂点との距離
			if (fLength <= fWidth)
			{ // 半径以内だったら
				float fRatio = 1.0f - (fLength / fWidth);		// どれだけ離れているか

				// 離れた距離で割った分高さを加算する
				pVtx[nCntVtx].pos.y += fPower * fRatio;
			}
		}

		// 法線の頂点設定
		for (int nCntZBlock = 0; nCntZBlock < (m_nNumZBlock + 1); nCntZBlock++)
		{
			for (int nCntXBlock = 0; nCntXBlock < (m_nNumXBlock + 1); nCntXBlock++)
			{ // インデックスを計算して情報を設定
				int nIdxVtx = nCntXBlock + (nCntZBlock * (m_nNumXBlock + 1));		// 設定する頂点のインデックス

				if (nCntZBlock == 0)
				{ // 一列目の場合
					if (nCntXBlock == 0)
					{ // 初めの頂点の場合、二つの法線ベクトルの平均を計算
						Vector3 norA, norB;		// 各頂点の法線
						Vector3 norAB;			// 求める法線
						Vector3 aVec[4];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx].pos - pVtx[(m_nNumZBlock + 1)].pos;
						aVec[1] = pVtx[(m_nNumZBlock + 2)].pos - pVtx[(m_nNumZBlock + 1)].pos;

						aVec[2] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
						aVec[3] = pVtx[(m_nNumZBlock + 2)].pos - pVtx[nIdxVtx + 1].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norB, &aVec[3], &aVec[2]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);

						// 2つの法線から1,2番目の頂点の法線を求める
						norAB = (norA + norB) * 0.5f;
						D3DXVec3Normalize(&norAB, &norAB);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norAB;
					}
					else if (nCntXBlock == m_nNumXBlock)
					{ // 右上の頂点の場合、一つの法線ベクトルの計算
						Vector3 nor;			// 求める法線
						Vector3 aVec[2];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&nor, &aVec[1], &aVec[0]);

						// 法線を正規化
						D3DXVec3Normalize(&nor, &nor);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = nor;
					}
					else
					{ // それ以外の場合、三つの法線ベクトルの平均を計算
						Vector3 norA, norB, norC;		// 各頂点の法線
						Vector3 norBC;			// BCの法線
						Vector3 norABC;			// 求める法線
						Vector3 aVec[6];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;

						aVec[2] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;
						aVec[3] = pVtx[nIdxVtx + (m_nNumZBlock + 2)].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;

						aVec[4] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
						aVec[5] = pVtx[nIdxVtx + (m_nNumZBlock + 2)].pos - pVtx[nIdxVtx + 1].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[1], &aVec[0]);
						D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
						D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);
						D3DXVec3Normalize(&norC, &norC);

						// 2つの法線から法線を求める
						norBC = (norB + norC) * 0.5f;
						D3DXVec3Normalize(&norBC, &norBC);

						norABC = (norA + norBC) * 0.5f;
						D3DXVec3Normalize(&norABC, &norABC);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABC;
					}
				}
				else if (nCntZBlock == m_nNumZBlock)
				{ // 最後の列の場合
					if (nCntXBlock == 0)
					{ // 左下の頂点の場合、一つの法線ベクトルの計算
						Vector3 nor;			// 求める法線
						Vector3 aVec[2];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&nor, &aVec[0], &aVec[1]);

						// 法線を正規化
						D3DXVec3Normalize(&nor, &nor);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = nor;
					}
					else if (nCntXBlock == m_nNumXBlock)
					{ // 最後の頂点の場合、二つの法線ベクトルの平均を計算
						Vector3 norA, norB;		// 各頂点の法線
						Vector3 norAB;			// 求める法線
						Vector3 aVec[4];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - 1].pos;
						aVec[1] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

						aVec[2] = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;
						aVec[3] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norB, &aVec[3], &aVec[2]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);

						// 2つの法線から1,2番目の頂点の法線を求める
						norAB = (norA + norB) * 0.5f;
						D3DXVec3Normalize(&norAB, &norAB);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norAB;
					}
					else
					{ // それ以外の場合、三つの法線ベクトルの平均を計算
						Vector3 norA, norB, norC;		// 各頂点の法線
						Vector3 norBC;			// BCの法線
						Vector3 norABC;			// 求める法線
						Vector3 aVec[6];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

						aVec[2] = pVtx[nIdxVtx - 1 - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx - 1].pos;
						aVec[3] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

						aVec[4] = pVtx[nIdxVtx - 1 - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos;
						aVec[5] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
						D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);
						D3DXVec3Normalize(&norC, &norC);

						// 2つの法線から法線を求める
						norBC = (norB + norC) * 0.5f;
						D3DXVec3Normalize(&norBC, &norBC);

						norABC = (norA + norBC) * 0.5f;
						D3DXVec3Normalize(&norABC, &norABC);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABC;
					}
				}
				else
				{ // 間の列の場合
					if (nCntXBlock == 0)
					{ // 最初の頂点の場合、三つの法線の平均を計算
						Vector3 norA, norB, norC;		// 各頂点の法線
						Vector3 norBC;			// BCの法線
						Vector3 norABC;			// 求める法線
						Vector3 aVec[6];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - (m_nNumXBlock + 1)].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

						aVec[2] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;
						aVec[3] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;

						aVec[4] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
						aVec[5] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + 1].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
						D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);
						D3DXVec3Normalize(&norC, &norC);

						// 2つの法線から法線を求める
						norBC = (norB + norC) * 0.5f;
						D3DXVec3Normalize(&norBC, &norBC);

						norABC = (norA + norBC) * 0.5f;
						D3DXVec3Normalize(&norABC, &norABC);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABC;
					}
					else if (nCntXBlock == m_nNumXBlock)
					{ // 右端の頂点の場合、三つの法線の平均を計算
						Vector3 norA, norB, norC;		// 各頂点の法線
						Vector3 norBC;			// BCの法線
						Vector3 norABC;			// 求める法線
						Vector3 aVec[6];		// 各頂点の境界線ベクトル

						// 各境界線ベクトルを求める
						aVec[0] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx].pos;

						aVec[2] = pVtx[nIdxVtx - 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - 1].pos;
						aVec[3] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

						aVec[4] = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;
						aVec[5] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norA, &aVec[1], &aVec[0]);
						D3DXVec3Cross(&norB, &aVec[2], &aVec[3]);
						D3DXVec3Cross(&norC, &aVec[5], &aVec[4]);

						// 法線を正規化
						D3DXVec3Normalize(&norA, &norA);
						D3DXVec3Normalize(&norB, &norB);
						D3DXVec3Normalize(&norC, &norC);

						// 2つの法線から法線を求める
						norBC = (norB + norC) * 0.5f;
						D3DXVec3Normalize(&norBC, &norBC);

						norABC = (norA + norBC) * 0.5f;
						D3DXVec3Normalize(&norABC, &norABC);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABC;
					}
					else
					{ // それ以外の頂点の場合、六つ(1は既に求まっている)の法線の平均を計算
						Vector3 norA, norB, norC, norD, norE;		// 各頂点の法線
						Vector3 norDE;			// BCの法線
						Vector3 norABCDE;		// 求める法線
						Vector3 aVec[8];		// 各頂点の境界線ベクトル

						// 左上の法線の取得
						norA = pVtx[nIdxVtx - 1 - (m_nNumZBlock + 1)].nor;

						// 各境界線ベクトルを求める
						// 右上の三角ポリゴン用のベクトル
						aVec[0] = pVtx[nIdxVtx - (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx].pos;
						aVec[1] = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

						// 左下の三角ポリゴン用のベクトル
						aVec[2] = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
						aVec[3] = pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx].pos;

						// 右下の三角ポリゴン用のベクトル
						aVec[4] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;
						aVec[5] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + (m_nNumZBlock + 1)].pos;

						aVec[6] = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
						aVec[7] = pVtx[nIdxVtx + 1 + (m_nNumZBlock + 1)].pos - pVtx[nIdxVtx + 1].pos;

						// 法線を各ベクトルから求める
						D3DXVec3Cross(&norB, &aVec[0], &aVec[1]);
						D3DXVec3Cross(&norC, &aVec[3], &aVec[2]);
						D3DXVec3Cross(&norD, &aVec[4], &aVec[5]);
						D3DXVec3Cross(&norE, &aVec[7], &aVec[6]);

						// 2つの法線から法線を求める
						norDE = (norD + norE) * 0.5f;
						D3DXVec3Normalize(&norDE, &norDE);

						// 求まった全ての法線の平均を計算
						norABCDE = (norA + norB + norC + norDE) * 0.25f;
						D3DXVec3Normalize(&norABCDE, &norABCDE);

						// 法線ベクトルの設定
						pVtx[nIdxVtx].nor = norABCDE;
					}
				}
			}
		}

		// 頂点バッファをアンロック
		CMesh::UnlockVertex();
	}
}