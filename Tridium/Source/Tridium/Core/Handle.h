#pragma once
#include <Tridium/Utils/TypeTraits.h>
#include <Tridium/Utils/Concepts.h>

namespace Tridium {

	//=============================================================================================
	// Handle: A simple handle wrapper around an integer type.
	// Null value defaults to -1/max value of the type, unless specified otherwise.
	//=============================================================================================
	template<Concepts::Integral _ValueType, _ValueType _NullValue = _ValueType( -1 )>
	struct Handle
	{
		//=========================================================================================
		using ValueType = _ValueType;
		static constexpr ValueType NullValue = _NullValue;

		//=========================================================================================
		constexpr Handle() = default;
		constexpr Handle( ValueType a_Value ) : m_Value( a_Value ) {}
		constexpr operator bool() const { return Valid(); }
		constexpr operator ValueType() const { return m_Value; }
		constexpr std::strong_ordering operator<=>( const Handle& a_Other ) const = default;

		//=========================================================================================
		[[nodiscard]] constexpr bool Valid() const { return m_Value != NullValue; }
		[[nodiscard]] constexpr ValueType Get() const { return m_Value; }

	private:

		//=========================================================================================
		ValueType m_Value = NullValue;

	};

	template<Concepts::Integral _ValueType, _ValueType _NullValue>
	struct Hash<Handle<_ValueType, _NullValue>>
	{
		size_t operator()( const Handle<_ValueType, _NullValue>& a_Handle ) const
		{
			return Hash<_ValueType>()( a_Handle.Get() );
		}
	};

	template<Concepts::Integral _ValueType, _ValueType _NullValue>
	struct UnderlyingType<Handle<_ValueType, _NullValue>>
	{
		using Type = _ValueType;
	};

} // namespace Tridium