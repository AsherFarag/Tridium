#pragma once
#include <type_traits>
#include <utility>
#include <initializer_list>
#include <Tridium/Core/Assert.h>
#include "Optional.h"

namespace Tridium {

	//TODO( "Probably make Expected use a variant or something

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

	template <class _Error>
	class Unexpected {
	public:
		using ErrorType = _Error;
		constexpr Unexpected( Unexpected const& ) = default;
		constexpr Unexpected( Unexpected&& ) = default;  // NOLINT
		constexpr auto operator=( Unexpected const& ) -> Unexpected & = default;
		constexpr auto operator=( Unexpected&& ) -> Unexpected & = default;  // NOLINT
		~Unexpected() = default;

		template <class... _Args>
			requires std::constructible_from<_Error, _Args...>
		constexpr explicit Unexpected( std::in_place_t, _Args&&... a_Args )
			: m_Error( std::forward<_Args>( a_Args )... )
		{}

		template <class U, class... _Args>
			requires std::constructible_from< _Error, std::initializer_list<U>&, _Args... > 
		constexpr explicit Unexpected( 
			std::in_place_t,
			std::initializer_list<U> a_InitList,
			_Args&&... a_Args )
			: m_Error( a_InitList, std::forward<_Args>( a_Args )... )
		{}

		template <class _UError = _Error>
			requires (!std::same_as<std::remove_cvref_t<_UError>, Unexpected>)
			&& (!std::same_as<std::remove_cvref_t<_UError>, std::in_place_t>)
			&& std::constructible_from<_Error, _UError>
		constexpr explicit Unexpected( _UError&& a_Error )
			: m_Error( std::forward<_UError>( a_Error ) )
		{}

		constexpr _Error const&  Error() const& noexcept  { return m_Error; }
		constexpr _Error&        Error() & noexcept { return m_Error; }
		constexpr _Error const&& Error() const&& noexcept { return std::move( m_Error ); }
		constexpr _Error&&       Error() && noexcept { return std::move( m_Error ); }

		constexpr void Swap( Unexpected& other ) noexcept(std::is_nothrow_swappable_v<_Error>) requires(std::is_swappable_v<_Error>)
		{
			std::swap( m_Error, other.m_Error );
		}

		template <class _UError>
			requires(requires(_Error const& x, _UError const& y) 
			{
				{ x == y } -> std::convertible_to<bool>; 
			})
		friend constexpr bool operator==( Unexpected const& a_A, Unexpected<_UError> const& a_B )
		{
			return a_A.Error() == a_B.Error();
		}

		friend constexpr void swap( Unexpected& a_X, Unexpected& a_Y ) noexcept(noexcept(a_X.Swap( a_Y )))
			requires(std::is_swappable_v<_Error>) 
		{
			a_X.Swap( a_Y );
		}

	private: 
		_Error m_Error;
	};

	template <class _Error>
	Unexpected( _Error ) -> Unexpected<_Error>;

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
			: m_Error( a_Error.Error() ), m_HasValue( false )
		{}

		template<typename _UError>
		constexpr Expected( Unexpected<_UError>&& a_Error ) noexcept
			: m_Error( std::move( std::move( a_Error ).Error() ) ), m_HasValue( false )
		{}

		template <typename... _Args>
		constexpr Expected( std::in_place_t, _Args&&... a_Args )
			: m_Value( std::forward<_Args>( a_Args )... ), m_HasValue( true )
		{}

		constexpr Expected& operator=( const Expected& a_Other )
		{
			if ( this != &a_Other )
			{
				Clear();
				m_HasValue = a_Other.m_HasValue;
				if ( m_HasValue )
				{
					std::construct_at( std::addressof( m_Value ), a_Other.m_Value );
				}
				else
				{
					std::construct_at( std::addressof( m_Error ), a_Other.m_Error );
				}
			}
			return *this;
		}

		constexpr Expected& operator=( Expected&& a_Other ) noexcept
		{
			if ( this != &a_Other )
			{
				Clear();
				m_HasValue = a_Other.m_HasValue;
				if ( m_HasValue )
				{
					std::construct_at( std::addressof( m_Value ), std::move( a_Other.m_Value ) );
				}
				else
				{
					std::construct_at( std::addressof( m_Error ), std::move( a_Other.m_Error ) );
				}
			}
			return *this;
		}

		constexpr ~Expected()
		{
			Clear();
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

	private:
		union
		{
			_Value m_Value;
			_Error m_Error;
		};
		bool m_HasValue;

	private:
		constexpr void Clear()
		{
			if ( m_HasValue )
			{
				if constexpr ( !std::is_trivially_destructible_v<_Value> )
				{
					m_Value.~_Value();
				}
			}
			else if constexpr ( !std::is_trivially_destructible_v<_Error> )
			{
				m_Error.~_Error();
			}
		}
	};

	template<typename _Error>
	class Expected<void, _Error>
	{
	public:
		using ValueType = void;
		using ErrorType = _Error;

		constexpr Expected() noexcept = default;
		constexpr Expected( const Expected& a_Other ) noexcept = default;
		constexpr Expected( Expected&& a_Other ) noexcept = default;
		constexpr Expected& operator=( const Expected& a_Other ) noexcept = default;
		constexpr Expected& operator=( Expected&& a_Other ) noexcept = default;
		constexpr Expected( const _Error& a_Error ) noexcept : m_Error( a_Error ) {}
		constexpr Expected( _Error&& a_Error ) noexcept : m_Error( std::move( a_Error ) ) {}
		constexpr Expected( const Unexpected<_Error>& a_Error ) noexcept : m_Error( a_Error.Error() ) {}
		constexpr Expected( Unexpected<_Error>&& a_Error ) noexcept : m_Error( std::move( std::move( a_Error ).Error() ) ) {}

		[[nodiscard]] constexpr bool IsError() const noexcept { return !m_Error.has_value(); }
		[[nodiscard]] constexpr bool HasValue() const noexcept { return !IsError(); }
		[[nodiscard]] constexpr _Error& Error() { EXPECTED_ASSERT( IsError(), "Attempting to access error when there is no error" ); return m_Error.value(); }
		[[nodiscard]] constexpr const _Error& Error() const { EXPECTED_ASSERT( IsError(), "Attempting to access error when there is no error" ); return m_Error.value(); }
		[[nodiscard]] constexpr explicit operator bool() const { return HasValue(); }

	private:
		Optional<_Error> m_Error{};
	};


} // namespace Tridium