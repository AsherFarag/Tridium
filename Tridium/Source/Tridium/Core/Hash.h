#pragma once
#include "Types.h"
#include "Cast.h"
#include <Tridium/Utils/TypeTraits.h>
#include <ctype.h>

namespace Tridium {

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
		BasicStringView<_Char> m_String;
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
		constexpr BasicHashedString() noexcept : m_String(), m_Hash( 0 ) {}
		constexpr BasicHashedString( BasicStringView<_Char> a_String )
			: m_String( a_String ), m_Hash( Helper( a_String.data(), a_String.size() ) ) {}
		constexpr BasicHashedString( const BasicHashedString& a_Other )
			: m_String( a_Other.m_String ), m_Hash( a_Other.m_Hash ) {}

		constexpr BasicHashedString& operator=( BasicStringView<_Char> a_String )
		{
			m_String = a_String;
			m_Hash = Helper( a_String.data(), a_String.size() );
			return *this;
		}

		constexpr BasicHashedString& operator=( const BasicHashedString& a_Other )
		{
			m_String = a_Other.m_String;
			m_Hash = a_Other.m_Hash;
			return *this;
		}

		constexpr hash_t Hash() const noexcept { return m_Hash; }
		constexpr BasicStringView<_Char> String() const noexcept { return m_String; }

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
		return HashedString( StringView( a_String, a_Length ) );
	}

	inline constexpr HashedWString operator"" _H( const wchar_t* a_String, size_t a_Length )
	{
		return HashedWString( WStringView( a_String, a_Length ) );
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
		TRIDIUM_NODISCARD inline constexpr HashedString HashString( StringView a_String )
		{
			return HashedString( a_String );
		}

		// Hashes a null-terminated C-style wide string and returns a hashed string.
		// Uses the FNV-1a algorithm.
		// Example:
		//   HashString( L"Hello, World!" );
		TRIDIUM_NODISCARD inline constexpr HashedWString HashString( WStringView a_String )
		{
			return HashedWString( a_String );
		}

		// Combines two hashes into a single hash.
		TRIDIUM_NODISCARD inline constexpr hash_t HashCombine( hash_t a_Left, hash_t a_Right )
		{
			return a_Left ^ ( a_Right + 0x9e3779b9 + ( a_Left << 6 ) + ( a_Left >> 2 ) );
		}

		template<typename T>
		TRIDIUM_NODISCARD constexpr hash64_t HashCombine( hash64_t a_Seed, const T& a_Value )
		{
			a_Seed ^= std::hash<T>{}( a_Value ) + 0x9e3779b9 + ( a_Seed << 6 ) + ( a_Seed >> 2 );
			return a_Seed;
		}

		template <typename T>
		TRIDIUM_NODISCARD constexpr HashedString TypeHash()
		{
			return HashString( GetTypeName<T>() );
		}

		namespace Util {

			template<typename _Hashable, typename _Func>
			[[nodiscard]] inline constexpr hash_t HashAs( const _Hashable& a_Hashable, _Func&& a_HashFunc )
			{
				hash_t hash = 2166136261U;
				for ( const auto& elem : a_Hashable )
				{
					hash ^= Cast<hash_t>( a_HashFunc( elem ) );
					hash *= 16777619U;
				}
				return hash;
			}

			// Hashes a string but converts each character to lower case while hashing.
			template<typename _Elem>
			[[nodiscard]] inline constexpr hash_t HashAsLowerCase( BasicStringView<_Elem> a_String )
			{
				return HashAs( a_String, []( _Elem c ) { return std::tolower( c ); } );
			}

			// Hashes a string but converts each character to upper case while hashing.
			template<typename _Elem>
			[[nodiscard]] inline constexpr hash_t HashAsUpperCase( BasicStringView<_Elem> a_String )
			{
				return HashAs( a_String, []( _Elem c ) { return std::toupper( c ); } );
			}

			// Hashes a string but skips whitespace and converts each character to lower case while hashing.
			template<typename _Elem>
			[[nodiscard]] inline constexpr hash_t HashAsLowerCaseAndTrimmed( BasicStringView<_Elem> a_String )
			{
				hash_t hash = 2166136261U;
				for ( const auto& c : a_String )
				{
					if ( !std::isspace( c ) )
					{
						hash ^= std::tolower( c );
						hash *= 16777619U;
					}
				}
				return hash;
			}

			// Hashes a string but skips whitespace and converts each character to upper case while hashing.
			template<typename _Elem>
			[[nodiscard]] inline constexpr hash_t HashAsUpperCaseAndTrimmed( BasicStringView<_Elem> a_String )
			{
				hash_t hash = 2166136261U;
				for ( const auto& c : a_String )
				{
					if ( !std::isspace( c ) )
					{
						hash ^= std::toupper( c );
						hash *= 16777619U;
					}
				}
				return hash;
			}
		}

	} // namespace Hashing

} // namespace Tridium

namespace std {

	template<typename _Char>
	struct hash<Tridium::BasicHashedString<_Char>>
	{
		[[nodiscard]] constexpr size_t operator()( const Tridium::BasicHashedString<_Char>& a_HashedString ) const noexcept
		{
			return hash<decltype( a_HashedString.Hash() )>{}( a_HashedString.Hash() );
		}
	};

}