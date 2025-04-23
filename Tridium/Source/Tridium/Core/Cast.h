#pragma once

namespace Tridium {

	// Cast: Wrapper for static_cast
	template<typename _To, typename _From>
	static constexpr decltype(auto) Cast( const _From&& a_From )
	{
		return static_cast<_To>(a_From);
	}

	// Cast: Wrapper for static_cast
	template<typename _To, typename _From>
	static constexpr decltype(auto) Cast( _From&& a_From )
	{
		return static_cast<_To>(a_From);
	}

	// DynamicCast: Wrapper for dynamic_cast
	template<typename _To, typename _From>
	static constexpr decltype(auto) DynamicCast( const _From&& a_From )
	{
		return dynamic_cast<_To>(a_From);
	}

	// DynamicCast: Wrapper for dynamic_cast
	template<typename _To, typename _From>
	static constexpr decltype(auto) DynamicCast( _From&& a_From )
	{
		return dynamic_cast<_To>(a_From);
	}

	// ReinterpretCast: Wrapper for reinterpret_cast
	template<typename _To, typename _From>
	static constexpr decltype(auto) ReinterpretCast( const _From&& a_From )
	{
		return reinterpret_cast<_To>(a_From);
	}

	// ReinterpretCast: Wrapper for reinterpret_cast
	template<typename _To, typename _From>
	static constexpr decltype(auto) ReinterpretCast( _From&& a_From )
	{
		return reinterpret_cast<_To>(a_From);
	}

	// ConstCast: Wrapper for const_cast
	template<typename _To, typename _From>
	static constexpr decltype(auto) ConstCast( const _From&& a_From )
	{
		return const_cast<_To>(a_From);
	}

	// ConstCast: Wrapper for const_cast
	template<typename _To, typename _From>
	static constexpr decltype(auto) ConstCast( _From&& a_From )
	{
		return const_cast<_To>(a_From);
	}

}