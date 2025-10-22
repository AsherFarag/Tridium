#pragma once
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Utils/FunctionTraits.h>
#include <Tridium/Reflection/FieldReflection.h>

namespace Tridium::Meta {

	//=================================================================================================
	// This macro registers a type into the runtime reflection system.
	// This allows the type to be discovered and used at runtime for things like
	// serialization, scripting, editor integration, etc.
	// NOTE: The type must be Reflectable 
	// (i.e., have a Reflector specialization or be an aggregate type) to be registered.
	#define REGISTER_TYPE( _Type )

	//TODO( "Move these helpers into type traits. (And give them better names)" );
	template<template<class...> class _Template, class T>
	struct IsInstantiationOf : std::false_type {};

	template<template<class...> class _Template, class... _Types>
	struct IsInstantiationOf<_Template, _Template<_Types...>> : std::true_type {};

	template<template<auto...> class _Template, class T>
	struct IsInstantiationOfAuto : std::false_type {};

	template<template<auto...> class _Template, auto... _Values>
	struct IsInstantiationOfAuto<_Template, _Template<_Values...>> : std::true_type {};

#pragma region Attributes

	// Attributes are special traits that can be attached to meta properties such as; Fields, Functions, and Bases.
	// They provide additional metadata about the reflected item which can be queried at compile-time.
	// Attributes must inherit from at least one of the following base attribute types:
	// - FieldAttribute
	// - FunctionAttribute
	// - BaseAttribute

	// A limitation of attributes is that they cannot have mixed template parameters.
	// For example, you cannot have an attribute that takes both type and non-type template parameters.
	// template<typename Type, auto Value>
	// struct InvalidAttribute : Attribute {}; // This is not allowed.
	// Instead, the parameters must be all types or all values.

	struct Attribute{};

	struct TypeAttribute : Attribute { static constexpr bool IsTypeAttribute = true; };

	struct BaseAttribute : Attribute { static constexpr bool IsBaseAttribute = true; };

	struct FieldAttribute : Attribute { static constexpr bool IsFieldAttribute = true; };

	struct FunctionAttribute : Attribute { static constexpr bool IsFunctionAttribute = true; };

#pragma endregion

#pragma region Is Attribute

	template<typename T>
	concept IsAttribute = Concepts::Derived<T, Attribute> || Concepts::IsSame<T, Attribute>;

	template<typename T>
	concept IsTypeAttribute = Concepts::Derived<T, TypeAttribute> || Concepts::IsSame<T, TypeAttribute>;

	template<typename T>
	concept IsBaseAttribute = Concepts::Derived<T, BaseAttribute> || Concepts::IsSame<T, BaseAttribute>;

	template<typename T>
	concept IsFieldAttribute = Concepts::Derived<T, FieldAttribute> || Concepts::IsSame<T, FieldAttribute>;

	template<typename T>
	concept IsFunctionAttribute = Concepts::Derived<T, FunctionAttribute> || Concepts::IsSame<T, FunctionAttribute>;

#pragma endregion

#pragma region Has Attribute

	// Check if a list of attributes contains an attribute instantiated from a specific template that takes auto parameters.
	template<template<auto...> class _Attribute, typename... _Attributes>
	struct HasAttributeAutoTemplate
	{
		static constexpr bool Value = ( ( IsInstantiationOfAuto<_Attribute, _Attributes>::value ) || ... );
	};

	// Check if a list of attributes contains an attribute instantiated from a specific template that takes type parameters.
	template<template<typename...> class _Attribute, typename... _Attributes>
	struct HasAttributeTypeTemplate
	{
		static constexpr bool Value = ( ( IsInstantiationOf<_Attribute, _Attributes>::value ) || ... );
	};

	// Check if a list of attributes contains a specific attribute type.
	template<class _Attribute, typename... _Attributes>
	struct HasAttribute
	{
		static constexpr bool Value = ( ( Concepts::IsSame<_Attribute, _Attributes> ) || ... );
	};

#pragma endregion

#pragma region Get Attribute

	// Base case: No types left to search.
	template<template<auto...> typename _Attribute, typename...>
	struct GetAttributeAutoTemplateImpl
	{
		static constexpr auto Value() 
		{
			return Attribute{}; // Optional fallback type
		}
	};

	// Recursive case: Check first type, then recurse
	template<template<auto...> typename _Attribute, typename _FirstAttribute, typename... _RestAttributes>
	struct GetAttributeAutoTemplateImpl<_Attribute, _FirstAttribute, _RestAttributes...>
	{
		static constexpr auto Value()
		{
			if constexpr ( IsInstantiationOfAuto<_Attribute, _FirstAttribute>::value )
			{
				return _FirstAttribute{};

			}
			else
			{
				return GetAttributeAutoTemplateImpl<_Attribute, _RestAttributes...>::Value();
			}
		}
	};

	// Wrapper for ease of use in the field system
	template<template<auto...> typename _Attribute, typename... _Attributes>
	struct GetAttributeAutoTemplate
	{
		static constexpr auto Value()
		{
			return GetAttributeAutoTemplateImpl<_Attribute, _Attributes...>::Value();
		}

		using AttributeType = decltype( Value() );
	};


	// Base case: No types left to search.
	template<template<typename...> typename _Attribute, typename...>
	struct GetAttributeTypeTemplateImpl
	{
		static constexpr auto Value()
		{
			return Attribute{}; // Optional fallback type
		}
	};

	// Recursive case: Check first type, then recurse
	template<template<typename...> typename _Attribute, typename _FirstAttribute, typename... _RestAttributes>
	struct GetAttributeTypeTemplateImpl<_Attribute, _FirstAttribute, _RestAttributes...>
	{
		static constexpr auto Value()
		{
			if constexpr ( IsInstantiationOf<_Attribute, _FirstAttribute>::value )
			{
				return _FirstAttribute{};

			}
			else
			{
				return GetAttributeTypeTemplateImpl<_Attribute, _RestAttributes...>::Value();
			}
		}
	};

	// Wrapper for ease of use in the field system
	template<template<typename...> typename _Attribute, typename... _Attributes>
	struct GetAttributeTypeTemplate
	{
		static constexpr auto Value()
		{
			return GetAttributeTypeTemplateImpl<_Attribute, _Attributes...>::Value();
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
	struct ExcludeAutoTemplate
	{
		template<typename... _Attributes>
		constexpr static bool Matches()
		{
			if constexpr ( ( IsInstantiationOfAuto<_Attribute, _Attributes>::value || ... ) )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template<template<typename...> typename _Attribute>
	struct ExcludeTypeTemplate
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
	constexpr auto Exclude() -> ExclusionList<ExcludeAutoTemplate<_Attribute>> { return {}; }

	template<typename _Attribute>
	constexpr auto Exclude() -> ExclusionList<ExcludeType<_Attribute>> { return {}; }

	template<typename... _AttributesA, typename... _AttributesB>
	constexpr ExclusionList<_AttributesA..., _AttributesB...> operator|( ExclusionList<_AttributesA...>, ExclusionList<_AttributesB...> )
	{
		return {};
	}

	// Helper: check if a type has an ExcludeAttributes member
	template<typename T, typename = void>
	struct HasExclusionList : std::false_type {};

	template<typename T>
	struct HasExclusionList<T, std::void_t<decltype( T::ExcludeAttributes )>> : std::true_type {};

	// Helper: check if T is valid w.r.t its exclusion list
	template<typename _Attribute, typename... _Attributes>
	constexpr bool CheckExclusion()
	{
		if constexpr ( HasExclusionList<_Attribute>::value )
		{
			static_assert( !_Attribute::ExcludeAttributes.template ContainsAny<_Attributes...>(),
				"Invalid attribute combination: An attribute has been excluded by another attribute." );
			return !_Attribute::ExcludeAttributes.template ContainsAny<_Attributes...>();
		}
		else
		{
			return true; // No exclusions => always valid
		}
	}

#pragma endregion

#pragma region Meta Properties

	// ValidateAttributes: all attributes are mutually valid
	template<typename... _Attributes>
	concept ValidateAttributes = ( CheckExclusion<_Attributes, _Attributes...>() && ... );

	//=================================================================================================
	// Attribute List: A list of attributes stored in type information.
	// All property types (Type, Base, Field, Function, etc) that support attributes inherit from this.
	//=================================================================================================
	template<typename... _Attributes> requires ValidateAttributes<_Attributes...>
	struct AttributeList 
	{
		template<template<auto...> class _Attribute>
		constexpr static bool Has()
		{
			return HasAttributeAutoTemplate<_Attribute, _Attributes...>::Value;
		}

		template<template<typename...> class _Attribute>
		constexpr static bool Has()
		{
			return HasAttributeTypeTemplate<_Attribute, _Attributes...>::Value;
		}

		template<class _Attribute>
		constexpr static bool Has()
		{
			return HasAttribute<_Attribute, _Attributes...>::Value;
		}

		template<template<auto...> class _Attribute> requires HasAttributeAutoTemplate<_Attribute, _Attributes...>::Value
		constexpr static auto Get()
		{
			return GetAttributeAutoTemplate<_Attribute, _Attributes...>::Value();
		}

		template<template<typename...> class _Attribute> requires HasAttributeTypeTemplate<_Attribute, _Attributes...>::Value
		constexpr static auto Get()
		{
			return GetAttributeTypeTemplate<_Attribute, _Attributes...>::Value();
		}

		template<class _Attribute> requires HasAttribute<_Attribute, _Attributes...>::Value
		constexpr static auto Get()
		{
			return GetAttribute<_Attribute, _Attributes...>::Value();
		}
	};

	//=================================================================================================
	// Accessor: Helper used by Fields and Properties for defining Getters and Setters.
	template<auto _Getter, auto _Setter = _Getter>
	struct Accessor
	{
		using MemberPointerTraits = std::conditional_t< _Getter != nullptr,
			MemberPointerTraits<decltype( _Getter )>,
			MemberPointerTraits<decltype( _Setter )>>;

		using GetterType = decltype( _Getter );
		using SetterType = decltype( _Setter );
		using ObjectType = typename MemberPointerTraits::ClassType;
		using MemberType = typename MemberPointerTraits::MemberType;
		static constexpr auto Getter = _Getter;
		static constexpr auto Setter = _Setter;
		static constexpr bool CanGet = true;
		static constexpr bool CanSet = _Setter != nullptr;
		static constexpr auto RawAccess = std::is_same_v<GetterType, SetterType> ? Getter : nullptr;

		constexpr static decltype( auto ) Get( ObjectType& a_Object ) requires ( CanGet )
		{
			if constexpr ( std::is_member_function_pointer_v<GetterType> )
			{
				return ( a_Object.*_Getter )( );
			}
			else
			{
				return a_Object.*_Getter;
			}
		}

		constexpr static decltype( auto ) Get( const ObjectType& a_Object ) requires ( CanGet )
		{
			if constexpr ( std::is_member_function_pointer_v<GetterType> )
			{
				return ( a_Object.*_Getter )( );
			}
			else
			{
				return a_Object.*_Getter;
			}
		}

		template<typename _Value>
		constexpr static void Set( ObjectType& a_Object, _Value&& a_Value ) requires ( CanSet )
		{
			if constexpr ( std::is_member_function_pointer_v<SetterType> )
			{
				( a_Object.*_Setter )( std::forward<_Value>( a_Value ) );
			}
			else
			{
				a_Object.*_Setter = std::forward<_Value>( a_Value );
			}
		}
	};

	//=================================================================================================
	// MetaProperty: Meta properties are members of a Reflector specialization.
	//=================================================================================================
	struct MetaProperty {};

	//=================================================================================================
	// Type: Used for declaring type-level attributes.
	// For this to be registered and used, it must be declared as 'using Type = ...'
	// E.g.,
	// 	template<>
	// 	struct Reflector<BoxColliderComponent>
	//  {
	//		using Type = Type<RequireComponents<RigidBodyComponent>>;
	//  };
	//=================================================================================================
	template<typename T, IsTypeAttribute... _TypeAttributes>
	struct Type : MetaProperty, AttributeList<_TypeAttributes...>
	{
		using ClassType = T;
		using Attributes = AttributeList<_TypeAttributes...>;
	};

	//=================================================================================================
	// Base: used for declaring a base class the type inherits from,
	// along with any attributes for that base.
	// E.g.,
	//  struct Animal {};
	//  struct Dog : Animal {};
	// 
	// 	template<>
	// 	struct Reflector<Dog>
	// 	{
	//		Base<Animal> AnimalBase;
	// 	};
	//=================================================================================================
	template<typename T, IsBaseAttribute... _BaseAttributes>
	struct Base : MetaProperty, AttributeList<_BaseAttributes...> {};

	//=================================================================================================
	// Field: Used for declaring member variables within a type along with any attributes.
	// The first template parameter is an accessor that defines how to get and set the field's value.
	// Getters and setters can be either member pointers, member functions, free functions, or a mix of them.
	// In case of free functions,
	// setters and getters must accept a reference to an instance of the parent type as their first argument.
	// A setter has then an extra argument of a type convertible to that of the parameter to set.
	// In case of member functions, getters have no arguments at all,
	// while setters has an argument of a type convertible to that of the parameter to set.
	// NOTE: If you want to define a custom getter and setter, use the Property alias instead.
	// E.g.,
	//  struct Player
	//  {
	//		float Health;
	//  };
	// 
	// 	template<>
	// 	struct Reflector<Player>
	// 	{
	//		Field<&Player::Health, Editable> Health;
	// 	};
	//=================================================================================================
	template<auto _Accessor, IsFieldAttribute... _FieldAttributes> 
	struct Field : MetaProperty, AttributeList<_FieldAttributes...>
	{
		using Attributes = AttributeList<_FieldAttributes...>;
		using Accessor = std::conditional_t<IsInstantiationOfAuto<Accessor, decltype( _Accessor )>::value, decltype( _Accessor ), Accessor<_Accessor>>;
		using ObjectType = typename Accessor::ObjectType;
		using MemberType = typename Accessor::MemberType;

		constexpr static decltype( auto ) GetValue( ObjectType& a_Instance ) requires ( Accessor::CanGet )
		{
			return Accessor::Get( a_Instance );
		}

		constexpr static decltype( auto ) GetValue( const ObjectType& a_Instance ) requires ( Accessor::CanGet )
		{
			return Accessor::Get( a_Instance );
		}

		template<typename _Value>
		constexpr static void SetValue( ObjectType& a_Instance, _Value&& a_Value ) requires ( Accessor::CanSet )
		{
			Accessor::Set( a_Instance, std::forward<_Value>( a_Value ) );
		}
	};

	//=================================================================================================
	// Property: Alias for defining a field with custom getter and setter functions.
	// You can make a property read-only by passing nullptr as the setter.
	// NOTE: See comments above 'Field' for more details on how getters and setters work.
	// Usage E.g.,
	//  struct Player
	//  {
	//		float GetHealth() const;
	//		void SetHealth(float health);
	//		int GetReadOnlyValue() const;
	//  };
	//
	// 	template<>
	// 	struct Reflector<Player>
	// 	{
	//		Property<&Player::GetHealth, &Player::SetHealth, Editable> Health;
	//      Property<&Player::GetReadOnlyValue, nullptr, Editable> ReadOnlyValue;
	// 	};
	//=================================================================================================
	template<auto _Getter, auto _Setter, IsFieldAttribute... _FieldAttributes> requires ( _Getter != nullptr )
	using Property = Field<Accessor<_Getter, _Setter>{}, _FieldAttributes... > ;

	//=================================================================================================
	// Function: Used for declaring functions within a type along with any attributes for that function.
	// E.g.,
	//  struct Player
	//  {
	//		void Jump();
	//  };
	//
	// 	template<>
	// 	struct Reflector<Player>
	// 	{
	//		Function<&Player::Jump, Scriptable> JumpFunction;
	// 	};
	//=================================================================================================
	template<auto _FunctionPtr, IsFunctionAttribute... _FunctionAttributes>
	struct Function : MetaProperty, AttributeList<_FunctionAttributes...>
	{
		using Attributes = AttributeList<_FunctionAttributes...>;
		using FunctionType = decltype( _FunctionPtr );
		using FunctionTraits = apl::function_traits<FunctionType>;
		using ClassType = typename FunctionTraits::object_type;
		static constexpr auto FunctionPtr = _FunctionPtr;
		static constexpr bool IsStatic = apl::is_static_fn_v<FunctionType>;

		// MSVC workaround: cannot use 'ClassType' directly in a parameter list if 'IsStatic' is true.
		// Because in that case, 'ClassType' may be 'void', which is not a valid parameter type.
		// And for some reason, MSVC still tries to evaluate the parameter type even in the 'requires' clause.
		using _WorkAround = std::conditional_t<IsStatic, void*, ClassType>;

		template<typename... _Args, typename = std::enable_if_t<!IsStatic>>
		constexpr static decltype( auto ) operator()( _WorkAround& a_Instance, _Args&&... a_Args ) requires ( !IsStatic )
		{
			return ( a_Instance.*FunctionPtr )( std::forward<_Args>( a_Args )... );
		}

		template<typename... _Args>
		constexpr static decltype( auto ) operator()( _Args&&... a_Args ) requires ( IsStatic )
		{
			return FunctionPtr( std::forward<_Args>( a_Args )... );
		}
	};

#pragma endregion

#pragma region Is Meta Property

	template<typename T>
	concept IsMeta = Concepts::Derived<T, MetaProperty>;

	template<typename T>
	struct IsBaseT : std::false_type {};
	template<typename T, typename... _BaseAttributes>
	struct IsBaseT<Base<T, _BaseAttributes...>> : std::true_type {};
	template<typename T>
	concept IsBase = IsBaseT<std::remove_const_t<std::remove_reference_t<T>>>::value;

	template<typename T>
	struct IsFieldT : std::false_type {};
	template<auto _Accessor, typename... _FieldAttributes>
	struct IsFieldT<Field<_Accessor, _FieldAttributes...>> : std::true_type {};
	template<typename T>
	concept IsField = IsFieldT<std::remove_const_t<std::remove_reference_t<T>>>::value;

	template<typename T>
	struct IsFunctionT : std::false_type {};
	template<auto _FunctionPtr, IsFunctionAttribute... _FunctionAttributes>
	struct IsFunctionT<Function<_FunctionPtr, _FunctionAttributes...>> : std::true_type {};
	template<typename T>
	concept IsFunction = IsFunctionT<std::remove_const_t<std::remove_reference_t<T>>>::value;

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

	//=================================================================================================
	// Reflectable: Checks if a type is reflectable,
	// either by having a reflector defined or being an aggregate type (that can be reflected automatically).
	//=================================================================================================
	template<typename T>
	concept Reflectable = HasReflector<T> || Concepts::Aggregate<T>;

	template<Reflectable T>
	struct ReflectableInterface;

	template<Reflectable T> requires HasReflector<T>
	struct ReflectableInterface<T>
	{
		static constexpr Reflector<T> Reflector{};
		T* Instance;

		template<typename _Func>
		constexpr void ForEach( _Func&& a_Callback ) const
		{
		}
	};

	template<Reflectable T> requires ( !HasReflector<T> )
	struct ReflectableInterface<T>
	{
		T* Instance;

		template<typename _Func>
		constexpr void ForEach( _Func&& a_Callback ) const
		{
		}
	};


	//=================================================================================================
	template<Reflectable T>
	constexpr ReflectableInterface<T> AsReflectable( T& a_Instance )
	{
		return ReflectableInterface<T>{ &a_Instance };
	}

	template<typename T>
	constexpr ReflectableInterface<const T> AsReflectable( const T& a_Type )
	{
		return {};
	}

	//=================================================================================================
	// Checks if the reflector has a 'Type' property defined.
	// Otherwise, the default 'Type<T>' will be used.
	template<typename T>
	constexpr auto GetType( const T& )
	{
		if constexpr ( requires { typename Reflector<T>::Type; } )
		{
			//TODO( "This safety check is needed" );
			//if constexpr ( IsInstantiationOf<Type, decltype( Reflector<T>::Type )> )
			{
				return typename Reflector<T>::Type{};
			}
		}
		else
		{
			return Type<T>{};
		}
	}

	//=================================================================================================
	// Iterates over each base class defined in the reflector.
	template<typename T, typename _Func>
	constexpr void ForEachBase( const Reflector<T>& a_Reflector, _Func&& a_Callback )
	{
		::Tridium::ForEachField( a_Reflector, [&]( const auto& a_FieldName, const auto& a_Field )
		{
			if constexpr ( IsBase<decltype( a_Field )> )
			{
				a_Callback( a_FieldName, a_Field );
			}
		} );
	}

	//=================================================================================================
	// Iterates over each field/property defined in the reflector.
	template<typename T, typename _Func>
	constexpr void ForEachField( const Reflector<T>& a_Reflector, _Func&& a_Callback )
	{
		::Tridium::ForEachField( a_Reflector, [&]( const auto& a_FieldName, const auto& a_Field )
		{
			if constexpr ( IsField<decltype( a_Field )> )
			{
				a_Callback( a_FieldName, a_Field );
			}
		} );
	}

	//=================================================================================================
	// Iterates over each function defined in the reflector.
	template<typename T, typename _Func>
	constexpr void ForEachFunction( const Reflector<T>& a_Reflector, _Func&& a_Callback )
	{
		::Tridium::ForEachField( a_Reflector, [&]( const auto& a_FieldName, const auto& a_Field )
		{
			if constexpr ( IsFunction<decltype( a_Field )> )
			{
				a_Callback( a_FieldName, a_Field );
			}
		} );
	}

} // namespace Tridium::Meta

#include <Tridium/Reflection/MetaAttributes.h>