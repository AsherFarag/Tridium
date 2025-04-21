#pragma once
#include <Tridium/Core/Enum.h>
#include <Tridium/Utils/FunctionTraits.h>

namespace Tridium {

	//=====================================================
	// Meta Attribute
	//  A base class for all meta attributes.
	//  These are to be used with Field<T> to store static type traits.
	//  Fields can be queried for these attributes at compile time, via Field::Has<T>() and Field::Get<T>().
	//  NOTE: Custom meta attributes should inherit from this class and must have type traits defined.
	//        For example, Range<size_t Min, size_t Max> must have:
	//        template<typename T> 
	//        struct RangeAttribute : std::false_type {};
	//        template<size_t _Min, size_t _Max>
	//        struct RangeAttribute<Range<_Min, _Max>> : std::true_type {};
	//
	//        This allows the static reflection system to query for these attributes, regardless of the parameterized types.
	struct MetaAttribute {};



	//=====================================================
	// Is Meta Attribute
	//  A concept to check if 'T' is a valid meta attribute.
	template<typename T>
	concept IsMetaAttribute = std::is_base_of_v<MetaAttribute, T> || std::is_same_v<T, MetaAttribute>;



	//=====================================================
	// Meta Property
	//  A base class for all meta properties such as functions and fields.
	template<IsMetaAttribute... _MetaAttributes>
	struct MetaProperty : _MetaAttributes...
	{
	private:
		// Helper to check if a trait is present in the meta attributes
		template<template<typename> typename _Trait>
		struct HasTrait
		{
			static constexpr bool Value = (_Trait<_MetaAttributes>::value || ...);
		};

		// Helper to get the first trait that matches the given template
		// Recursively checks each type in the variadic template pack until it finds a match.
		template<template<typename> typename _Trait, typename... Ts>
		struct GetTraitImpl;

		template<template<typename> typename _Trait, typename T, typename... Ts>
		struct GetTraitImpl<_Trait, T, Ts...>
		{
			static constexpr auto Value()
			{
				if constexpr ( _Trait<T>::value )
					return T{};
				else
					return GetTraitImpl<_Trait, Ts...>::Value();
			}
		};

		template<template<typename> typename _Trait>
		struct GetTraitImpl<_Trait>
		{
			static constexpr auto Value()
			{
				static_assert(sizeof( _Trait ) == 0, "No matching type found");
			}
		};

		// Helper to get a trait's value
		template<template<typename> typename _Trait>
		struct GetTrait
		{
			static constexpr auto Value()
			{
				return GetTraitImpl<_Trait, _MetaAttributes...>::Value();
			}
		};

	public:
		// Returns whether the given trait is present in the meta attributes
		template<template<typename> typename _Trait>
		static consteval bool Has()
		{
			return HasTrait<_Trait>::Value;
		}

		// Returns the first trait that matches the given template
		template<template<typename> typename _Trait> requires HasTrait<_Trait>::Value
		static consteval auto Get()
		{
			return GetTrait<_Trait>::Value();
		}
	};



	//=====================================================
	// Field Attribute
	//  A base class for all field attributes.
	//  Attributes that are specific to 'Field' must inherit from this class.
	struct FieldAttribute : MetaAttribute {};



	//=====================================================
	// Is Field Attribute
	//  A concept to check if 'T' is a valid field attribute.
	template<typename T>
	concept IsFieldAttribute = std::is_base_of_v<FieldAttribute, T> || std::is_same_v<T, FieldAttribute>;



	//=====================================================
	// Field
	//  A wrapper around a value type that stores static type traits such as meta attributes.
	//  Template parameters:
	//  - _ValueType: The type of the value. For example, int, float, etc.
	//  - _FieldAttributes: A variadic template parameter that allows for multiple meta attributes to be passed in.
	template<typename _ValueType, IsFieldAttribute... _FieldAttributes>
	struct Field : MetaProperty<_FieldAttributes...>
	{
	public:
		using FieldType = Field<_ValueType, _FieldAttributes...>;
		using ValueType = _ValueType;
		ValueType Value;
	};



	template<typename T>
	struct IsField_Trait : std::false_type {};
	template<typename _ValueType, IsFieldAttribute... _FieldAttributes>
	struct IsField_Trait<Field<_ValueType, _FieldAttributes...>> : std::true_type {};
	//=====================================================
	// Is Field
	//  A useful concept for checking if 'T' is a Field type.
	template<typename T>
	concept IsField = IsField_Trait<T>::value;


#pragma region Field Attributes

	//=====================================================
	// EditAnywhere - Editor Only
	//  A meta attribute used for specifying that a field is visible and can be modified in the editor.
	struct EditAnywhere : FieldAttribute {};
	template<typename T>
	struct EditAnywhereAttribute : std::false_type {};
	template<>
	struct EditAnywhereAttribute<EditAnywhere> : std::true_type {};
	//=====================================================

	//=====================================================
	// VisibleAnywhere - Editor Only
	//  A meta attribute used for specifying that a field is visible in the editor, but cannot be modified.
	struct VisibleAnywhere : FieldAttribute {};
	template<typename T>
	struct VisibleAnywhereAttribute : std::false_type {};
	template<>
	struct VisibleAnywhereAttribute<VisibleAnywhere> : std::true_type {};
	//=====================================================
	
	//=====================================================
	// Range - Editor Only
	//  Describes a valid range for a field.
	//  In the editor, this field will show a slider for the range.
	template<auto _Min, auto _Max>
		requires (std::is_arithmetic_v<decltype(_Min)>&& std::is_arithmetic_v<decltype(_Max)>)
			  && (_Min < _Max)
	struct Range : FieldAttribute
	{
		static constexpr auto Min = _Min;
		static constexpr auto Max = _Max;
	};
	template<typename T> 
	struct RangeAttribute : std::false_type {};
	template<auto _Min, auto _Max> 
	struct RangeAttribute<Range<_Min, _Max>> : std::true_type {};
	//=====================================================

	//=====================================================
	// Getter - Editor Only
	//  A meta attribute used for specifying a getter function for a field that will be used by the inspector.
	//  '_Getter' can be a member function pointer.
	template<typename T>
	struct GetterAttribute : std::false_type {};
	template<auto _Getter>
	struct Getter : FieldAttribute
	{
	private:
		static constexpr bool IsFieldMemberPointer = std::is_member_object_pointer_v<decltype(_Getter)>;
		template<typename T>
		static constexpr bool IsInvocable = requires(const T & a_Object) {
			std::invoke( _Getter, a_Object );
		};
	public:
		template<typename T>
		static constexpr auto GetValue( const T& a_Object )
		{
			// If _Getter is a member pointer, such as &MyComponent::MyInt, we need to dereference it.
			if constexpr ( IsFieldMemberPointer )
			{
				using ValueType = std::decay_t<decltype(a_Object.*_Getter)>;
				// If the member pointer is to a Field, we need to use .Value to get the value.
				if constexpr ( IsField<ValueType> )
				{
					// If the field has a defined getter attribute, we need to call it.
					if constexpr ( ValueType::template Has<GetterAttribute>() )
					{
						return (a_Object.*_Getter).GetValue( a_Object );
					}
					// Else return the value directly.
					else
					{
						return (a_Object.*_Getter).Value;
					}
				}
				else
				{
					return a_Object.*_Getter;
				}
			}
			// If _Getter is a function pointer, such as &MyComponent::GetInt, we need to call it.
			else if constexpr ( IsInvocable<T> )
			{
				return std::invoke( _Getter, a_Object );
			}
			else
			{
				static_assert(false, "Getter must be a member pointer or a callable that takes the object as an argument.");
			}
		}
	};
	template<auto _Getter>
	struct GetterAttribute<Getter<_Getter>> : std::true_type {};
	//=====================================================

	//=====================================================
	// Setter - Editor Only
	//  A meta attribute used for specifying a setter function for a field that will be used by the inspector.
	//  '_Setter' can be a member function pointer.
	template<typename T>
	struct SetterAttribute : std::false_type {};
	template<auto _Setter>
	struct Setter : FieldAttribute
	{
	private:
		static constexpr bool IsFieldMemberPointer = std::is_member_object_pointer_v<decltype(_Setter)>;
		template<typename _ObjectType, typename _ValueType>
		static constexpr bool IsInvocable = requires( _ObjectType& a_Object, const _ValueType& a_Value )
		{
			std::invoke( _Setter, a_Object, a_Value );
		};
	public:
		// Sets the value of the field using the setter function.
		// a_Object is the object that contains the field.
		template<typename _ObjectType, typename _ValueType>
		static constexpr void SetValue( _ObjectType& a_Object, const _ValueType& a_Value )
		{
			if constexpr ( IsFieldMemberPointer )
			{
				using FieldType = std::decay_t<decltype(a_Object.*_Setter)>;
				// If the member pointer is to a Field, we need to use .Value to get the value.
				if constexpr ( IsField<FieldType> )
				{
					// If the field has a defined setter attribute, we need to call it.
					if constexpr ( FieldType::template Has<SetterAttribute>() )
					{
						(a_Object.*_Setter).SetValue( a_Object, a_Value );
					}
					else
					{
						(a_Object.*_Setter).Value = a_Value;
					}
				}
				else
				{
					a_Object.*_Setter = a_Value;
				}
			}
			else if constexpr ( IsInvocable<_ObjectType, _ValueType> )
			{
				std::invoke( _Setter, a_Object, a_Value );
			}
			else
			{
				static_assert(false, "Setter must be a member pointer or a callable that takes the object and value as arguments.");
			}
		}
	};
	template<auto _Setter>
	struct SetterAttribute<Setter<_Setter>> : std::true_type {};
	//=====================================================

#pragma endregion



	//=====================================================
	// Function Attribute
	//  A base class for all function attributes.
	struct FunctionAttribute : MetaAttribute {};



	//=====================================================
	// Is Function Attribute
	//  A concept to check if 'T' is a valid function attribute.
	template<typename T>
	concept IsFunctionAttribute = std::is_base_of_v<FunctionAttribute, T> || std::is_same_v<T, FunctionAttribute>;



	namespace Detail {

		template<auto _Function>
		struct MemberFunction
		{
			static constexpr bool IsConst = apl::is_member_fn_const_v<decltype(_Function)>;
			using ObjectType = apl::member_fn<_Function>::object_type;
			using ReturnType = apl::get_fn_return_t<decltype(_Function)>;
			using ObjectParamType = std::conditional_t<IsConst, const ObjectType&, ObjectType&>;

			template<typename... _Args>
			static constexpr auto Invoke( ObjectParamType a_Object, _Args&&... a_Args )
			{
				return std::invoke( _Function, a_Object, std::forward<_Args>( a_Args )... );
			}
		};

		template<auto _Function>
		struct StaticFunction
		{
			using ReturnType = apl::get_fn_return_t<decltype(_Function)>;
			template<typename... _Args>
			static constexpr auto Invoke( _Args&&... a_Args )
			{
				return std::invoke( _Function, std::forward<_Args>( a_Args )... );
			}
		};

	} // namespace Detail



	//=====================================================
	// Function
	//  Contains meta information about a function.
	//  Used to create reflection data for functions.
	//  If you would like to reflect a function, you must specialize CustomReflector<T> for your type.
	template<auto _Function, IsFunctionAttribute... _FunctionAttributes>
		requires (apl::is_member_fn_v<decltype(_Function)> || apl::is_static_fn_v<decltype(_Function)>)
	struct Function 
		: MetaProperty<_FunctionAttributes...>,
		  std::conditional_t<apl::is_member_fn_v<decltype(_Function)>,
				Detail::MemberFunction<_Function>,
				Detail::StaticFunction<_Function>>
	{};



	//=====================================================
	// Custom Reflector
	//  Template type that allows for custom reflection data to be created for a type.
	//  This is useful for creating custom reflection data for types that are not directly supported by the reflection system.
	//  Create a specialization of this template for your type and define the reflection data inside it.
	template<typename T>
	struct CustomReflector;



	//=====================================================
	// Has Custom Reflector
	//  A concept to check if a type has a custom reflector defined.
	template<typename T>
	concept HasCustomReflector = requires(T a_Type)
	{
		typename CustomReflector<T>;
		requires std::is_class_v<CustomReflector<T>>;
	};



	//=====================================================
	// Detail
	//  A namespace for internal reflection types and functions.
	namespace Detail {
		template<typename T, typename... _MetaAttributes>
		struct Reflect
		{
		private:
			struct StaticReflector
			{
				StaticReflector()
				{
					// Create some runtime reflection data for the type.
				}
			};

			static inline StaticReflector s_StaticReflector{};
		};
	}

#define REFLECT_TEST( _Type ) \
		private: \
			static inline const ::Tridium::Detail::Reflect<_Type> __InternalReflector{}; \
		public:
}