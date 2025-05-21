#pragma once
#include <initializer_list>

namespace Tridium {

	//=================================================================================================
	// InitList
	//  A simple wrapper around std::initializer_list.
	//  A lightweight proxy object that provides a read-only view into an array of objects of type T.
    template <class T>
    class InitList {
    public:
        using ValueType = T;
        using Reference = const T&;
        using ConstReference = const T&;
        using SizeType = size_t;

        using Iterator = const T*;
        using ConstIterator = const T*;

        constexpr InitList() noexcept 
            : m_First( nullptr ), m_Last( nullptr ) {}

        constexpr InitList( const T* a_First, const T* a_Last ) noexcept
            : m_First( a_First ), m_Last( a_Last ) {}

		constexpr InitList( std::initializer_list<T> a_InitializerList ) noexcept
			: m_First( a_InitializerList.begin() ), m_Last( a_InitializerList.end() ) {}

		[[nodiscard]] constexpr const T* Begin() const noexcept { return m_First; }

        [[nodiscard]] constexpr const T* End() const noexcept { return m_Last; }

        [[nodiscard]] constexpr size_t Size() const noexcept { return Cast<size_t>(m_Last - m_First); }

		constexpr operator std::initializer_list<T>() const noexcept {
			return std::initializer_list<T>( m_First, m_Last );
		}

		[[nodiscard]] constexpr const T& operator[]( size_t a_Index ) const noexcept {
			return m_First[a_Index];
		}

		[[nodiscard]] constexpr const T& At( size_t a_Index ) const noexcept {
			return m_First[a_Index];
		}

    private:
        const T* m_First;
        const T* m_Last;
    };

    template <class T>
    [[nodiscard]] constexpr const T* begin( InitList<T> _Ilist ) noexcept {
        return _Ilist.Begin();
    }

    template <class T>
    [[nodiscard]] constexpr const T* end( InitList<T> _Ilist ) noexcept {
        return _Ilist.End();
    }

}