#pragma once
#include <Tridium/Core/Assert.h>
#include "InitList.h"
#include "Span.h"
#include <array>
#include <vector>
#include <type_traits>
#include <utility>
#include <algorithm>

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
		constexpr FixedArray( InitList<T> a_InitList ) { TRIDIUM_ARRAY_ASSERT( a_InitList.size() <= MaxSize(), "Initializer list is too large" ); Fill( a_InitList ); }
		constexpr FixedArray( Span<const T> a_InitList ) { TRIDIUM_ARRAY_ASSERT( a_InitList.size() <= MaxSize(), "Initializer list is too large" ); Fill( a_InitList ); }
		constexpr FixedArray& operator=( const FixedArray& a_Other ) = default;
		constexpr FixedArray& operator=( FixedArray&& a_Other ) = default;

		constexpr operator Span<T>() { return Span<T>( m_Data.data(), _Size ); }
		constexpr operator Span<const T>() const { return Span<const T>( m_Data.data(), _Size ); }

		constexpr T& operator[]( size_t a_Index ) { return At( a_Index ); }
		constexpr const T& operator[]( size_t a_Index ) const { return At( a_Index ); }

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
		constexpr bool Empty() const { return false; }
		constexpr bool IsValidIndex( size_t a_Index ) const { return a_Index < _Size; }

		constexpr void Swap( FixedArray& a_Other ) { m_Data.swap( a_Other.m_Data ); }
		constexpr void Fill( const T& a_Value ) { m_Data.fill( a_Value ); }
		constexpr void Fill( InitList<T> a_InitializerList )
		{
			for ( size_t i = 0; i < a_InitializerList.Size() && i < MaxSize(); ++i )
			{
				m_Data[i] = a_InitializerList[i];
			}
		}

		constexpr void Fill( Span<const T> a_Data )
		{
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

	private:
		std::array<T, _Size> m_Data{};
	};
	// ================================================================================================

	//=================================================================================================
	// InlineArray
	//  Fixed Size Array with an API similar to std::vector/Tridium::Array
	//  An array of contiguous memory with a fixed MAX size.
	//  STD equivalent: std::array
	template<typename _Value, size_t _Size>
	class InlineArray
	{
	public:
		using ValueType = _Value;
		using Iterator = _Value*;
		using ConstIterator = const _Value*;
		using ReverseIterator = std::reverse_iterator<Iterator>;
		using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

		//============================================================================================
		// Default constructor
		constexpr InlineArray() 
			: m_Size( 0 ) {}

		//============================================================================================
		// Copy constructor
		// NOTE: a_Other can be of any size and this array will attempt to copy as many elements as possible
		template<size_t _OtherSize>
		constexpr InlineArray( const InlineArray<_Value, _OtherSize>& a_Other )
			: m_Size( 0 )
		{
			for ( const _Value& value : a_Other )
				PushBack( value );
		}

		//============================================================================================
		// Move constructor
		// WARNING: a_Other is required to be able to fit into this array.
		template<size_t _OtherSize>
		constexpr InlineArray( InlineArray<_Value, _OtherSize>&& a_Other )
			: m_Size( a_Other.m_Size )
		{
			TRIDIUM_ARRAY_ASSERT( a_Other.Size() <= MaxSize(), "Array size is out of bounds" );
			for ( size_t i = 0; i < m_Size; ++i )
				std::construct_at( &m_Storage[i], std::move( a_Other.m_Storage[i] ) );
			a_Other.m_Size = 0;
		}

		//============================================================================================
		// Copy constructor from InitList
		constexpr InlineArray( InitList<_Value> a_List )
			: m_Size( 0 )
		{
			TRIDIUM_ARRAY_ASSERT( a_List.Size() <= MaxSize(), "Initializer list is too large" );
			Fill( a_List );
		}

		//============================================================================================
		// Copy constructor from Span
		constexpr InlineArray( Span<const _Value> a_Span )
			: m_Size( 0 )
		{
			TRIDIUM_ARRAY_ASSERT( a_Span.size() <= MaxSize(), "Initalizer span is too large" );
			Fill( a_Span );
		}

		//============================================================================================
		// Copy Assignment
		// NOTE: a_Other can be of any size and this array will attempt to copy as many elements as possible.
		//		 This also clears the current array.
		template<size_t _OtherSize>
		constexpr InlineArray& operator=( const InlineArray<_Value, _OtherSize>& a_Other ) 
		{
			if ( this != &a_Other )
			{
				Clear();
				for ( const _Value& value : a_Other )
					PushBack( value );
			}
			return *this;
		}

		//============================================================================================
		// Move Assignment
		// WARNING: a_Other is required to be able to fit into this array.
		//		    This also clears the current array.
		template<size_t _OtherSize>
		constexpr InlineArray& operator=( InlineArray<_Value, _OtherSize>&& a_Other )
		{
			if ( this != &a_Other )
			{
				TRIDIUM_ARRAY_ASSERT( a_Other.Size() <= MaxSize(), "Array size is out of bounds" );
				Clear();
				m_Size = a_Other.m_Size;
				for ( size_t i = 0; i < m_Size; ++i )
					std::construct_at( &m_Storage[i], std::move( a_Other.m_Storage[i] ) );
				a_Other.m_Size = 0;
			}
			return *this;
		}

		//============================================================================================
		// Copy Assignment from InitList
		constexpr InlineArray& operator=( InitList<_Value> a_List )
		{
			TRIDIUM_ARRAY_ASSERT( a_List.Size() <= MaxSize(), "Initializer list is too large" );
			Fill( a_List );
			return *this;
		}

		//============================================================================================
		// Copy Assignment from Span
		constexpr InlineArray& operator=( Span<const _Value> a_Span )
		{
			TRIDIUM_ARRAY_ASSERT( a_Span.size() <= MaxSize(), "Initalizer span is too large" );
			Fill( a_Span );
			return *this;
		}

		constexpr ~InlineArray() { Clear(); }

		//============================================================================================
		// Access
		constexpr _Value& operator[]( size_t a_Index ) { return At( a_Index ); }
		constexpr const _Value& operator[]( size_t a_Index ) const { return At( a_Index ); }

		constexpr _Value& At( size_t a_Index )
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Storage[a_Index];
		}

		constexpr const _Value& At( size_t a_Index ) const
		{
			TRIDIUM_ARRAY_ASSERT( IsValidIndex( a_Index ), "Index out of bounds" );
			return m_Storage[a_Index];
		}

		constexpr _Value& Front() { TRIDIUM_ARRAY_ASSERT( !Empty(), "Array is empty"); return At(0); }
		constexpr const _Value& Front() const { TRIDIUM_ARRAY_ASSERT( !Empty(), "Array is empty" ); return At( 0 ); }

		constexpr _Value& Back() { TRIDIUM_ARRAY_ASSERT( !Empty(), "Array is empty" ); return At( m_Size - 1 ); }
		constexpr const _Value& Back() const { TRIDIUM_ARRAY_ASSERT( !Empty(), "Array is empty" ); return At( m_Size - 1 ); }

		constexpr _Value* Data() { return m_Storage; }
		constexpr const _Value* Data() const { return m_Storage; }

		constexpr operator Span<_Value>() { return Span<_Value>( m_Storage, m_Size ); }
		constexpr operator Span<const _Value>() const { return Span<const _Value>( m_Storage, m_Size ); }
		//============================================================================================

		//============================================================================================
		// Capacity
		constexpr size_t Size() const { return m_Size; }
		constexpr size_t MaxSize() const { return _Size; }
		constexpr bool Empty() const { return m_Size == 0; }
		constexpr bool IsValidIndex( size_t a_Index ) const { return !Empty() && a_Index < m_Size; }
		//============================================================================================

		constexpr void Resize( size_t a_Size )
		{
			TRIDIUM_ARRAY_ASSERT( a_Size <= MaxSize(), "Size is out of bounds" );
			for ( size_t i = m_Size; i < a_Size; ++i )
				std::construct_at( &m_Storage[i] );
			for ( size_t i = a_Size; i < m_Size; ++i )
				m_Storage[i].~_Value();
			m_Size = a_Size;
		}

		constexpr void Fill( const _Value& a_Value )
		{
			for ( size_t i = 0; i < m_Size; ++i )
				m_Storage[i] = a_Value;
		}

		constexpr void Fill( Span<const _Value> a_Data )
		{
			Clear();
			for ( size_t i = 0; i < a_Data.size() || i < MaxSize(); ++i )
				std::construct_at( &m_Storage[i], a_Data[i] );
			m_Size = a_Data.size();
		}

		constexpr void Fill( InitList<_Value> a_InitializerList )
		{
			Clear();
			for ( size_t i = 0; i < a_InitializerList.Size() || i < MaxSize(); ++i )
				std::construct_at( &m_Storage[i], a_InitializerList[i] );
			m_Size = a_InitializerList.Size();
		}

		constexpr void Clear() 
		{
			for ( size_t i = 0; i < m_Size; ++i )
				m_Storage[i].~_Value();
			m_Size = 0;
		}

		constexpr void PushBack( const _Value& a_Value ) 
		{
			TRIDIUM_ARRAY_ASSERT( m_Size < MaxSize(), "Array is full" );
			std::construct_at( &m_Storage[m_Size], a_Value );
			++m_Size;
		}

		constexpr void PopBack() 
		{
			TRIDIUM_ARRAY_ASSERT( !Empty(), "Array is empty");
			m_Storage[--m_Size].~_Value();
		}

		template<typename... _Args>
		constexpr _Value& EmplaceBack( _Args&&... a_Args )
		{
			TRIDIUM_ARRAY_ASSERT( m_Size < MaxSize(), "Array is full" );
			std::construct_at( &m_Storage[m_Size], std::forward<_Args>( a_Args )... );
			return m_Storage[m_Size++];
		}

		template<typename... _Args>
		constexpr _Value& Emplace( Iterator a_Pos, _Args&&... a_Args )
		{
			TRIDIUM_ARRAY_ASSERT( m_Size < MaxSize(), "Array is full" );
			TRIDIUM_ARRAY_ASSERT( a_Pos >= Begin() && a_Pos <= End(), "Position out of bounds" );
			for ( Iterator it = End(); it != a_Pos; --it )
				*it = std::move( *(it - 1) );
			std::construct_at( a_Pos, std::forward<_Args>( a_Args )... );
			++m_Size;
			return *a_Pos;
		}

		constexpr Iterator Insert( Iterator a_Pos, const _Value& a_Value )
		{
			TRIDIUM_ARRAY_ASSERT( m_Size < MaxSize(), "Array is full" );
			TRIDIUM_ARRAY_ASSERT( a_Pos >= Begin() && a_Pos <= End(), "Position out of bounds" );
			for ( Iterator it = End(); it != a_Pos; --it )
				*it = std::move( *(it - 1) );
			std::construct_at( a_Pos, a_Value );
			++m_Size;
			return a_Pos;
		}

		constexpr Iterator Erase( Iterator a_Position )
		{
			TRIDIUM_ARRAY_ASSERT( a_Position >= Begin() && a_Position < End(), "Position out of bounds" );
			for ( Iterator it = a_Position; it != End() - 1; ++it )
				*it = std::move( *(it + 1) );
			m_Size--;
			return a_Position;
		}

		constexpr Iterator Erase( Iterator a_First, Iterator a_Last )
		{
			TRIDIUM_ARRAY_ASSERT( a_First >= Begin() && a_Last <= End(), "Position out of bounds" );
			for ( Iterator it = a_First; it != a_Last; ++it )
				it->~_Value();
			for ( Iterator it = a_Last; it != End(); ++it )
				*(it - (a_Last - a_First)) = std::move( *it );
			m_Size -= (a_Last - a_First);
			return a_First;
		}

		// Iterators
		constexpr Iterator Begin() { return m_Storage; }
		constexpr ConstIterator Begin() const { return m_Storage; }
		constexpr Iterator End() { return m_Storage + m_Size; }
		constexpr ConstIterator End() const { return m_Storage + m_Size; }

		constexpr ReverseIterator RBegin() { return ReverseIterator( End() ); }
		constexpr ConstReverseIterator RBegin() const { return ConstReverseIterator( End() ); }
		constexpr ReverseIterator REnd() { return ReverseIterator( Begin() ); }
		constexpr ConstReverseIterator REnd() const { return ConstReverseIterator( Begin() ); }

	private:
		_Value m_Storage[_Size];
		size_t m_Size;
	};
	//==============================================================================================

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
		bool Empty() const { return m_Data.empty(); }
		bool IsValidIndex( size_t a_Index ) const { return a_Index < m_Data.size(); }
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

	private:
		std::vector<T> m_Data;
	};

	// standard begin/end/rbegin/rend for FixedArray
	template<typename T, size_t _Size>
	constexpr auto begin( const FixedArray<T, _Size>& a_Array ) { return a_Array.Begin(); }
	template<typename T, size_t _Size>
	constexpr auto end( const FixedArray<T, _Size>& a_Array ) { return a_Array.End(); }
	template<typename T, size_t _Size>
	constexpr auto begin( FixedArray<T, _Size>& a_Array ) { return a_Array.Begin(); }
	template<typename T, size_t _Size>
	constexpr auto end( FixedArray<T, _Size>& a_Array ) { return a_Array.End(); }
	template<typename T, size_t _Size>
	constexpr auto rbegin( const FixedArray<T, _Size>& a_Array ) { return a_Array.RBegin(); }
	template<typename T, size_t _Size>
	constexpr auto rend( const FixedArray<T, _Size>& a_Array ) { return a_Array.REnd(); }
	template<typename T, size_t _Size>
	constexpr auto rbegin( FixedArray<T, _Size>& a_Array ) { return a_Array.RBegin(); }
	template<typename T, size_t _Size>
	constexpr auto rend( FixedArray<T, _Size>& a_Array ) { return a_Array.REnd(); }

	// standard begin/end/rbegin/rend for InlineArray
	template<typename T, size_t _Size>
	constexpr auto begin( const InlineArray<T, _Size>& a_Array ) { return a_Array.Begin(); }
	template<typename T, size_t _Size>
	constexpr auto end( const InlineArray<T, _Size>& a_Array ) { return a_Array.End(); }
	template<typename T, size_t _Size>
	constexpr auto begin( InlineArray<T, _Size>& a_Array ) { return a_Array.Begin(); }
	template<typename T, size_t _Size>
	constexpr auto end( InlineArray<T, _Size>& a_Array ) { return a_Array.End(); }
	template<typename T, size_t _Size>
	constexpr auto rbegin( const InlineArray<T, _Size>& a_Array ) { return a_Array.RBegin(); }
	template<typename T, size_t _Size>
	constexpr auto rend( const InlineArray<T, _Size>& a_Array ) { return a_Array.REnd(); }
	template<typename T, size_t _Size>
	constexpr auto rbegin( InlineArray<T, _Size>& a_Array ) { return a_Array.RBegin(); }
	template<typename T, size_t _Size>
	constexpr auto rend( InlineArray<T, _Size>& a_Array ) { return a_Array.REnd(); }

	// standard begin/end/rbegin/rend for Array
	template<typename T>
	constexpr auto begin( const Array<T>& a_Array ) { return a_Array.Begin(); }
	template<typename T>
	constexpr auto end( const Array<T>& a_Array ) { return a_Array.End(); }
	template<typename T>
	constexpr auto begin( Array<T>& a_Array ) { return a_Array.Begin(); }
	template<typename T>
	constexpr auto end( Array<T>& a_Array ) { return a_Array.End(); }
	template<typename T>
	constexpr auto rbegin( const Array<T>& a_Array ) { return a_Array.RBegin(); }
	template<typename T>
	constexpr auto rend( const Array<T>& a_Array ) { return a_Array.REnd(); }
	template<typename T>
	constexpr auto rbegin( Array<T>& a_Array ) { return a_Array.RBegin(); }
	template<typename T>
	constexpr auto rend( Array<T>& a_Array ) { return a_Array.REnd(); }
}