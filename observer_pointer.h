//==================================================================================
// 
// 非所有ポインタクラスのヘッダーファイル [observer_ptr.h]
// Author : TENMA SAITO
// Date   : 2026/8/22
// 
//==================================================================================
#ifndef _OBSERVER_PTR_H_		// インクルードガード
#define _OBSERVER_PTR_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include <memory>

//**********************************************************************************
// *** 名前空間 ***
//**********************************************************************************
namespace own
{
    //******************************************************************************
    // 
    // 非所有ポインタ
    // 
    // 生ポインタやユニークポインタのポインタを所有権を保持せずに
    // 中身のみ使う場合に使用するポインタ
    // 所有権を持たないため、生ポインタ取得後のdeleteは不可能
    // 
    // 生ポインタの場合、コンストラクタで受け取ったポインタで処理実行
    // ユニークポインタの場合、コンストラクタで受け取ったユニークポインタからポインタを取得し、処理実行
    // 
    // Caution :
    // ポインタを渡した後は、生ポインタの変更も参照も出来なくなるため、
    // 非所有ポインタに直にインスタンス生成をするのは厳禁
    // 
    //******************************************************************************
    template<class T> class ObserverPtr
    {
    public:
        ObserverPtr() = delete;
        ObserverPtr(std::nullptr_t) = delete;
        constexpr ObserverPtr(const ObserverPtr &other) = delete;
        constexpr ObserverPtr(ObserverPtr &&right) noexcept : m_pRawPtr(right.m_pRawPtr), m_ppPtr(right.m_ppPtr)
        {
            right.m_ppPtr = nullptr;
            right.m_pRawPtr = nullptr;
        }

        explicit constexpr ObserverPtr(T *pPtr) : m_pRawPtr(pPtr), m_ppPtr(nullptr) {}
        explicit constexpr ObserverPtr(const std::unique_ptr<T> &rpPtr) : m_pRawPtr(nullptr), m_ppPtr(&rpPtr) {}
        constexpr ~ObserverPtr() = default;

        constexpr T *operator->() const;
        constexpr T &operator*() const;
        constexpr ObserverPtr &operator=(std::nullptr_t);
        ObserverPtr &operator=(T *pPtr) = delete;
        ObserverPtr &operator=(const std::unique_ptr<T> &rpPtr) = delete;
        ObserverPtr &operator=(const ObserverPtr &) = delete;
        ObserverPtr &operator=(ObserverPtr &&) = delete;
        explicit constexpr operator bool() const;

    private:
        T *m_pRawPtr;     // 実際のポインタ
        const std::unique_ptr<T> *m_ppPtr;      // ポインタ所持者の参照
    };

    // 演算子のオーバーロード (nullptr_tによる代入演算子)
    template<class T> constexpr ObserverPtr<T> &ObserverPtr<T>::operator=(std::nullptr_t)
    {
        m_pRawPtr = nullptr;
        m_ppPtr = nullptr;
        return *this;
    }

    // 演算子のオーバーロード (アロー演算子)
    template<class T> constexpr T *ObserverPtr<T>::operator->() const
    {
        if (m_ppPtr) return m_ppPtr->get();
        return m_pRawPtr;
    }

    // 演算子のオーバーロード (間接参照演算子)
    template<class T> constexpr T &ObserverPtr<T>::operator*() const
    {
        if (m_ppPtr) return **m_ppPtr;
        return *m_pRawPtr;
    }

    // bool型へのキャスト
    template<class T> constexpr ObserverPtr<T>::operator bool() const
    {
        if (m_ppPtr) return *m_ppPtr != nullptr;
        return m_pRawPtr != nullptr;
    }

    // オブザーバーポインタの生成補助関数 (生ポインタ)
    template<class T> constexpr ObserverPtr<T> Make_Observer(T *pPtr)
    {
        return ObserverPtr<T>(pPtr);
    }
}
#endif