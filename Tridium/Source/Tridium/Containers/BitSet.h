#pragma once
#include <bitset>
#include <ostream>
#include <type_traits>
#include <utility>

namespace Tridium {

	template <size_t _Bits>
	using BitSet = std::bitset<_Bits>;

    // Helper class for bitwise flag-like operations on scoped enums.
    //
    // This class provides a way to represent combinations of enum values without
    // directly overloading operators on the enum type itself. This approach
    // avoids ambiguity in the type system and allows the enum type to continue
    // representing a single value, while the BitFlags can hold a combination
    // of enum values.
    //
    // Example usage:
    //
    // enum class MyEnum { FlagA = 1 << 0, FlagB = 1 << 1, FlagC = 1 << 2 };
    //
    // BitFlags<MyEnum> flags = { MyEnum::FlagA, MyEnum::FlagC };
    // flags.Unset(MyEnum::FlagA);
    // if (flags.IsSet(MyEnum::FlagC)) {
    //   // ...
    // }
    //
    // flags |= MyEnum::FlagB;
    // BitFlags<MyEnum> new_flags = ~flags;
    template <typename T>
    class BitFlags 
    {
        using UnderlyingT = std::underlying_type_t<T>;

    public:
        constexpr BitFlags() : m_Flags( Cast<UnderlyingT>( 0 ) ) {}
        constexpr explicit BitFlags( T v ) : m_Flags( ToUnderlying( v ) ) {}
        constexpr BitFlags( std::initializer_list<T> vs ) : BitFlags() 
        {
            for ( T v : vs ) 
            {
                m_Flags |= ToUnderlying( v );
            }
        }

        // Checks if a specific flag is set.
        constexpr bool IsSet( T v ) const 
        {
            return ( m_Flags & ToUnderlying( v ) ) == ToUnderlying( v );
        }

        // Sets a single flag value.
        constexpr void Set( T v ) { m_Flags |= ToUnderlying( v ); }
        // Unsets a single flag value.
        constexpr void Unset( T v ) { m_Flags &= ~ToUnderlying( v ); }
        // Clears all flag values.
        constexpr void Clear() { m_Flags = Cast<UnderlyingT>( 0 ); }

        constexpr operator bool() const 
        {
            return m_Flags != Cast<UnderlyingT>( 0 );
        }

        friend constexpr BitFlags operator|( BitFlags lhs, T rhs ) 
        {
            return BitFlags( lhs.m_Flags | ToUnderlying( rhs ) );
        }

        friend constexpr BitFlags operator|( BitFlags lhs, BitFlags rhs ) 
        {
            return BitFlags( lhs.m_Flags | rhs.m_Flags );
        }

        friend constexpr BitFlags operator&( BitFlags lhs, T rhs ) 
        {
            return BitFlags( lhs.m_Flags & ToUnderlying( rhs ) );
        }

        friend constexpr BitFlags operator&( BitFlags lhs, BitFlags rhs ) 
        {
            return BitFlags( lhs.m_Flags & rhs.m_Flags );
        }

        friend constexpr BitFlags operator^( BitFlags lhs, T rhs ) 
        {
            return BitFlags( lhs.m_Flags ^ ToUnderlying( rhs ) );
        }

        friend constexpr BitFlags operator^( BitFlags lhs, BitFlags rhs ) 
        {
            return BitFlags( lhs.m_Flags ^ rhs.m_Flags );
        }

        friend constexpr BitFlags& operator|=( BitFlags& lhs, T rhs ) {
            lhs.m_Flags |= ToUnderlying( rhs );
            return lhs;
        }

        friend constexpr BitFlags& operator|=( BitFlags& lhs, BitFlags rhs ) 
        {
            lhs.m_Flags |= rhs.m_Flags;
            return lhs;
        }

        friend constexpr BitFlags& operator&=( BitFlags& lhs, T rhs ) 
        {
            lhs.m_Flags &= ToUnderlying( rhs );
            return lhs;
        }

        friend constexpr BitFlags& operator&=( BitFlags& lhs, BitFlags rhs ) 
        {
            lhs.m_Flags &= rhs.m_Flags;
            return lhs;
        }

        friend constexpr BitFlags& operator^=( BitFlags& lhs, T rhs ) 
        {
            lhs.m_Flags ^= ToUnderlying( rhs );
            return lhs;
        }

        friend constexpr BitFlags& operator^=( BitFlags& lhs, BitFlags rhs ) 
        {
            lhs.m_Flags ^= rhs.m_Flags;
            return lhs;
        }

        friend constexpr BitFlags operator~( const BitFlags& bf ) 
        {
            return BitFlags( ~bf.m_Flags );
        }

        friend constexpr bool operator==( const BitFlags& lhs, const BitFlags& rhs ) 
        {
            return lhs.m_Flags == rhs.m_Flags;
        }

        friend constexpr bool operator!=( const BitFlags& lhs, const BitFlags& rhs ) 
        {
            return lhs.m_Flags != rhs.m_Flags;
        }

        // Stream output operator for debugging.
        friend std::ostream& operator<<( std::ostream& os, const BitFlags& bf ) 
        {
            // Write out a bitset representation.
            os << std::bitset<sizeof( UnderlyingT ) * 8>( bf.m_Flags );
            return os;
        }

        // Construct BitFlags from raw values.
        static constexpr BitFlags FromRaw( UnderlyingT flags ) 
        {
            return BitFlags( flags );
        }

        // Retrieve the raw underlying flags.
        constexpr UnderlyingT ToRaw() const { return m_Flags; }

    private:
        constexpr explicit BitFlags( UnderlyingT flags ) : m_Flags( flags ) {}
        static constexpr UnderlyingT ToUnderlying( T v ) { return Cast<UnderlyingT>( v ); }

        UnderlyingT m_Flags;
    };

}  // namespace Tridium