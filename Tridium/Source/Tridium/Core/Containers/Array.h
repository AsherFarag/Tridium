#pragma once
#include <Tridium/Core/Assert.h>
#include <array>
#include <vector>

namespace Tridium {

#ifndef TE_ENABLE_ARRAY_BOUNDS_CHECK
	#define TE_ENABLE_ARRAY_BOUNDS_CHECK 1
#endif

	// Fixed size array
	// An array of contiguous memory with a fixed size.
	// STD equivalent: std::array
	template<typename T, size_t _Size>
	class FixedArray
	{
	public:
		using Iterator = typename std::array<T, _Size>::iterator;
		using ConstIterator = typename std::array<T, _Size>::const_iterator;
		using ReverseIterator = typename std::array<T, _Size>::reverse_iterator;
		using ConstReverseIterator = typename std::array<T, _Size>::const_reverse_iterator;

		FixedArray() = default;
		FixedArray( const FixedArray& a_Other ) = default;
		FixedArray( FixedArray&& a_Other ) = default;
		FixedArray( std::initializer_list<T> a_InitializerList ) : m_Data( a_InitializerList ) {}
		FixedArray& operator=( const FixedArray& a_Other ) = default;
		FixedArray& operator=( FixedArray&& a_Other ) = default;

		T& operator[]( size_t a_Index )
		{
		#if TE_ENABLE_ARRAY_BOUNDS_CHECK
			CORE_ENSURE_LOG( IsValidIndex( a_Index ), "Index out of bounds" );
		#endif
			return m_Data[a_Index];
		}

		const T& operator[]( size_t a_Index ) const
		{
		#if TE_ENABLE_ARRAY_BOUNDS_CHECK
			CORE_ENSURE_LOG( IsValidIndex( a_Index ), "Index out of bounds" );
		#endif
			return m_Data[a_Index];
		}

		T& At( size_t a_Index )
		{
		#if TE_ENABLE_ARRAY_BOUNDS_CHECK
			CORE_ENSURE_LOG( IsValidIndex( a_Index ), "Index out of bounds" );
		#endif
			return m_Data[a_Index];
		}

		const T& At( size_t a_Index ) const
		{
		#if TE_ENABLE_ARRAY_BOUNDS_CHECK
			CORE_ENSURE_LOG( IsValidIndex( a_Index ), "Index out of bounds" );
		#endif
			return m_Data[a_Index];
		}

		T& Front() { return m_Data.front(); }
		const T& Front() const { return m_Data.front(); }

		T& Back() { return m_Data.back(); }
		const T& Back() const { return m_Data.back(); }

		T* Data() { return m_Data.data(); }
		const T* Data() const { return m_Data.data(); }

		constexpr size_t Size() const { return _Size; }
		constexpr size_t MaxSize() const { return _Size; }
		constexpr bool IsEmpty() const { return false; }
		constexpr bool IsValidIndex( size_t a_Index ) const { return ( ( a_Index >= 0 ) && ( a_Index < _Size ) ); }

		void Fill( const T& a_Value ) { m_Data.fill( a_Value ); }
		void Swap( FixedArray& a_Other ) { m_Data.swap( a_Other.m_Data ); }

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
		std::array<T, _Size> m_Data;
	};
	// =================================================================================================



	// Dynamic array
	// An array of contiguous memory with dynamic size.
	// STD equivalent: std::vector
	template<typename T>
	class Array
	{
	public:
		using Iterator = typename std::vector<T>::iterator;
		using ConstIterator = typename std::vector<T>::const_iterator;
		using ReverseIterator = typename std::vector<T>::reverse_iterator;
		using ConstReverseIterator = typename std::vector<T>::const_reverse_iterator;

		Array() = default;
		Array( const Array& a_Other ) = default;
		Array( Array&& a_Other ) = default;
		Array( size_t a_Size ) : m_Data( a_Size ) {}
		Array( std::initializer_list<T> a_InitializerList ) : m_Data( a_InitializerList ) {}
		Array& operator=( const Array& a_Other ) = default;
		Array& operator=( Array&& a_Other ) = default;

		T& operator[]( size_t a_Index )
		{
		#if TE_ENABLE_ARRAY_BOUNDS_CHECK
			CORE_ENSURE_LOG( IsValidIndex( a_Index ), "Index out of bounds" );
		#endif
			return m_Data[a_Index];
		}

		const T& operator[]( size_t a_Index ) const
		{
		#if TE_ENABLE_ARRAY_BOUNDS_CHECK
			CORE_ENSURE_LOG( IsValidIndex( a_Index ), "Index out of bounds" );
		#endif
			return m_Data[a_Index];
		}

		T& At( size_t a_Index )
		{
		#if TE_ENABLE_ARRAY_BOUNDS_CHECK
			CORE_ENSURE_LOG( IsValidIndex( a_Index ), "Index out of bounds" );
		#endif
			return m_Data[a_Index];
		}

		const T& At( size_t a_Index ) const
		{
		#if TE_ENABLE_ARRAY_BOUNDS_CHECK
			CORE_ENSURE_LOG( IsValidIndex( a_Index ), "Index out of bounds" );
		#endif
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

		Iterator Insert( Iterator a_Position, const T& a_Value ) { return m_Data.insert( a_Position, a_Value ); }
		Iterator Erase( Iterator a_Position ) { return m_Data.erase( a_Position ); }
		Iterator Erase( Iterator a_First, Iterator a_Last ) { return m_Data.erase( a_First, a_Last ); }

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