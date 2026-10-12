//==================================================================================
// 
// コマンドマネージャクラスのヘッダーファイル [commandManager.h]
// Author : TENMA SAITO
// Date   : 2026/9/28
// 
//==================================================================================
#ifndef _COMMAND_MANAGER_H_
#define _COMMAND_MANAGER_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "command.h"
#include <list>
#include <memory>

//**********************************************************************************
// *** コマンド管理クラス ***
//**********************************************************************************
class CCommandManager
{
public:
	class CEmptyCommand : public CCommandBase
	{ // 空コマンド
	public:
		CEmptyCommand() = default;
		~CEmptyCommand() = default;

		void Do(void) override {}
		void Undo(void) override {}
	};

	CCommandManager() = default;
	~CCommandManager() = default;

	void SetUp(void)
	{ // 空コマンドを作成
		m_lpCommands.emplace_back(std::make_shared<CEmptyCommand>());

		// イテレーターに先頭を指定
		m_commIter = m_lpCommands.begin();
	}

	void Do(std::shared_ptr<CCommandBase> command)
	{ // コマンドを実行
		command->Do();

		// イテレーターが最後尾でなければ、Undo済みのコマンドを削除する
		if (m_commIter != std::prev(m_lpCommands.end()))
		{ // イテレーター以降のコマンド削除
			m_lpCommands.erase(std::next(m_commIter), m_lpCommands.end());
		}

		// コマンドを追加
		m_lpCommands.emplace_back(command);

		// イテレーターを更新
		m_commIter = std::prev(m_lpCommands.end());
	}

	template<class CommandType, typename... Args>
	void Do(Args... args)
	{ // コマンドのインスタンスを生成し、コマンドを実行
		auto pCommand = std::make_shared<CommandType>(args...);
		Do(pCommand);
	}

	void Undo(void)
	{ // 既に最初のイテレーター(空コマンド)の場合、何もしない
		if (m_commIter == m_lpCommands.begin()) return;

		// 現在のコマンドを取得
		std::shared_ptr<CCommandBase> spCommand = *m_commIter;
		if (spCommand == nullptr)
		{ // コマンドが存在しない場合、スキップ
			return;
		}

		// 取り消し処理を実行
		spCommand->Undo();

		// イテレーターを一つ下げる
		m_commIter = std::prev(m_commIter);
	}

	void Redo(void)
	{ // イテレーターが最後のコマンドを指している場合、スキップ
		if (m_commIter == std::prev(m_lpCommands.end())) return;

		// イテレーターを一つ進める
		m_commIter++;

		// イテレーターの指すコマンドを実行
		m_commIter->get()->Do();
	}

private:
	std::list<std::shared_ptr<CCommandBase>> m_lpCommands;				// 実行したコマンドを保存するリスト
	std::list<std::shared_ptr<CCommandBase>>::iterator m_commIter;		// コマンドイテレーター
};
#endif