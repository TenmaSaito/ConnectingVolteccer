//==================================================================================
// 
// オブジェクトクラスのソースファイル [object.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/8
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "object.h"
#include "manager.h"
#include "camera.h"

//**********************************************************************************
// *** 静的メンバ変数宣言 ***
//**********************************************************************************
#if ENABLE_OBJECT_LIST
CObject *CObject::m_apTop[MAX_OBJPRIORITY] = { nullptr };		// 先頭オブジェクトへのポインタ
CObject *CObject::m_apCur[MAX_OBJPRIORITY] = { nullptr };		// 最後尾オブジェクトへのポインタ
#else
CObject *CObject::m_apObject[MAX_OBJPRIORITY][MAX_OBJECT];		// オブジェクトへのポインタ
#endif
int CObject::m_nNumAll = 0;						// オブジェクトの総数

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CObject::CObject(const int nPriority)
{
#if ENABLE_OBJECT_LIST
	// メンバ変数のクリア
	m_pPrev = nullptr;
	m_pNext = nullptr;
	m_nPriority = nPriority;
	m_bDeath = false;
	
	// 自身をリストに追加
	AddList();

	// 総数増加
	m_nNumAll++;
#else
	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{ // 配列の総数分繰り返す
		if (m_apObject[nPriority][nCntObject] == NULL)
		{ // NULLだった場合、配列に代入
			// ポインタ配列に自身のポインタを追加
			m_apObject[nPriority][nCntObject] = this;

			// 優先順位の保存
			m_nPriority = nPriority;

			// IDを設定
			m_nID = nCntObject;

			// オブジェクトの総数を増やす
			m_nNumAll++;

			// ループ終了
			break;
		}
	}
#endif

	// タイプを設定
	SetType(TYPE_NONE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CObject::~CObject()
{
}

//==================================================================================
// --- 自身の観察者に対しての通知処理 ---
//==================================================================================
void CObject::NotifyAll(std::string_view message)
{
	for (auto &pObserver : m_vpObserver)
	{ // nullの場合スキップ
		if (pObserver == nullptr) continue;

		// 自身が死んだことを通知
		pObserver->Notified(this, message);
	}
}

//==================================================================================
// --- 全オブジェクトの解放処理 ---
//==================================================================================
void CObject::ReleaseAll(void)
{
#if ENABLE_OBJECT_LIST
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位分回す
		CObject *pObject = m_apTop[nCntPriority];		// 先頭オブジェクトへのポインタ
		while (pObject != nullptr)
		{ // nullptrになるまで繰り返す
			CObject *pObjectNext = pObject->m_pNext;	// 次のオブジェクトへのポインタ

			// オブジェクトの終了
			pObject->Uninit();

			pObject = pObjectNext;		// ポインタを次のオブジェクトへのポインタに変更
		}
	}

	// 死亡フラグを確認
	FlagCheckAll();
#else
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位の総数分繰り返し
		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{ // オブジェクトの総数分繰り返し
			// NULLではなかった場合
			if (m_apObject[nCntPriority][nCntObject] != NULL)
			{ // オブジェクトの終了処理
				m_apObject[nCntPriority][nCntObject]->Uninit();
			}
		}
	}
#endif
}

//==================================================================================
// --- 全オブジェクトの更新処理 ---
//==================================================================================
void CObject::UpdateAll(void)
{
#if ENABLE_OBJECT_LIST
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位分回す
		CObject *pObject = m_apTop[nCntPriority];		// 先頭オブジェクトへのポインタ
		while (pObject != nullptr)
		{ // nullptrになるまで繰り返す
			CObject *pObjectNext = pObject->m_pNext;	// 次のオブジェクトへのポインタ

#ifdef DEBUG_ASSERT_TYPE_NONE
			// オブジェクトのタイプがNONEの場合、アサーション + ブレークポイント
			assert(pObject->m_type != TYPE_NONE);
			if (pObject->m_type == TYPE_NONE) __debugbreak();
#endif
			// オブジェクトの更新
			pObject->Update();

			pObject = pObjectNext;		// ポインタを次のオブジェクトへのポインタに変更
		}
	}

	// 死亡フラグを確認
	FlagCheckAll();
#else
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位の総数分繰り返し
		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{ // オブジェクトの総数分繰り返し
			// NULLではなかった場合
			if (m_apObject[nCntPriority][nCntObject] != NULL)
			{ // オブジェクトの更新処理
				m_apObject[nCntPriority][nCntObject]->Update();
			}
		}
	}
#endif
}

//==================================================================================
// --- 全オブジェクトの描画処理 ---
//==================================================================================
void CObject::DrawAll(void)
{
#if ENABLE_OBJECT_LIST
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位分回す
		CObject* pObject = m_apTop[nCntPriority];		// 先頭オブジェクトへのポインタ
		while (pObject != nullptr)
		{ // nullptrになるまで繰り返す
			CObject* pObjectNext = pObject->m_pNext;	// 次のオブジェクトへのポインタ

			// オブジェクトの描画
			pObject->Draw();

			pObject = pObjectNext;		// ポインタを次のオブジェクトへのポインタに変更
		}
	}
#else
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位の総数分繰り返し
		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{ // オブジェクトの総数分繰り返し
			// NULLではなかった場合
			if (m_apObject[nCntPriority][nCntObject] != NULL)
			{ // オブジェクトの描画処理
				m_apObject[nCntPriority][nCntObject]->Draw();
			}
		}
	}
#endif
}

//==================================================================================
// --- 全オブジェクトのフラグ確認処理 ---
//==================================================================================
void CObject::FlagCheckAll(void)
{
#if ENABLE_OBJECT_LIST
	// 死亡フラグを確認
	for (int nCntPriority = 0; nCntPriority < MAX_OBJPRIORITY; nCntPriority++)
	{ // 優先順位分回す
		CObject *pObject = m_apTop[nCntPriority];		// 先頭オブジェクトへのポインタ
		while (pObject != nullptr)
		{ // nullptrになるまで繰り返す
			CObject *pObjectNext = pObject->m_pNext;	// 次のオブジェクトへのポインタ

			if (pObject->m_bDeath == true)
			{ // 死亡フラグが立っていれば
				// リストから削除
				pObject->RemoveList();

				// 自分自身を破棄
				delete pObject;

				m_nNumAll--;		// オブジェクトの総数を減らす
			}

			pObject = pObjectNext;		// ポインタを次のオブジェクトへのポインタに変更
		}
	}
#endif
}

//==================================================================================
// --- 自身の解放処理 ---
//==================================================================================
void CObject::Release(void)
{
#if ENABLE_OBJECT_LIST
	// 死亡フラグを立てる
	m_bDeath = true;
#else
	if (m_apObject[m_nPriority][m_nID] != NULL)
	{ // 配列がNULLじゃなかったら (未解放なら)
		// 元居た配列にNULLを代入
		m_apObject[m_nPriority][m_nID] = NULL;
	}
	
	// 自分自身を破棄
	delete this;

	m_nNumAll--;		// 総数減少
#endif

	// 死亡したことを通知
	NofifyAllWhenDeath();
}

//==================================================================================
// --- 自身のリスト追加処理 ---
//==================================================================================
void CObject::AddList(void)
{
	CObject *&rpTop = m_apTop[m_nPriority];		// 先頭オブジェクトへのポインタ参照
	CObject *&rpCur = m_apCur[m_nPriority];		// 最後尾オブジェクトへのポインタ参照

	if (rpTop == nullptr && rpCur == nullptr)
	{ // もしリストが空なら
		rpTop = this;		// 先頭に自分を保存
		rpCur = this;		// 最後尾に自分を保存
	}
	else
	{ // リストに一つでも存在すれば
		rpCur->m_pNext = this;	// 最後尾のオブジェクトの次のオブジェクトへのポインタに自分を保存
		m_pPrev = rpCur;		// 最後尾のポインタを前のオブジェクトへのポインタに保存
		rpCur = this;			// 最後尾を自分に変更
	}
}

//==================================================================================
// --- 自身のリスト削除処理 ---
//==================================================================================
void CObject::RemoveList(void)
{
	CObject *&rpTop = m_apTop[m_nPriority];		// 先頭オブジェクトへのポインタ参照
	CObject *&rpCur = m_apCur[m_nPriority];		// 最後尾オブジェクトへのポインタ参照

	if (m_pNext != nullptr)
	{ // 次のオブジェクトが存在すれば
		m_pNext->m_pPrev = m_pPrev;		// 前のオブジェクトへのポインタを自分の前のオブジェクトへのポインタに変更
	}

	if (m_pPrev != nullptr)
	{ // 前のオブジェクトが存在すれば
		m_pPrev->m_pNext = m_pNext;		// 次のオブジェクトへのポインタを自分の次のオブジェクトへのポインタに変更
	}

	if (rpTop == this)
	{ // 先頭が自分なら
		rpTop = m_pNext;		// 先頭を次のオブジェクトに変更
	}

	if (rpCur == this)
	{ // 最後尾が自分なら
		rpCur = m_pPrev;		// 最後尾を前のオブジェクトに変更
	}
}

//==================================================================================
// --- 自身の死亡を観察者へ通知する処理 ---
//==================================================================================
void CObject::NofifyAllWhenDeath(void)
{ // 自身が死んだことを通知
	NotifyAll(NOTIFY_WHEN_DEATH);
}