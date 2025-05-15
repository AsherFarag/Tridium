#pragma once
#include <Tridium/Core/Assert.h>
#include <array>
#include <vector>
#include <type_traits>
#include "InitList.h"
#include "Span.h"

namespace Tridium {

#ifndef CONFIG_ENABLE_BOUNDS_CHECK
	#if CONFIG_DEBUG
		#define CONFIG_ENABLE_BOUNDS_CHECK 1
	#else
		#define CONFIG_ENABLE_BOUNDS_CHECK 0
	#endif // CONFIG_DEBUG
#endif

#if CONFIG_ENABLE_BOUNDS_CHECK
	#define TRIDIUM_ARRAY_ASSERT( _Condition, _Message ) \
		if ( std::is_constant_evaluated() ) \
		{ \
			if ( !(_Condition) ) \
				throw "Assertion failed: " _Message; \
		} \
		else \
		{ \
			ENSURE( _Condition, _Message ); \
		}
#else
	#define TRIDIUM_ARRAY_ASSERT( _Condition, _Message )
#endif

	//=================================================================================================
	// FixedArray
	//  Fixed Size Array
	//  An array of contiguous memory with a fixed size.
	//  STD equivalent: std::array
	//=================================================================================================
	template<typename T, size_t _Size>
	class FixedArray
	{
	public:
		using Iterator = typename std::array<T, _Size>::iterator;
		using ConstIterator = typename std::array<T, _Size>::const_iterator;
		using ReverseIterator = typename std::array<T, _Size>::reverse_iterator;
		using ConstReverseIterator = typename std::array<T, _Size>::const_reverse_iterator;

		constexpr FixedArray() = default;
		constexpr FixedArray( const FixedArray& a_Other ) = default;
		constexpr FixedArray( FixedArray&& a_Other ) = default;
		constexpr FixedArray( InitList<T> a_InitList ) { Fill( a_InitList ); }
		constexpr FixedArray( Span<const T> a_InitList ) { Fill( a_InitList ); }
		constexpr FixedArray& operator=( const FixedArray& a_Other ) = default;
		constexpr FixedArray& operator=( FixedArray&& a_Other ) = default;

		constexpr operator Span<T>() { return Span<T>( m_Data.data(), _Size ); }
		constexpr operator Span<const T>() const { return Span<const T>( m_Data.data(), _Size ); }

		constexpr T& operator[]( size_t a_Index )
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Data[a_Index];
		}

		constexpr const T& operator[]( size_t a_Index ) const
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Data[a_Index];
		}

		constexpr T& At( size_t a_Index )
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Data[a_Index];
		}

		constexpr const T& At( size_t a_Index ) const
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Data[a_Index];
		}

		constexpr T& Front() { return m_Data.front(); }
		constexpr const T& Front() const { return m_Data.front(); }

		constexpr T& Back() { return m_Data.back(); }
		constexpr const T& Back() const { return m_Data.back(); }

		constexpr T* Data() { return m_Data.data(); }
		constexpr const T* Data() const { return m_Data.data(); }

		constexpr size_t Size() const { return _Size; }
		constexpr size_t MaxSize() const { return _Size; }
		constexpr bool IsEmpty() const { return false; }
		constexpr bool IsValidIndex( size_t a_Index ) const { return ( ( a_Index >= 0 ) && ( a_Index < _Size ) ); }

		constexpr void Swap( FixedArray& a_Other ) { m_Data.swap( a_Other.m_Data ); }
		constexpr void Fill( const T& a_Value ) { m_Data.fill( a_Value ); }
		constexpr void Fill( InitList<T> a_InitializerList )
		{
			TRIDIUM_ARRAY_ASSERT( a_InitializerList.Size() <= MaxSize(), "Initializer list is too large" );
			for ( size_t i = 0; i < a_InitializerList.Size() && i < MaxSize(); ++i )
			{
				m_Data[i] = a_InitializerList[i];
			}
		}

		constexpr void Fill( Span<const T> a_Data )
		{
			TRIDIUM_ARRAY_ASSERT( a_Data.size() <= MaxSize(), "Initializer list is too large" );
			for ( size_t i = 0; i < a_Data.size() && i < MaxSize(); ++i )
			{
				m_Data[i] = a_Data[i];
			}
		}

		constexpr Iterator Begin() { return m_Data.begin(); }
		constexpr ConstIterator Begin() const { return m_Data.begin(); }
		constexpr Iterator End() { return m_Data.end(); }
		constexpr ConstIterator End() const { return m_Data.end(); }

		constexpr ReverseIterator RBegin() { return m_Data.rbegin(); }
		constexpr ConstReverseIterator RBegin() const { return m_Data.rbegin(); }
		constexpr ReverseIterator REnd() { return m_Data.rend(); }
		constexpr ConstReverseIterator REnd() const { return m_Data.rend(); }

		constexpr auto begin() { return m_Data.begin(); }
		constexpr auto begin() const { return m_Data.begin(); }
		constexpr auto end() { return m_Data.end(); }
		constexpr auto end() const { return m_Data.end(); }

		constexpr auto rbegin() { return m_Data.rbegin(); }
		constexpr auto rbegin() const { return m_Data.rbegin(); }
		constexpr auto rend() { return m_Data.rend(); }
		constexpr auto rend() const { return m_Data.rend(); }

	private:
		std::array<T, _Size> m_Data{};
	};
	// ================================================================================================

	//=================================================================================================
	// InlineArray
	//  Fixed Size Array with an API similar to std::vector/Tridium::Array
	//  An array of contiguous memory with a fixed MAX size.
	//  STD equivalent: std::array
	//=================================================================================================
	template<typename T, size_t _Size>
	class InlineArray
	{
	public:
		using Storage = FixedArray<T, _Size>;
		using Iterator = typename Storage::Iterator;
		using ConstIterator = typename Storage::ConstIterator;
		using ReverseIterator = typename Storage::ReverseIterator;
		using ConstReverseIterator = typename Storage::ConstReverseIterator;

		constexpr InlineArray() = default;
		constexpr InlineArray( const InlineArray& a_Other ) = default;
		constexpr InlineArray( InlineArray&& a_Other ) = default;
		constexpr InlineArray& operator=( const InlineArray& a_Other ) = default;
		constexpr InlineArray& operator=( InlineArray&& a_Other ) = default;
		constexpr InlineArray( std::initializer_list<T> a_InitializerList )
			: m_Size( 0 )
		{
			for ( const T& value : a_InitializerList )
			{
				PushBack( value );
			}
		}

		constexpr operator Span<T>() { return Span<T>( m_Storage.Data(), m_Size ); }
		constexpr operator Span<const T>() const { return Span<const T>( m_Storage.Data(), m_Size ); }

		constexpr T& operator[]( size_t a_Index ) { return m_Storage[a_Index]; }
		constexpr const T& operator[]( size_t a_Index ) const { return m_Storage[a_Index]; }

		constexpr T& At( size_t a_Index ) { return m_Storage.At( a_Index ); }
		constexpr const T& At( size_t a_Index ) const { return m_Storage.At( a_Index ); }
		constexpr T& Front() { return m_Storage.Front(); }
		constexpr const T& Front() const { return m_Storage.Front(); }
		constexpr T& Back() { return m_Storage.Back(); }
		constexpr const T& Back() const { return m_Storage.Back(); }
		constexpr T* Data() { return m_Storage.Data(); }
		constexpr const T* Data() const { return m_Storage.Data(); }
		constexpr size_t Size() const { return m_Size; }
		constexpr size_t MaxSize() const { return _Size; }
		constexpr bool IsEmpty() const { return ( m_Size == 0 ); }
		constexpr bool IsValidIndex( size_t a_Index ) const { return ( ( a_Index >= 0 ) && ( a_Index < m_Size ) ); }
		constexpr void Fill( const T& a_Value ) { m_Storage.Fill( a_Value ); m_Size = MaxSize(); }
		constexpr void Swap( InlineArray& a_Other ) { m_Storage.Swap( a_Other.m_Storage ); }
		constexpr void Resize( size_t a_Size ) { m_Size = Math::Min( a_Size, MaxSize() ); }

		constexpr void Fill( const T& a_Value, size_t a_Size )
		{
			TRIDIUM_ARRAY_ASSERT( a_Size <= MaxSize(), "Inline Array is full" );
			if ( a_Size < m_Size )
			{
				// Destroy the elements beyond the new size
				for ( size_t i = a_Size; i < m_Size; ++i )
				{
					m_Storage[i].~T();
				}
			}

			for ( size_t i = 0; i < a_Size; ++i )
			{
				m_Storage[i] = a_Value;
			}
		}

		constexpr void Clear()
		{
			for ( size_t i = 0; i < m_Size; ++i )
			{
				m_Storage[i].~T();
			}
			m_Size = 0;
		}

		constexpr void PushBack( const T& a_Value )
		{
			TRIDIUM_ARRAY_ASSERT( m_Size < MaxSize(), "Inline Array is full" );
			m_Storage[m_Size++] = a_Value;
		}

		constexpr void PopBack()
		{
			if ( m_Size == 0 )
				return;

			m_Storage[--m_Size].~T();
		}

		template<typename... _Args>
		constexpr auto& EmplaceBack( _Args&&... a_Args )
		{
			TRIDIUM_ARRAY_ASSERT( m_Size < MaxSize(), "Inline Array is full" );
			return m_Storage[m_Size++] = T( std::forward<_Args>( a_Args )... );
		}

		constexpr Iterator Insert( Iterator a_Position, const T& a_Value )
		{
			TRIDIUM_ARRAY_ASSERT( m_Size < MaxSize(), "Inline Array is full" );
			for ( Iterator it = m_Storage.End(); it != a_Position; --it )
			{
				*it = *( it - 1 );
			}
			*a_Position = a_Value;
			++m_Size;
			return a_Position;
		}

		constexpr Iterator Erase( Iterator a_Position )
		{
			for ( Iterator it = a_Position; it != m_Storage.End() - 1; ++it )
			{
				*it = std::move( *( it + 1 ) );
			}
			m_Size--;

			return a_Position;
		}

		constexpr Iterator Erase( Iterator a_First, Iterator a_Last )
		{
			size_t count = a_Last - a_First;
			for ( Iterator it = a_First; it != m_Storage.End() - count; ++it )
			{
				*it = *( it + count );
			}
			m_Size -= count;
			return a_First;
		}

		constexpr Iterator Begin() { return m_Storage.Begin(); }
		constexpr ConstIterator Begin() const { return m_Storage.Begin(); }
		constexpr Iterator End() { return m_Storage.Begin() + m_Size; }
		constexpr ConstIterator End() const { return m_Storage.Begin() + m_Size; }

		constexpr ReverseIterator RBegin() { return m_Storage.RBegin(); }
		constexpr ConstReverseIterator RBegin() const { return m_Storage.RBegin(); }
		constexpr ReverseIterator REnd() { return m_Storage.RBegin() + m_Size; }
		constexpr ConstReverseIterator REnd() const { return m_Storage.RBegin() + m_Size; }

		constexpr auto begin() { return m_Storage.begin(); }
		constexpr auto begin() const { return m_Storage.begin(); }
		constexpr auto end() { return m_Storage.begin() + m_Size; }
		constexpr auto end() const { return m_Storage.begin() + m_Size; }

		constexpr auto rbegin() { return m_Storage.rbegin(); }
		constexpr auto rbegin() const { return m_Storage.rbegin(); }
		constexpr auto rend() { return m_Storage.rbegin() + m_Size; }
		constexpr auto rend() const { return m_Storage.rbegin() + m_Size; }

	private:
		Storage m_Storage{};
		size_t m_Size = 0;
	};


	//=================================================================================================
	// Array
	//  Dynamic Size Array
	//  An array of contiguous memory with dynamic size.
	//  STD equivalent: std::vector
	//=================================================================================================
	template<typename T>
	class Array
	{
	public:
		using Iterator = typename std::vector<T>::iterator;
		using ConstIterator = typename std::vector<T>::const_iterator;
		using ReverseIterator = typename std::vector<T>::reverse_iterator;
		using ConstReverseIterator = typename std::vector<T>::const_reverse_iterator;

		Array() : m_Data() {}
		Array( const Array& a_Other ) = default;
		Array( Array&& a_Other ) = default;
		Array( size_t a_Size ) : m_Data( a_Size ) {}
		Array( std::initializer_list<T> a_InitializerList ) : m_Data( a_InitializerList ) {}
		Array& operator=( const Array& a_Other ) = default;
		Array& operator=( Array&& a_Other ) = default;

		operator Span<T>() { return Span<T>( m_Data.data(), m_Data.size() ); }
		operator Span<const T>() const { return Span<const T>( m_Data.data(), m_Data.size() ); }

		T& operator[]( size_t a_Index )
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Data[a_Index];
		}

		const T& operator[]( size_t a_Index ) const
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Data[a_Index];
		}

		T& At( size_t a_Index )
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Data[a_Index];
		}

		const T& At( size_t a_Index ) const
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Data[a_Index];
		}

		T& Front() { return m_Data.front(); }
		const T& Front() const { return m_Data.front(); }

		T& Back() { return m_Data.back(); }
		const T& Back() const { return m_Data.back(); }

		T* Data() { return m_Data.data(); }
		const T* Data() const { return m_Data.data(); }

		size_t Size() const { return m_Data.size(); }
		size_t MaxSize() const { return m_Data.max_size(); }
		size_t Capacity() const { return m_Data.capacity(); }
		bool IsEmpty() const { return m_Data.empty(); }
		bool IsValidIndex( size_t a_Index ) const { return ( ( a_Index >= 0 ) && ( a_Index < m_Data.size() ) ); }
		void Fill( const T& a_Value ) { m_Data.fill( a_Value ); }
		void Swap( Array& a_Other ) { m_Data.swap( a_Other.m_Data ); }
		void Resize( size_t a_Size ) { m_Data.resize( a_Size ); }
		void Resize( size_t a_Size, const T& a_Value ) { m_Data.resize( a_Size, a_Value ); }
		void Reserve( size_t a_Size ) { m_Data.reserve( a_Size ); }
		void ShrinkToFit() { m_Data.shrink_to_fit(); }
		void Clear() { m_Data.clear(); }
		void PushBack( const T& a_Value ) { m_Data.push_back( a_Value ); }
		void PopBack() { m_Data.pop_back(); }

		template<typename... _Args>
		auto& EmplaceBack( _Args&&... a_Args ) { return m_Data.emplace_back( std::forward<_Args>( a_Args )... ); }

		Iterator Erase( Iterator a_Position ) { return m_Data.erase( a_Position ); }
		Iterator Erase( Iterator a_First, Iterator a_Last ) { return m_Data.erase( a_First, a_Last ); }

		Iterator Insert( Iterator a_Position, const T& a_Value ) { return m_Data.insert( a_Position, a_Value ); }
		template<typename _Iter>
		Iterator Insert( Iterator a_Position, _Iter a_First, _Iter a_Last ) { return m_Data.insert( a_Position, a_First, a_Last ); }

		Iterator Begin() { return m_Data.begin(); }
		ConstIterator Begin() const { return m_Data.begin(); }
		Iterator End() { return m_Data.end(); }
		ConstIterator End() const { return m_Data.end(); }

		ReverseIterator RBegin() { return m_Data.rbegin(); }
		ConstReverseIterator RBegin() const { return m_Data.rbegin(); }
		ReverseIterator REnd() { return m_Data.rend(); }
		ConstReverseIterator REnd() const { return m_Data.rend(); }

		auto begin() { return m_Data.begin(); }
		auto begin() const { return m_Data.begin(); }
		auto end() { return m_Data.end(); }
		auto end() const { return m_Data.end(); }

		auto rbegin() { return m_Data.rbegin(); }
		auto rbegin() const { return m_Data.rbegin(); }
		auto rend() { return m_Data.rend(); }
		auto rend() const { return m_Data.rend(); }

	private:
		std::vector<T> m_Data;
	};

}