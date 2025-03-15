#pragma once
#include "TypeTraits.h"

namespace Tridium::Concepts {

	// std::Is Same
	template<typename _First, typename _Second>
	concept IsSame = std::is_same_v<_First, _Second>;

	// std::Same as
	template<typename _First, typename _Second>
	concept SameAs = std::same_as<_First, _Second>;

	template<typename _First, typename _Second>
	concept IsConvertible = std::is_convertible_v<_First, _Second>;

	template<typename T>
	concept IsPointer = std::is_pointer_v<T>;

	template<typename T>
	concept IsReference = std::is_reference_v<T>;

	template<typename T>
	concept IsTriviallyCopyable = std::is_trivially_copyable_v<T>;

	template<typename T>
	concept IsTriviallyDestructible = std::is_trivially_destructible_v<T>;

	template<typename T>
	concept IsTriviallyCopyAssignable = std::is_trivially_copy_assignable_v<T>;

	template<typename T>
	concept IsDefaultConstructable = std::is_default_constructible_v<T>;

	template<typename T>
	concept Aggregate = std::is_aggregate_v<T>;

	// Checks if the type can be initialized with the given arguments via aggregate initialization.
	template<typename T, typename... _Args>
	concept IsAggregateInitializable = requires { T{ std::declval<_Args>()... }; };

	template<typename T>
	concept IsStruct = std::is_class_v<T> && Aggregate<T>;

	namespace Detail {
		template<typename T, template<typename...> typename _Template>
		constexpr bool IsSpecialisation = false;

		template<template<typename...> typename _Template, typename... Args>
		constexpr bool IsSpecialisation<_Template<Args...>, _Template> = true;
	}

	// Checks if the first type is a specialisation of the second type.
	// Example: IsSpecialisation<std::vector<int>, std::vector> == true
	template<typename T, template<typename...> typename _Template>
	concept IsSpecialisation = Detail::IsSpecialisation<T, _Template>;

	//================================================================
	// Inheritance
	//================================================================
	
	// Checks if the second type derives from the first type.
	template<typename _Base, typename _Derived>
	concept IsBaseOf = std::is_base_of_v<_Base, _Derived>;

}