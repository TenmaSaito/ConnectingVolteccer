//==================================================================================
// 
// 自作デリゲーター型クラスのヘッダーファイル [delegate_t.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _DELEGATE_T_H_
#define _DELEGATE_T_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include <string>
#include <functional>
#include <vector>
#include <tuple>

//**********************************************************************************
// *** 自作デリゲーター型クラス ***
//**********************************************************************************
namespace hyp
{
	template<class> class Delegate {};

	template<class Return, class... Args> class Delegate<Return(Args...)>
	{
	public:
		using _PtrRef = Return(Args...);
		using _Ptr = std::function<_PtrRef>;
		using _Func = std::function<_PtrRef>;
		using _Key = std::pair<void*, void*>;

		struct FuncInfo
		{
			_Func pFunc;		// 関数ポインタ
			_Key key;			// 鍵
		};

		Delegate();
		template<class T> Delegate(T *pFunc);
		template<class T, class Instance> Delegate(T pFunc, Instance* pInst);
		Delegate(const Delegate &copy);
		~Delegate();

		template<class T> void Add(T *pFunc);
		template<class T, class Instance> void Add(T pFunc, Instance *pInst);
		Return InvokeAll(Args... args);
		void UnregisterAll(void);
		operator bool(void) const { return m_apFunction.size() > 0; }

	private:
		template<class T> void *ConvertToVoid(T pFunc);
		auto Find(void *pFirst, void *pSecond = nullptr);

		std::vector<FuncInfo> m_apFunction;	// 関数ポインタ群
	};

	//=================================================================================
	// --- コンストラクタ ---
	//=================================================================================
	template<class Return, class... Args> Delegate<Return(Args...)>::Delegate()
	{
	}

	//=================================================================================
	// --- コンストラクタのオーバーロード ---
	//=================================================================================
	template<class Return, class... Args> 
	template<class T> Delegate<Return(Args...)>::Delegate(T *pFunc)
	{
		Add(pFunc);
	}

	//=================================================================================
	// --- コンストラクタのオーバーロード ---
	//=================================================================================
	template<class Return, class... Args>
	template<class T, class Instance> Delegate<Return(Args...)>::Delegate(T pFunc, Instance *pInst)
	{
		Add(pFunc, pInst);
	}

	//=================================================================================
	// --- コピーコンストラクタ ---
	//=================================================================================
	template<class Return, class... Args> Delegate<Return(Args...)>::Delegate(const Delegate &copy)
	{ // 配列コピー
		m_apFunction = copy.m_apFunction;
	}

	//=================================================================================
	// --- デストラクタ ---
	//=================================================================================
	template<class Return, class... Args> Delegate<Return(Args...)>::~Delegate()
	{
	}

	//=================================================================================
	// --- 関数ポインタ追加処理 ---
	//=================================================================================
	template<class Return, class... Args> template<class T> void Delegate<Return(Args...)>::Add(T *pFunc)
	{
		if (Find(pFunc) != m_apFunction.cend()) return;

		m_apFunction.push_back({ [=](Args... args)
			{
				return pFunc(std::forward<Args>(args)...);
			}, _Key(pFunc, nullptr) });
	}

	//=================================================================================
	// --- メンバ関数ポインタ追加処理 ---
	//=================================================================================
	template<class Return, class... Args> template<class T, class Instance> void Delegate<Return(Args...)>::Add(T pFunc, Instance *pInst)
	{
		void *pVoid = ConvertToVoid(pFunc);
		if (Find(pVoid, pInst) != m_apFunction.cend()) return;

		m_apFunction.push_back({ [pInst, pFunc](Args... args)
			{
				return (pInst->*pFunc)(std::forward<Args>(args)...);
			}, _Key(pVoid, pInst) });
	}

	//=================================================================================
	// --- 検索処理 ---
	//=================================================================================
	template<class Return, class... Args> auto Delegate<Return(Args...)>::Find(void *pFirst, void *pSecond)
	{
		_Key key{ pFirst, pSecond };		// 鍵
		auto find = [&](FuncInfo &info) {return info.key == key; };		// ラムダ式

		// 見つけたイテレーターを返す
		return std::find_if(m_apFunction.begin(), m_apFunction.end(), find);
	}

	//=================================================================================
	// --- void*変換処理 ---
	//=================================================================================
	template<class Return, class... Args>
	template<class T> void *Delegate<Return(Args...)>::ConvertToVoid(T pFunc)
	{
		typedef union
		{
			T ptr;
			void *pVoid;
		} PtrToVoid;

		PtrToVoid ret;		// 変換用変数

		ret.ptr = pFunc;
		return ret.pVoid;
	}

	//=================================================================================
	// --- 関数呼び出し処理 ---
	//=================================================================================
	template<class Return, class... Args> Return Delegate<Return(Args...)>::InvokeAll(Args... args)
	{
		for (auto &func : m_apFunction)
		{
			if (func.pFunc)
			{ // 呼び出し可能なら、関数呼び出し
				func.pFunc(args...);
			}
		}
	}

	//=================================================================================
	// --- 登録済みデリゲーターの登録解除処理 ---
	//=================================================================================
	template<class Return, class... Args> void Delegate<Return(Args...)>::UnregisterAll(void)
	{
		m_apFunction.clear();
	}

	// 別名設定
	template<class... Args> using Action = Delegate<void(Args...)>;
	template<class Return, class... Args> using Func = Delegate<Return(Args...)>;

	template<class Return, class... Args>
	class CCallBacker
	{
	public:
		constexpr CCallBacker(Return(*pInvoke)(Args...)) : m_pInvoke(pInvoke) {}
		Return Invoke(Args... args) { return (*m_pInvoke)(std::forward<Args>(args)...); }

	private:
		Return(*m_pInvoke)(Args...);
	};

	template<class Return, class... Args>
	struct ICallback
	{
		virtual ~ICallback() = default;
		virtual Return Invoke(Args... args) = 0;
	};

	template<class T, class Return, class... Args>
	class CMemberCallBacker : public ICallback<Return, Args...>
	{
	public:
		constexpr CMemberCallBacker(T *pInst, Return(T::*pInvoke)(Args...)) : m_pInstance(pInst), m_pInvoke(pInvoke) {}
		constexpr Return Invoke(Args... args) { return (m_pInstance->*m_pInvoke)(std::forward<Args>(args)...); }

	private:
		T *m_pInstance;
		Return(T::*m_pInvoke)(Args...);
	};

	template<class Return, class... Args>
	class CMemberDelegete
	{
	public:
		template<class T> void Add(T *pInst, Return(T::*pInvoke)(Args...)) { m_vpCallback.emplace_back(std::make_unique<CMemberCallBacker<T, Return, Args...>>(pInst, pInvoke)); }
		Return InvokeAll(Args... args) 
		{
			for (auto &pCallback : m_vpCallback)
			{
				pCallback->Invoke(std::forward<Args>(args)...);
			}
		}

	private:
		std::vector<std::unique_ptr<ICallback<Return, Args...>>> m_vpCallback;
	};
}

//**********************************************************************************
// *** 自作デリゲーターの補助関数 ***
//**********************************************************************************
namespace hyp
{
	template<typename T>
	struct RemoveClass {};

	template<class Return, class... Args>
	struct RemoveClass<Return(*)(Args...)>
	{
		using type = Return(Args...);
	};

	template<class Return, class Class, class... Args>
	struct RemoveClass<Return(Class::*)(Args...)>
	{
		using type = Return(Args...);
	};

	template<class Return, class Class, class... Args>
	struct RemoveClass<Return(Class::*)(Args...) const>
	{
		using type = Return(Args...) const;
	};

	template <class Return>
	auto MakeDelegate(Return Ret)
	{
		using type = decltype(Ret);
		return Delegate<RemoveClass<type>::type>();
	}
}
#endif
