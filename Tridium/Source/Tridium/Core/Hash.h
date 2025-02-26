#pragma once
#include "Types.h"
#include <Tridium/Utils/TypeTraits.h>

namespace Tridium {

	using hash_t = uint32_t;

	namespace HashAlgorithms {
		TRIDIUM_NODISCARD constexpr hash_t fnv1a( const char* a_String, hash_t a_Hash = 2166136261U )
		{
			for ( ; *a_String; ++a_String )
			{
				a_Hash ^= *a_String;
				a_Hash *= 16777619U;
			}
			return a_Hash;
		}

		TRIDIUM_NODISCARD constexpr hash_t fnv1a( const uint8_t* a_Data, size_t a_Length, hash_t a_Hash = 2166136261U )
		{
			for ( size_t i = 0; i < a_Length; ++i )
			{
				a_Hash ^= a_Data[i];
				a_Hash *= 16777619U;
			}
			return a_Hash;
		}
	}

	//========================================
	// Basic Hashed String
	//  A zero-overhead unique identifier.
	//  Used for creating human-readable identifiers in the codebase that are used as a hash at runtime.
	//  WARNING: This does not take ownership of the string. Deleting the string this points to can cause undefined behavior.
	template<typename _Char>
	struct BasicHashedString
	{
		using CharTraits = std::char_traits<_Char>;

	private:
		const _Char* m_String;
		size_t m_Length;
		hash_t m_Hash;

		
		static constexpr hash_t Helper( const _Char* a_String, size_t a_Length )
		{
			hash_t hash = 2166136261U;
			for ( ; a_Length; ++a_String, --a_Length )
			{
				hash ^= *a_String;
				hash *= 16777619U;
			}
			return hash;
		}

	public:

		constexpr BasicHashedString( const _Char* a_String, size_t a_Length )
			: m_String( a_String ), m_Length( a_Length ), m_Hash( Helper( a_String, a_Length ) ) {}
		constexpr BasicHashedString( const _Char* a_String )
			: BasicHashedString( a_String, CharTraits::length( a_String ) ) {}
		constexpr BasicHashedString( const BasicHashedString& a_Other )
			: m_String( a_Other.m_String ), m_Length( a_Other.m_Length ), m_Hash( a_Other.m_Hash ) {}

		constexpr BasicHashedString& operator=( const BasicHashedString& a_Other )
		{
			m_String = a_Other.m_String;
			m_Length = a_Other.m_Length;
			m_Hash = a_Other.m_Hash;
			return *this;
		}

		constexpr hash_t Hash() const noexcept { return m_Hash; }
		constexpr TStringView<_Char> String() const noexcept { return TStringView<_Char>( m_String, m_Length ); }

		constexpr bool operator==( const BasicHashedString& a_Other ) const noexcept { return m_Hash == a_Other.m_Hash; }
		constexpr bool operator!=( const BasicHashedString& a_Other ) const noexcept { return m_Hash != a_Other.m_Hash; }
		constexpr bool operator==( hash_t a_Hash ) const noexcept { return m_Hash == m_Hash; }
		constexpr bool operator!=( hash_t a_Hash ) const noexcept { return m_Hash != m_Hash; }
		constexpr operator hash_t() const noexcept { return m_Hash; }
	};

	using HashedString = BasicHashedString<char>;
	using HashedWString = BasicHashedString<wchar_t>;

	inline constexpr HashedString operator"" _H( const char* a_String, size_t a_Length )
	{
		return HashedString( a_String, a_Length );
	}

	inline constexpr HashedWString operator"" _H( const wchar_t* a_String, size_t a_Length )
	{
		return HashedWString( a_String, a_Length );
	}

	//========================================
	// Hashing Functions
	namespace Hashing {

		// Hashes a contiguous block of data (represented as a byte array).
		// Uses the FNV-1a algorithm.
		// Useful for create a checksum of a block of data.
		// Example:
		//   uint32_t myData[] = { 1, 2, 3, 4, 5 };
		//   Hash( (const uint8_t*)myData, sizeof( myData ) / sizeof( uint32_t ) );
		TRIDIUM_NODISCARD inline constexpr hash_t Hash( const uint8_t* a_Data, size_t a_Length )
		{
			return HashAlgorithms::fnv1a( a_Data, a_Length );
		}

		// Hashes a null-terminated C-style string and returns a hashed string.
		// Uses the FNV-1a algorithm.
		// Example:
		//   HashString( "Hello, World!" );
		TRIDIUM_NODISCARD inline constexpr HashedString HashString( const char* a_String )
		{
			return HashedString( a_String );
		}

		// Hashes a null-terminated C-style wide string and returns a hashed string.
		// Uses the FNV-1a algorithm.
		// Example:
		//   HashString( L"Hello, World!" );
		TRIDIUM_NODISCARD inline constexpr HashedWString HashString( const wchar_t* a_String )
		{
			return HashedWString( a_String );
		}

		// Combines two hashes into a single hash.
		TRIDIUM_NODISCARD inline constexpr hash_t HashCombine( hash_t a_Left, hash_t a_Right )
		{
			return a_Left ^ ( a_Right + 0x9e3779b9 + ( a_Left << 6 ) + ( a_Left >> 2 ) );
		}

		template <typename T>
		TRIDIUM_NODISCARD constexpr hash_t TypeHash()
		{
			return HashString( StrippedTypeName<T>().data() );
		}

	} // namespace Hashing

} // namespace Tridium