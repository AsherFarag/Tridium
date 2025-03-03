#pragma once

// - Common Includes -
#include <Tridium/Core/Types.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Time.h>
#include <Tridium/Core/GUID.h>
#include <Tridium/Core/Color.h>
#include <Tridium/Math/Math.h>
#include <Tridium/Math/Rotator.h>
#include <Tridium/Asset/Asset.h>
#include <Tridium/IO/FilePath.h>
#include <Tridium/Utils/Macro.h>
#include <Tridium/Reflection/ReflectionFwd.h>

namespace Tridium {

	template<typename _Flags, typename _EnumFlag>
	constexpr bool HasFlag( _Flags Flags, _EnumFlag Flag )
	{
		std::underlying_type_t<_EnumFlag> flag = static_cast<std::underlying_type_t<_EnumFlag>>( Flag );
		return ( static_cast<std::underlying_type_t<_EnumFlag>>(Flags) & static_cast<std::underlying_type_t<_EnumFlag>>( Flag ) ) == flag;
	}

	template<typename _Array>
	bool IsValidIndex( const _Array& a_Array, uint32_t a_Index )
	{
		return  a_Index >= 0 && a_Index < a_Array.size();
	}

	inline constexpr void HashCombine( size_t& a_Seed, size_t a_Hash )
	{
		a_Seed ^= a_Hash + 0x9e3779b9 + ( a_Seed << 6 ) + ( a_Seed >> 2 );
	}

}

#define TE_BIND_EVENT_FN(fn, PlaceHolder) std::bind( &fn, this, std::placeholders::_##PlaceHolder )

// - Configuration -

#ifdef IS_EDITOR
	#define IS_EDITOR 1
#else
	#define IS_EDITOR 0
#endif // IS_EDITOR

#ifdef TE_DEBUG
	#define TE_DEBUG 1
#else
	#define TE_DEBUG 0
#endif // TE_DEBUG

#ifdef TE_RELEASE
	#define TE_RELEASE 1
#else
	#define TE_RELEASE 0
#endif // TE_RELEASE

#ifdef TE_SHIPPING
	#define TE_SHIPPING 1
#else
	#define TE_SHIPPING 0
#endif // TE_SHIPPING

#if !TE_SHIPPING
	#define TE_USE_DEBUG 1
#else
	#define TE_USE_DEBUG 0
#endif

#if TE_USE_DEBUG
	#define TE_DRAW_DEBUG 1
#else
	#define TE_DRAW_DEBUG 0
#endif

// ------------------

#ifdef TE_PLATFORM_WINDOWS

	#define TE_DLL 0

	#if TE_DLL
		#ifdef TE_BUILD_DLL
			#define TRIDIUM_API __declspec(dllexport)
		#else
			#define TRIDIUM_API __declspec(dllimport)
		#endif // TE_BUILD_DLL
	#else
		#define TRIDIUM_API
	#endif // TE_DLL

#else
	#error Tridium currently only supports Windows!
#endif // TE_PLATFORM_WINDOWS

#define TE_ENABLE_ASSERTS

#ifdef TE_ENABLE_ASSERTS
	#define TE_CORE_ASSERT_1(x) { if (!(x)) { __debugbreak(); } }
	#define TE_CORE_ASSERT_2(x, ...) { if (!(x)) { TE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define TE_CORE_ASSERT(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, TE_CORE_ASSERT_2, TE_CORE_ASSERT_1 )(__VA_ARGS__))

	#define TE_ASSERT(x, ...) { if (!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define TE_ASSERT(x, ...)
	#define TE_CORE_ASSERT(x, ...)
#endif // TE_ENABLE_ASSERTS

#ifdef TE_DEBUG
	#define TE_ENABLE_CHECKS
#endif // TE_DEBUG


#ifdef TE_ENABLE_CHECKS
	#define CHECK(x) { if (!(x)) { __debugbreak(); } }
#else
	#define CHECK(x)
#endif // TE_ENABLE_CHECKS