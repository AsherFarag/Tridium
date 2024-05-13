#pragma once

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
	#define TE_ASSERT(x, ...) { if (!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define TE_CORE_ASSERT(x, ...) { if (!(x)) { TE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define TE_ASSERT(x, ...)
	#define TE_CORE_ASSERT(x, ...)
#endif // TE_ENABLE_ASSERTS



#define BIT(x) (1 << x)

#define TE_BIND_EVENT_FN(fn, ...) std::bind( &fn, this, __VA_ARGS__ )

#include <memory>

namespace Tridium {

	template <typename T>
	using UniquePtr = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr UniquePtr<T> MakeUnique( Args&& ... args )
	{
		return std::make_unique<T>( std::forward<Args>( args )... );
	}

	template<typename T>
	using SharedPtr = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr SharedPtr<T> MakeShared( Args&& ... args )
	{
		return std::make_shared<T>( std::forward<Args>( args )... );
	}

}