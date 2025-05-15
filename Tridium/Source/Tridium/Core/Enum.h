#pragma once
#include <type_traits>

#define ENUM_ENABLE_BITMASK_OPERATORS(EnumType) \
    inline constexpr EnumType operator|(EnumType lhs, EnumType rhs) { \
        return Cast<EnumType>( \
            Cast<std::underlying_type_t<EnumType>>(lhs) | \
            Cast<std::underlying_type_t<EnumType>>(rhs)); \
    } \
    inline constexpr EnumType operator&(EnumType lhs, EnumType rhs) { \
        return Cast<EnumType>( \
            Cast<std::underlying_type_t<EnumType>>(lhs) & \
            Cast<std::underlying_type_t<EnumType>>(rhs)); \
    } \
    inline constexpr EnumType operator^(EnumType lhs, EnumType rhs) { \
        return Cast<EnumType>( \
            Cast<std::underlying_type_t<EnumType>>(lhs) ^ \
            Cast<std::underlying_type_t<EnumType>>(rhs)); \
    } \
    inline constexpr EnumType operator~(EnumType val) { \
        return Cast<EnumType>( \
            ~Cast<std::underlying_type_t<EnumType>>(val)); \
    } \
    inline EnumType& operator|=(EnumType& lhs, EnumType rhs) { \
        return lhs = (lhs | rhs); \
    } \
    inline EnumType& operator&=(EnumType& lhs, EnumType rhs) { \
        return lhs = (lhs & rhs); \
    } \
    inline EnumType& operator^=(EnumType& lhs, EnumType rhs) { \
        return lhs = (lhs ^ rhs); \
    }

namespace Tridium {

	template<typename _Enum>
	struct EnumFlags
	{
		using EnumType = std::underlying_type_t<_Enum>;
		EnumType Value;

		constexpr EnumFlags() = default;
		constexpr EnumFlags( _Enum a_Value )
			: Value( Cast<EnumType>( a_Value ) )
		{
		}

		constexpr EnumFlags( EnumType a_Value )
			: Value( a_Value )
		{
		}

		constexpr EnumFlags( const EnumFlags& a_Other )
			: Value( a_Other.Value )
		{
		}

		template<typename T>
		constexpr EnumFlags( T a_Value )
			: Value( Cast<EnumType>( a_Value ) )
		{
		}

		template<typename T>
		constexpr EnumFlags operator=( T a_Value )
		{
			Value = Cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator=( EnumFlags<T> a_Value )
		{
			Value = Cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator|( T a_Value )
		{
			Value |= Cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator|( EnumFlags<T> a_Value )
		{
			Value |= Cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator&( T a_Value )
		{
			Value &= Cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator&( EnumFlags<T> a_Value )
		{
			Value &= Cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator^( T a_Value )
		{
			Value ^= Cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator^( EnumFlags<T> a_Value )
		{
			Value ^= Cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator~()
		{
			Value = ~Value;
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator|=( T a_Value )
		{
			Value |= Cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator&=( T a_Value )
		{
			Value &= Cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		constexpr bool operator==( T a_Value ) const
		{
			return Value == Cast<EnumType>( a_Value );
		}

		template<typename T>
		constexpr bool operator==( EnumFlags<T> a_Value ) const
		{
			return Value == Cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		constexpr bool operator!=( T a_Value ) const
		{
			return Value != Cast<EnumType>( a_Value );
		}

		template<typename T>
		constexpr bool operator!=( EnumFlags<T> a_Value ) const
		{
			return Value != Cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		constexpr bool operator<( T a_Value ) const
		{
			return Value < Cast<EnumType>( a_Value );
		}

		template<typename T>
		constexpr bool operator<( EnumFlags<T> a_Value ) const
		{
			return Value < Cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		constexpr bool operator>( T a_Value ) const
		{
			return Value > Cast<EnumType>( a_Value );
		}

		template<typename T>
		constexpr bool operator>( EnumFlags<T> a_Value ) const
		{
			return Value > Cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		constexpr bool operator<=( T a_Value ) const
		{
			return Value <= Cast<EnumType>( a_Value );
		}

		template<typename T>
		constexpr bool operator<=( EnumFlags<T> a_Value ) const
		{
			return Value <= Cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		constexpr bool operator>=( T a_Value ) const
		{
			return Value >= Cast<EnumType>( a_Value );
		}

		template<typename T>
		constexpr bool operator>=( EnumFlags<T> a_Value ) const
		{
			return Value >= Cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		constexpr EnumFlags operator<<( T a_Value )
		{
			Value <<= Cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator<<( EnumFlags<T> a_Value )
		{
			Value <<= Cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator>>( T a_Value )
		{
			Value >>= Cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags operator>>( EnumFlags<T> a_Value )
		{
			Value >>= Cast<EnumType>( a_Value.Value );
			return *this;
		}

		constexpr operator EnumType() const
		{
			return Value;
		}

		constexpr operator _Enum() const
		{
			return Cast<_Enum>( Value );
		}

		constexpr operator bool() const
		{
			return Value != 0;
		}

		constexpr EnumFlags operator=( const EnumFlags& a_Other )
		{
			Value = a_Other.Value;
			return *this;
		}

		constexpr EnumFlags operator=( _Enum a_Value )
		{
			Value = Cast<EnumType>( a_Value );
			return *this;
		}

		constexpr EnumFlags operator=( EnumType a_Value )
		{
			Value = a_Value;
			return *this;
		}

		template<typename T>
		constexpr bool HasFlag( T Flag ) const
		{
			EnumType flag = Cast<EnumType>( Flag );
			return ( Value & Cast<EnumType>( Flag ) ) == flag;
		}

		template<typename T>
		constexpr EnumFlags AddFlag( T Flag )
		{
			Value |= Cast<EnumType>( Flag );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags RemoveFlag( T Flag )
		{
			Value &= ~Cast<EnumType>( Flag );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags ToggleFlag( T Flag )
		{
			Value ^= Cast<EnumType>( Flag );
			return *this;
		}

		template<typename T>
		constexpr EnumFlags SetFlag( T Flag, bool a_Set )
		{
			if ( a_Set )
			{
				Value |= Cast<EnumType>( Flag );
			}
			else
			{
				Value &= ~Cast<EnumType>( Flag );
			}
			return *this;
		}
	};

}