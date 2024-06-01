#pragma once

// - Common Includes -
#include <Tridium/Math/Math.h>
#include <Tridium/Core/Time.h>
#include <memory>
#include <filesystem>
namespace fs = std::filesystem;

#define BIT(x) (1 << x)

#define Stringize( x ) #x 
#define WRAP( m, x ) m(x)
#define PRAGMA(p) _Pragma(#p)
#define EXPAND(x) x

#define SELECT_MACRO_2(_1, _2, x, ...) x

#define TE_BIND_EVENT_FN(fn, ...) std::bind( &fn, this, __VA_ARGS__ )

#ifdef TE_PLATFORM_WINDOWS

	#ifdef TE_DLL
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

namespace Tridium {

#pragma region Memory

	template <typename T>
	using UniquePtr = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr UniquePtr<T> MakeUnique( Args&& ... args )
	{
		return std::make_unique<T>( std::forward<Args>( args )... );
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> MakeRef( Args&& ... args )
	{
		return std::make_shared<T>( std::forward<Args>( args )... );
	}

	// Type alias for std::weak_ptr
	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	// Function to convert std::shared_ptr to std::weak_ptr
	template<typename T>
	constexpr WeakRef<T> MakeWeakRef( const std::shared_ptr<T>& sharedPtr )
	{
		return WeakRef<T>( sharedPtr );
	}

#pragma endregion

}