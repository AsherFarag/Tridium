#pragma once
#include <type_traits>

namespace Tridium {

	namespace Concepts {

		template<typename _To, typename _From>
		concept StaticCastable = requires(_From&& a_From)
		{
			static_cast<_To>( std::forward<_From>( a_From ) );
		};

		template<typename _To, typename _From>
		concept DynamicCastable = requires(_From && a_From)
		{
			dynamic_cast<_To>(std::forward<_From>( a_From ));
		};

		template<typename _To, typename _From>
		concept ConstCastable = requires(_From && a_From)
		{
			const_cast<_To>(std::forward<_From>( a_From ));
		};

	} // namespace Concepts

	// Cast: Wrapper for static_cast
	template<typename _To, typename _From> requires Concepts::StaticCastable<_To, _From>
	inline constexpr decltype(auto) Cast( _From&& a_From )
	{
		return static_cast<_To>( std::forward<_From>( a_From ) );
	}

	// DynamicCast: Wrapper for dynamic_cast
	template<typename _To, typename _From> requires Concepts::DynamicCastable<_To, _From>
	inline constexpr decltype(auto) DynamicCast( _From&& a_From )
	{
		return dynamic_cast<_To>( std::forward<_From>( a_From ) );
	}

	// ReinterpretCast: Wrapper for reinterpret_cast
	template<typename _To, typename _From>
	inline constexpr decltype(auto) ReinterpretCast( _From&& a_From )
	{
		return reinterpret_cast<_To>(std::forward<_From>( a_From ));
	}

	// ConstCast: Wrapper for const_cast
	template<typename _To, typename _From> requires Concepts::ConstCastable<_To, _From>
	inline constexpr decltype(auto) ConstCast( _From&& a_From )
	{
		return const_cast<_To>( std::forward<_From>( a_From ) );
	}

}