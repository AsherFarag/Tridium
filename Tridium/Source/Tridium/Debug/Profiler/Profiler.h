#pragma once
#include <Tridium/Core/Config.h>
#include <Tridium/Utils/Macro.h>

#ifndef ENABLE_PROFILING
	#if CONFIG_DEBUG
		#define ENABLE_PROFILING 1
	#else
		#define ENABLE_PROFILING 0
	#endif
#endif // !ENABLE_PROFILING

// ==========================================
// Internal Macros
// ==========================================

#if defined( _MSC_VER )
	#define _FUNCTION_ EXPAND( __FUNCTION__ )
	#define _FUNCTION_SIG_ EXPAND( __FUNCSIG__ )
#elif defined(__clang__)
	#define _FUNCTION_ EXPAND( __PRETTY_FUNCTION__ )
	#define _FUNCTION_SIG_ EXPAND( __PRETTY_FUNCTION__ )
#endif

// For some reason, in MSVC, __LINE__ is not a constant expression.
// This is a workaround to make it a constant expression.
#define _USABLE_LINE_ uint32_t(_CONCAT(__LINE__,U))

// ==========================================



// ==========================================
// User Profiling Macros
// ==========================================
#if ENABLE_PROFILING
	#include "Instrumentor.h"

#pragma region Internal Profile Scope Macros

	#define _PROFILE_SCOPE_3( name, filter, color ) \
		static constexpr ::Tridium::ProfileDescription EXPAND( _CONCAT(__ProfileDescription_, __LINE__ ) ) = { \
			name, __FILE__, _USABLE_LINE_, filter, color \
		}; \
		::Tridium::ProfileScopeGuard _CONCAT(__ProfileScopeGuard_, __LINE__ )( &EXPAND( _CONCAT(__ProfileDescription_, __LINE__ ) ) )

	#define _PROFILE_SCOPE_2(name, category) _PROFILE_SCOPE_3(name, category.Filter, category.Color)

	#define _PROFILE_SCOPE_1(name) _PROFILE_SCOPE_2(name, ::Tridium::ProfilerCategory::None)

#pragma endregion

	// ==========================================
	// Profile Scope
	//  Used for profiling a scope with a name.
	//  This macro is variadic and can take 1, 2 or 3 arguments.
	//  1 argument: name
	//    name: The name of the scope. Can be a string literal or a variable.
	//    E.g. PROFILE_SCOPE("MyScope");
	//  2 arguments: name, category
	//    name: The name of the scope. Can be a string literal or a variable.
	//    category: The category of the profile. Must be a ProfilerCategory::Category.
	//    E.g. PROFILE_SCOPE("MyScope", ProfilerCategory::AI);
	//  3 arguments: name, filter, color
	//    name: The name of the scope. Can be a string literal or a variable.
	//    filter: The filter of the profile. Must be a ProfilerFilter::Filter.
	//    color: The color of the profile. Must be a ProfilerColor/uint32_t.
	//    E.g. PROFILE_SCOPE("MyScope", ProfilerFilter::AI, 0xFF0000);
	// ==========================================
	#define PROFILE_SCOPE(...) EXPAND( SELECT_MACRO_3( __VA_ARGS__, _PROFILE_SCOPE_3, _PROFILE_SCOPE_2, _PROFILE_SCOPE_1 )(__VA_ARGS__) )

#pragma region Internal Profile Function Macros

#define _CLEAN_FUNCTION_ _FUNCTION_
#define _CLEAN_FUNCTION_SIG_ _FUNCTION_SIG_

#define _PROFILE_FUNCTION_3(filter, color) \
		static constexpr ::Tridium::ProfileDescription EXPAND( _CONCAT(__ProfileDescription_, __LINE__ ) ) = { \
			_CLEAN_FUNCTION_, _CLEAN_FUNCTION_SIG_, __FILE__, _USABLE_LINE_, filter, color \
		}; \
		::Tridium::ProfileScopeGuard _CONCAT(__ProfileScopeGuard_, __LINE__ )( &EXPAND( _CONCAT(__ProfileDescription_, __LINE__ ) ) )

#define _PROFILE_FUNCTION_2(category) _PROFILE_FUNCTION_3(category.Filter, category.Color)

#define _PROFILE_FUNCTION_1() _PROFILE_FUNCTION_2(::Tridium::ProfilerCategory::None)

// Used for selecting the correct profile function macro,
// as _PROFILE_FUNCTION_1 has no arguments.
#define __DUMMY__ 0

#pragma endregion

	// ==========================================
	// Profile Function
	//  Used for profiling a function.
	//  This macro is variadic and can take 0, 1 or 2 arguments.
	//  0 arguments: No arguments.
	//    E.g. PROFILE_FUNCTION();
	//  1 argument: category
	//    category: The category of the profile. Must be a ProfilerCategory::Category.
	//    E.g. PROFILE_FUNCTION(ProfilerCategory::AI);
	//  2 arguments: filter, color
	//    filter: The filter of the profile. Must be a ProfilerFilter::Filter.
	//    color: The color of the profile. Must be a ProfilerColor/uint32_t.
	//    E.g. PROFILE_FUNCTION(ProfilerFilter::AI, 0xFF0000);
	// ==========================================
#define PROFILE_FUNCTION(...) EXPAND( SELECT_MACRO_3( __DUMMY__, __VA_ARGS__, _PROFILE_FUNCTION_3, _PROFILE_FUNCTION_2, _PROFILE_FUNCTION_1 )(__VA_ARGS__) )

	// ==========================================
	// Profile Frame
	//  Used for declaring a frame for profiling.
	// ==========================================
#define PROFILE_FRAME() ::Tridium::ProfileFrameGuard __ProfileFrameGuard__

#else
	#define PROFILE_SCOPE(...)
	#define PROFILE_FUNCTION(...)
	#define PROFILE_FRAME()
#endif // !ENABLE_PROFILING

// ==========================================