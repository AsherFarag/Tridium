#pragma once
#include "Config.h"

// Use of namespace to hide function names from the global namespace
namespace Tridium::Internal {

#if CONFIG_DEBUG
	#define CHECKS_ENABLED 1

	// === CORE ===
	#define CORE_CHECK(x) (::Tridium::Internal::Core_Check(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__))
	#define CORE_CHECK_LOG(x, ...) (::Tridium::Internal::Core_Check_Log(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__))
	void Core_Check( bool a_Condition, const char* a_Function, const char* a_File, int a_Line );
	void Core_Check_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... );

	// === CLIENT ===
	#define CHECK(x) (::Tridium::Internal::Check(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__))
	#define CHECK_LOG(x, ...) (::Tridium::Internal::Check_Log(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__))
	void Check( bool a_Condition, const char* a_Function, const char* a_File, int a_Line );
	void Check_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... );
#else
	#define CHECKS_ENABLED 0
	#define CORE_CHECK(x) (x)
	#define CORE_CHECK_LOG(x, ...)
	#define CHECK(x)
	#define CHECK_LOG(x, ...)
#endif

#if CONFIG_DEBUG || CONFIG_RELEASE
	#define ASSERTS_ENABLED 1
	#define BREAK_ON_ASSERT_FAIL 1

	// === CORE ===
	#define CORE_ASSERT(x) (::Tridium::Internal::Core_Assert(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__))
	#define CORE_ASSERT_LOG(x, ...) (::Tridium::Internal::Core_Assert_Log(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__))
	bool Core_Assert( bool a_Condition, const char* a_Function, const char* a_File, int a_Line );
	bool Core_Assert_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... );

	// === CLIENT ===
	#define ASSERT(x) (::Tridium::Internal::Assert(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__))
	#define ASSERT_LOG(x, ...) (::Tridium::Internal::Assert_Log(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__))
	bool Assert( bool a_Condition, const char* a_Function, const char* a_File, int a_Line );
	bool Assert_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... );
#else
	#define ASSERTS_ENABLED 0
	#define BREAK_ON_ASSERT_FAIL 0
	#define CORE_ASSERT(x) (x)
	#define CORE_ASSERT_LOG(x, ...)
	#define ASSERT(x) (x)
	#define ASSERT_LOG(x, ...) (x)
#endif

#if CONFIG_DEBUG || CONFIG_RELEASE
	#define ENSURES_ENABLED 1

	// === CORE ===
	#define CORE_ENSURE(x) (::Tridium::Internal::Core_Ensure(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__))
	#define CORE_ENSURE_LOG(x, ...) (::Tridium::Internal::Core_Ensure_Log(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__))
	bool Core_Ensure( bool a_Condition, const char* a_Function, const char* a_File, int a_Line );
	bool Core_Ensure_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... );

	// === CLIENT ===
	#define ENSURE(x) (::Tridium::Internal::Ensure(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__))
	#define ENSURE_LOG(x, ...) (::Tridium::Internal::Ensure_Log(static_cast<bool>(x), __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__))
	bool Ensure( bool a_Condition, const char* a_Function, const char* a_File, int a_Line );
	bool Ensure_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... );
#else
	#define ENSURES_ENABLED 0
	#define CORE_ENSURE(x) (x)
	#define CORE_ENSURE_LOG(x, ...)
	#define ENSURE(x) (x)
	#define ENSURE_LOG(x, ...) (x)
#endif

} // namespace Tridium