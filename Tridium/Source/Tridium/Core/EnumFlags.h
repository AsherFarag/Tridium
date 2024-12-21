#pragma once

namespace Tridium {

	template<typename _Enum>
	struct EnumFlags
	{
		using EnumType = std::underlying_type_t<_Enum>;
		EnumType Value;

		EnumFlags() = default;
		EnumFlags( _Enum a_Value )
			: Value( static_cast<EnumType>( a_Value ) )
		{
		}

		EnumFlags( EnumType a_Value )
			: Value( a_Value )
		{
		}

		EnumFlags( const EnumFlags& a_Other )
			: Value( a_Other.Value )
		{
		}

		template<typename T>
		EnumFlags( T a_Value )
			: Value( static_cast<EnumType>( a_Value ) )
		{
		}

		template<typename T>
		EnumFlags operator=( T a_Value )
		{
			Value = static_cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator=( EnumFlags<T> a_Value )
		{
			Value = static_cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator|( T a_Value )
		{
			Value |= static_cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator|( EnumFlags<T> a_Value )
		{
			Value |= static_cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator&( T a_Value )
		{
			Value &= static_cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator&( EnumFlags<T> a_Value )
		{
			Value &= static_cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator^( T a_Value )
		{
			Value ^= static_cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator^( EnumFlags<T> a_Value )
		{
			Value ^= static_cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator~()
		{
			Value = ~Value;
			return *this;
		}

		template<typename T>
		EnumFlags operator|=( T a_Value )
		{
			Value |= static_cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator&=( T a_Value )
		{
			Value &= static_cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		bool operator==( T a_Value ) const
		{
			return Value == static_cast<EnumType>( a_Value );
		}

		template<typename T>
		bool operator==( EnumFlags<T> a_Value ) const
		{
			return Value == static_cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		bool operator!=( T a_Value ) const
		{
			return Value != static_cast<EnumType>( a_Value );
		}

		template<typename T>
		bool operator!=( EnumFlags<T> a_Value ) const
		{
			return Value != static_cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		bool operator<( T a_Value ) const
		{
			return Value < static_cast<EnumType>( a_Value );
		}

		template<typename T>
		bool operator<( EnumFlags<T> a_Value ) const
		{
			return Value < static_cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		bool operator>( T a_Value ) const
		{
			return Value > static_cast<EnumType>( a_Value );
		}

		template<typename T>
		bool operator>( EnumFlags<T> a_Value ) const
		{
			return Value > static_cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		bool operator<=( T a_Value ) const
		{
			return Value <= static_cast<EnumType>( a_Value );
		}

		template<typename T>
		bool operator<=( EnumFlags<T> a_Value ) const
		{
			return Value <= static_cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		bool operator>=( T a_Value ) const
		{
			return Value >= static_cast<EnumType>( a_Value );
		}

		template<typename T>
		bool operator>=( EnumFlags<T> a_Value ) const
		{
			return Value >= static_cast<EnumType>( a_Value.Value );
		}

		template<typename T>
		EnumFlags operator<<( T a_Value )
		{
			Value <<= static_cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator<<( EnumFlags<T> a_Value )
		{
			Value <<= static_cast<EnumType>( a_Value.Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator>>( T a_Value )
		{
			Value >>= static_cast<EnumType>( a_Value );
			return *this;
		}

		template<typename T>
		EnumFlags operator>>( EnumFlags<T> a_Value )
		{
			Value >>= static_cast<EnumType>( a_Value.Value );
			return *this;
		}

		operator EnumType() const
		{
			return Value;
		}

		operator _Enum() const
		{
			return static_cast<_Enum>( Value );
		}

		operator bool() const
		{
			return Value != 0;
		}

		EnumFlags operator=( const EnumFlags& a_Other )
		{
			Value = a_Other.Value;
			return *this;
		}

		EnumFlags operator=( _Enum a_Value )
		{
			Value = static_cast<EnumType>( a_Value );
			return *this;
		}

		EnumFlags operator=( EnumType a_Value )
		{
			Value = a_Value;
			return *this;
		}

		template<typename T>
		bool HasFlag( T Flag ) const
		{
			EnumType flag = static_cast<EnumType>( Flag );
			return ( Value & static_cast<EnumType>( Flag ) ) == flag;
		}

		template<typename T>
		EnumFlags AddFlag( T Flag )
		{
			Value |= static_cast<EnumType>( Flag );
			return *this;
		}

		template<typename T>
		EnumFlags RemoveFlag( T Flag )
		{
			Value &= ~static_cast<EnumType>( Flag );
			return *this;
		}

		template<typename T>
		EnumFlags ToggleFlag( T Flag )
		{
			Value ^= static_cast<EnumType>( Flag );
			return *this;
		}

		template<typename T>
		EnumFlags SetFlag( T Flag, bool a_Set )
		{
			if ( a_Set )
			{
				Value |= static_cast<EnumType>( Flag );
			}
			else
			{
				Value &= ~static_cast<EnumType>( Flag );
			}
			return *this;
		}
	};

}