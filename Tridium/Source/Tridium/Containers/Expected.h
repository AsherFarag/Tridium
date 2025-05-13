#pragma once
#include <type_traits>
#include <utility>
#include <Tridium/Core/Assert.h>

namespace Tridium {

	//TODO( "Probably make Expected use a Variant as it is safer and I think I forgot some edge cases" );

#define EXPECTED_ASSERT(_Condition, _Message) \
		if constexpr ( std::is_constant_evaluated() ) \
		{ \
			if ( !(_Condition) ) \
				throw "Assertion failed: " _Message; \
		} \
		else \
		{ \
			ENSURE( _Condition, _Message ); \
		}

	template<typename _Error>
	class Unexpected
	{
	public:
		using ErrorType = _Error;

		constexpr Unexpected( const _Error& a_Error )
			: m_Error( a_Error ) {}
		constexpr Unexpected( _Error&& a_Error ) noexcept
			: m_Error( std::move( a_Error ) ) {}

		template<typename _UError>
		constexpr Unexpected( const Unexpected<_UError>& a_Other )
			: m_Error( a_Other.m_Error ) {}

		template<typename _UError>
        constexpr Unexpected( Unexpected<_UError>&& a_Other ) noexcept
            : m_Error( std::move( a_Other.m_Error ) ) {}

		constexpr Unexpected& operator=( const Unexpected& a_Other )
		{
			if ( this != &a_Other )
			{
				m_Error = a_Other.m_Error;
			}
			return *this;
		}

		constexpr Unexpected& operator=( Unexpected && a_Other ) noexcept
		{
			if ( this != &a_Other )
			{
				m_Error = std::move( a_Other.m_Error );
			}
			return *this;
		}

		[[nodiscard]] constexpr _Error& Error() { return m_Error; }
		[[nodiscard]] constexpr const _Error& Error() const { return m_Error; }

		template<typename _UError>
		[[nodiscard]] constexpr bool operator==( const Unexpected<_UError>& a_Other ) const
		{
			return m_Error == a_Other.Error();
		}

		template<typename _UError>
		[[nodiscard]] constexpr bool operator!=( const Unexpected<_UError>& a_Other ) const
		{
			return m_Error != a_Other.Error();
		}

    private:
		_Error m_Error;

		template<typename _UError>
		friend class Unexpected;

		template<typename _Value, typename _UError>
		friend class Expected;
	};

	template<typename _Value, typename _Error>
	class Expected
	{
	public:
		using ValueType = _Value;
		using ErrorType = _Error;

		constexpr Expected( const _Value& a_Value )
			: m_Value( a_Value ), m_HasValue( true )
		{}

		constexpr Expected( _Value&& a_Value ) noexcept
			: m_Value( std::move( a_Value ) ), m_HasValue( true )
		{}

		constexpr Expected( const _Error& a_Error )
			: m_Error( a_Error ), m_HasValue( false )
		{}

		constexpr Expected( _Error&& a_Error ) noexcept
			: m_Error( std::move( a_Error ) ), m_HasValue( false )
		{}

		constexpr Expected( const Expected& a_Other )
			: m_HasValue( a_Other.m_HasValue )
		{
			if ( m_HasValue )
			{
				new (&m_Value) _Value( a_Other.m_Value );
			}
			else
			{
				new (&m_Error) _Error( a_Other.m_Error );
			}
		}
		constexpr Expected( Expected&& a_Other ) noexcept
			: m_HasValue( a_Other.m_HasValue )
		{
			if ( m_HasValue )
			{
				new (&m_Value) _Value( std::move( a_Other.m_Value ) );
			}
			else
			{
				new (&m_Error) _Error( std::move( a_Other.m_Error ) );
			}
		}

		template<typename _UError>
		constexpr Expected( const Unexpected<_UError>& a_Error ) noexcept
			: m_Error( a_Error.m_Error ), m_HasValue( false )
		{}

		template<typename _UError>
		constexpr Expected( Unexpected<_UError>&& a_Error ) noexcept
			: m_Error( std::move( a_Error.m_Error ) ), m_HasValue( false )
		{}

		template <typename... _Args>
		constexpr Expected( std::in_place_t, _Args&&... a_Args )
			: m_Value( std::forward<_Args>( a_Args )... ), m_HasValue( true )
		{}

		constexpr Expected& operator=( const Expected& a_Other )
		{
			if ( this != &a_Other )
			{
				if ( m_HasValue )
				{
					m_Value.~_Value();
				}
				else
				{
					m_Error.~_Error();
				}
				m_HasValue = a_Other.m_HasValue;
				if ( m_HasValue )
				{
					new (&m_Value) _Value( a_Other.m_Value );
				}
				else
				{
					new (&m_Error) _Error( a_Other.m_Error );
				}
			}
			return *this;
		}

		constexpr Expected& operator=( Expected&& a_Other ) noexcept
		{
			if ( this != &a_Other )
			{
				if ( m_HasValue )
				{
					m_Value.~_Value();
				}
				else
				{
					m_Error.~_Error();
				}
				m_HasValue = a_Other.m_HasValue;
				if ( m_HasValue )
				{
					new (&m_Value) _Value( std::move( a_Other.m_Value ) );
				}
				else
				{
					new (&m_Error) _Error( std::move( a_Other.m_Error ) );
				}
			}
			return *this;
		}

		constexpr ~Expected()
		{
			if ( m_HasValue )
			{
				m_Value.~_Value();
			}
			else
			{
				m_Error.~_Error();
			}
		}

		[[nodiscard]] constexpr bool IsError() const noexcept { return !m_HasValue; }
		[[nodiscard]] constexpr bool HasValue() const noexcept { return m_HasValue; }
		[[nodiscard]] constexpr _Value& Value() { EXPECTED_ASSERT( m_HasValue, "Attempting to access value when there is none" ); return m_Value; }
		[[nodiscard]] constexpr const _Value& Value() const { EXPECTED_ASSERT( m_HasValue, "Attempting to access value when there is none" ); return m_Value; }
		[[nodiscard]] constexpr _Error& Error() { EXPECTED_ASSERT( !m_HasValue, "Attempting to access error when there is a value" ); return m_Error; }
		[[nodiscard]] constexpr const _Error& Error() const { EXPECTED_ASSERT( !m_HasValue, "Attempting to access error when there is a value" ); return m_Error; }

		[[nodiscard]] constexpr _Value& operator*() { return Value(); }
		[[nodiscard]] constexpr const _Value& operator*() const { return Value(); }
		[[nodiscard]] constexpr _Value* operator->() { return &Value(); }
		[[nodiscard]] constexpr const _Value* operator->() const { return &Value(); }

		[[nodiscard]] constexpr explicit operator bool() const { return m_HasValue; }
		[[nodiscard]] constexpr _Value& GetValueOr( _Value& a_Default ) { return m_HasValue ? m_Value : a_Default; }
		[[nodiscard]] constexpr const _Value& GetValueOr( _Value& a_Default ) const { return m_HasValue ? m_Value : a_Default; }

	private:
		union
		{
			_Value m_Value;
			_Error m_Error;
		};
		bool m_HasValue;
	};

} // namespace Tridium