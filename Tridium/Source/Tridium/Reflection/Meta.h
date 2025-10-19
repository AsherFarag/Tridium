#pragma once
#include <Tridium/Containers/String.h>
#include <Tridium/Utils/Concepts.h>

namespace Tridium::Meta {

	template<template<auto...> class _Template, typename T>
	struct IsInstantiationOf : std::false_type {};

	template<template<auto...> class _Template, auto... _Args>
	struct IsInstantiationOf<_Template, _Template<_Args...>> : std::true_type {};

#pragma region Attributes

	// Attributes are special traits that can be attached to meta properties such as; Fields, Functions, and Bases.
	// They provide additional metadata about the reflected item which can be queried at compile-time.
	// Attributes must inherit from at least one of the following base attribute types:
	// - FieldAttribute
	// - FunctionAttribute
	// - BaseAttribute

	struct Attribute{};

	struct FieldAttribute { static constexpr bool IsFieldAttribute = true; };

	struct FunctionAttribute { static constexpr bool IsFunctionAttribute = true; };

	struct BaseAttribute { static constexpr bool IsBaseAttribute = true; };

#pragma endregion

#pragma region Is Attribute

	template<typename T>
	concept IsAttribute = Concepts::Derived<T, Attribute> || Concepts::IsSame<T, Attribute>;

	template<typename T>
	concept IsFieldAttribute = Concepts::Derived<T, FieldAttribute> || Concepts::IsSame<T, FieldAttribute>;

	template<typename T>
	concept IsFunctionAttribute = Concepts::Derived<T, FunctionAttribute> || Concepts::IsSame<T, FunctionAttribute>;

	template<typename T>
	concept IsBaseAttribute = Concepts::Derived<T, BaseAttribute> || Concepts::IsSame<T, BaseAttribute>;

#pragma endregion

#pragma region Has Attribute

	template<template<auto...> class _Attribute, typename... _Attributes>
	struct HasAttributeTemplate
	{
		static constexpr bool Value = ( ( IsInstantiationOf<_Attribute, _Attributes>::value ) || ... );
	};

	template<class _Attribute, typename... _Attributes>
	struct HasAttribute
	{
		static constexpr bool Value = ( ( Concepts::IsSame<_Attribute, _Attributes> ) || ... );
	};

#pragma endregion

#pragma region Get Attribute

	// Base case: No types left to search.
	template<template<auto...> typename _Attribute, typename...>
	struct GetAttributeTemplateImpl
	{
		static constexpr auto Value() 
		{
			return Attribute{}; // Optional fallback type
		}
	};

	// Recursive case: Check first type, then recurse
	template<template<auto...> typename _Attribute, typename _FirstAttribute, typename... _RestAttributes>
	struct GetAttributeTemplateImpl<_Attribute, _FirstAttribute, _RestAttributes...>
	{
		static constexpr auto Value()
		{
			if constexpr ( IsInstantiationOf<_Attribute, _FirstAttribute>::value )
			{
				return _FirstAttribute{};

			}
			else
			{
				return GetAttributeTemplateImpl<_Attribute, _RestAttributes...>::Value();
			}
		}
	};

	// Wrapper for ease of use in the field system
	template<template<auto...> typename _Attribute, typename... _Attributes>
	struct GetAttributeTemplate
	{
		static constexpr auto Value()
		{
			return GetAttributeTemplateImpl<_Attribute, _Attributes...>::Value();
		}

		using AttributeType = decltype( Value() );
	};

	// Base case: No types left to search.
	template<typename _Attribute, typename...>
	struct GetAttributeImpl
	{
		static constexpr auto Value()
		{
			return Attribute{}; // Optional fallback type
		}
	};

	// Recursive case: Check first type, then recurse
	template<typename _Attribute, typename _FirstAttribute, typename... _RestAttributes>
	struct GetAttributeImpl<_Attribute, _FirstAttribute, _RestAttributes...>
	{
		static constexpr auto Value()
		{
			if constexpr ( Concepts::IsSame<_Attribute, _FirstAttribute> )
			{
				return _FirstAttribute{};

			}
			else
			{
				return GetAttributeImpl<_Attribute, _RestAttributes...>::Value();
			}
		}
	};

	// Wrapper for ease of use in the field system
	template<typename _Attribute, typename... _Attributes>
	struct GetAttribute
	{
		static constexpr auto Value()
		{
			return GetAttributeImpl<_Attribute, _Attributes...>::Value();
		}

		using AttributeType = decltype( Value() );
	};

#pragma endregion

#pragma region Attribute Exclusion

	template<template<auto...> typename _Attribute>
	struct ExcludeTemplate
	{
		template<typename... _Attributes>
		constexpr static bool Matches()
		{
			if constexpr ( ( IsInstantiationOf<_Attribute, _Attributes>::value || ... ) )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template<typename _Attribute>
	struct ExcludeType
	{
		template<typename... _Attributes>
		constexpr static bool Matches()
		{
			if constexpr ( ( Concepts::IsSame<_Attribute, _Attributes> || ... ) )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template<typename... _ExcludedAttributes>
	struct ExclusionList 
	{
		template<typename... _Attributes>
		constexpr static bool ContainsAny()
		{
			if constexpr ( ( _ExcludedAttributes::template Matches<_Attributes...>() || ... ) )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template<template<auto...> typename _Attribute>
	constexpr auto Exclude() -> ExclusionList<ExcludeTemplate<_Attribute>> { return {}; }

	template<typename _Attribute>
	constexpr auto Exclude() -> ExclusionList<ExcludeType<_Attribute>> { return {}; }

	template<typename... _AttributesA, typename... _AttributesB>
	constexpr ExclusionList<_AttributesA..., _AttributesB...> operator|( ExclusionList<_AttributesA...>, ExclusionList<_AttributesB...> )
	{
		return {};
	}

#pragma endregion

#pragma region Properties

	// Helper: check if a type has an ExcludeAttributes member
	template<typename T, typename = void>
	struct HasExclusionList : std::false_type {};

	template<typename T>
	struct HasExclusionList<T, std::void_t<decltype( T::ExcludeAttributes )>> : std::true_type {};

	// Helper: check if T is valid w.r.t its exclusion list
	template<typename T, typename... AllAttributes>
	constexpr bool CheckExclusion()
	{
		if constexpr ( HasExclusionList<T>::value )
		{
			static_assert( !T::ExcludeAttributes.template ContainsAny<AllAttributes...>(), 
				"Invalid attribute combination: An attribute has been excluded by another attribute." );
			return !T::ExcludeAttributes.template ContainsAny<AllAttributes...>();
		}
		else
		{
			return true; // No exclusions => always valid
		}
	}

	// ValidateAttributes: all attributes are mutually valid
	template<typename... _Attributes>
	concept ValidateAttributes = ( CheckExclusion<_Attributes, _Attributes...>() && ... );

	template<typename... _Attributes> requires ValidateAttributes<_Attributes...>
	struct Property 
	{
		template<template<auto...> class _Attribute>
		constexpr static bool Has()
		{
			return HasAttributeTemplate<_Attribute, _Attributes...>::Value;
		}

		template<template<auto...> class _Attribute> requires HasAttributeTemplate<_Attribute, _Attributes...>::Value
		constexpr static auto Get()
		{
			return GetAttributeTemplate<_Attribute, _Attributes...>::Value();
		}

		template<class _Attribute>
		constexpr static bool Has()
		{
			return HasAttribute<_Attribute, _Attributes...>::Value;
		}

		template<class _Attribute> requires HasAttribute<_Attribute, _Attributes...>::Value
		constexpr static auto Get()
		{
			return GetAttribute<_Attribute, _Attributes...>::Value();
		}
	};

	template<auto _MemberPtr, IsFieldAttribute... _FieldAttributes>
	struct Field : Property<_FieldAttributes...> 
	{
		static constexpr auto MemberPtr = _MemberPtr;
	};

	template<auto _FunctionPtr, IsFunctionAttribute... _FunctionAttributes>
	struct Function : Property<_FunctionAttributes...> {};

	template<typename T, IsBaseAttribute... _BaseAttributes>
	struct Base : Property<_BaseAttributes...> {};

#pragma endregion

#pragma region Is Property

#pragma endregion

	//=================================================================================================
	// Reflector: Primary template for exposing reflection metadata for types.
	// Reflectors must be aggregate structs that can be reflected via FieldReflection.
	//=================================================================================================
	template<typename T>
	struct Reflector;

	//=================================================================================================
	// Checks if a type has a reflector defined.
	//=================================================================================================
	template<typename T>
	concept HasReflector = requires(T a_Type)
	{
		typename Reflector<T>;
		requires std::is_class_v<Reflector<T>>;
	};

#pragma region Common Attributes

	//=================================================================================================
	// Getter: Attribute for specifying a custom getter function for a field.
	// This will be used instead of direct member access when getting the field's value.
	//=================================================================================================
	template<auto _Function>
	struct Getter : FieldAttribute
	{
		static constexpr auto Function = _Function;
	};

	//=================================================================================================
	// Setter: Attribute for specifying a custom setter function for a field.
	// This will be used instead of direct member access when setting the field's value.
	//=================================================================================================
	template<auto _Function>
	struct Setter : FieldAttribute
	{
		static constexpr auto Function = _Function;
	};

#pragma endregion

	template<typename _Field, typename T>
	constexpr decltype( auto ) GetFieldValue( const _Field& a_Field, T& a_Instance )
	{
		if constexpr ( a_Field.template Has<Getter>() )
		{
			constexpr auto getter = a_Field.template Get<Getter>().Function;
			return ( a_Instance.*getter )();
		}
		else
		{
			constexpr auto memberPtr = _Field::MemberPtr;
			return a_Instance.*memberPtr;
		}
	}

	template<typename _Field, typename T, typename V>
	constexpr void SetFieldValue( const _Field& a_Field, T& a_Instance, V&& a_Value )
	{
		if constexpr ( a_Field.template Has<Setter>() )
		{
			constexpr auto setter = a_Field.template Get<Setter>().Function;
			( a_Instance.*setter )( std::forward<V>( a_Value ) );
		}
		else
		{
			constexpr auto memberPtr = _Field::MemberPtr;
			a_Instance.*memberPtr = std::forward<V>( a_Value );
		}
	}

} // namespace Tridium::Meta