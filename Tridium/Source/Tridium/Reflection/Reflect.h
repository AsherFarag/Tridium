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
	struct MetaAttribute 
	{
	};



	//=====================================================
	// Is Meta Attribute
	//  A concept to check if 'T' is a valid meta attribute.
	template<typename T>
	concept IsMetaAttribute = std::is_base_of_v<MetaAttribute, T> || std::is_same_v<T, MetaAttribute>;

	namespace Detail {

		struct Dummy {};

		// Check if a specific meta trait exists among the attributes
		template<template<typename> typename _Trait, IsMetaAttribute... _MetaAttributes>
		struct HasTrait
		{
			static constexpr bool Value = (_Trait<_MetaAttributes>::value || ...);
		};

		// Base case: No types left to search.
		template<template<typename> typename _Trait, typename...>
		struct GetTraitImpl
		{
			static constexpr auto Value()
			{
				return Dummy{}; // Optional fallback type
			}
		};

		// Recursive case: Check first type, then recurse
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


		// Wrapper for ease of use in the field system
		template<template<typename> typename _Trait, IsMetaAttribute... _MetaAttributes>
		struct GetTrait
		{
			static constexpr auto Value()
			{
				return GetTraitImpl<_Trait, _MetaAttributes...>::Value();
			}

			using AttributeType = decltype(Value());
		};
	}


	//=====================================================
	// Meta Property
	//  A base class for all meta properties such as functions and fields.
	template<IsMetaAttribute... _MetaAttributes>
	struct MetaProperty : public _MetaAttributes...
	{
		// Returns whether the given trait is present in the meta attributes
		template<template<typename> typename _Trait>
		static consteval bool HasMeta()
		{
			return Detail::HasTrait<_Trait>::Value;
		}

		// Returns the first trait that matches the given template
		template<template<typename> typename _Trait> requires HasTrait<_Trait>::Value
		static consteval auto GetMeta()
		{
			return Detail::GetTrait<_Trait>::Value();
		}
	};



	//=====================================================
	// Field Attribute
	//  A base class for all field attributes.
	//  Attributes that are specific to 'Field' must inherit from this class.
	struct FieldAttribute : MetaAttribute
	{
	};



	//=====================================================
	// Is Field Attribute
	//  A concept to check if 'T' is a valid field attribute.
	template<typename T>
	concept IsFieldAttribute = std::is_base_of_v<FieldAttribute, T> || std::is_same_v<T, FieldAttribute>;



	// Forward declare the Property class
	template<typename T>
	struct PropertyAttribute : std::false_type {};



	namespace Detail {

		template<typename _ValueType, typename _PropertyType>
		struct PropertyWrapper;

		template<typename _ValueType>
		struct PropertyWrapper<_ValueType, Dummy>
		{
			using ObjectType = void;
			using ValueType = _ValueType;
			// Gets the value of the field without any processing (such as from Property<Get, Set>).
			// WARNING: Avoid using this directly, as this is meant for internal use of the owning class.
			ValueType& GetInternalValue() { return m_Value; }
			// Gets the value of the field without any processing (such as from Property<Get, Set>).
			// WARNING: Avoid using this directly, as this is meant for internal use of the owning class.
			const ValueType& GetInternalValue() const { return m_Value; }

			ValueType& Get()
			{
				return m_Value;
			}

			const ValueType& Get() const
			{
				return m_Value;
			}

			void Set( const ValueType& a_Value )
			{
				m_Value = a_Value;
			}

			void Set( ValueType&& a_Value )
			{
				m_Value = std::move( a_Value );
			}

			PropertyWrapper() = default;
			PropertyWrapper( const PropertyWrapper& a_Other ) : m_Value( a_Other.m_Value ) {}
			PropertyWrapper( PropertyWrapper&& a_Other ) : m_Value( std::move( a_Other.m_Value ) ) {}
			PropertyWrapper( const ValueType& a_Value ) : m_Value( a_Value ) {}
			PropertyWrapper( ValueType&& a_Value ) : m_Value( std::move( a_Value ) ) {}
			PropertyWrapper& operator=( const PropertyWrapper& a_Other ) { m_Value = a_Other.m_Value; return *this; }
			PropertyWrapper& operator=( PropertyWrapper&& a_Other ) { m_Value = std::move( a_Other.m_Value ); return *this; }
			PropertyWrapper& operator=( const ValueType& a_Value ) { m_Value = a_Value; return *this; }
			PropertyWrapper& operator=( ValueType&& a_Value ) { m_Value = std::move( a_Value ); return *this; }
			operator ValueType& () { return m_Value; }
			operator const ValueType& () const { return m_Value; }
			operator ValueType* () { return &m_Value; }
			operator const ValueType* () const { return &m_Value; }

		private:
			ValueType m_Value;
		};

		template<typename _ValueType, IsFieldAttribute... _FieldAttributes>
		using GetPropertyWrapper = 
			std::conditional_t<HasTrait<PropertyAttribute, _FieldAttributes...>::Value,
				PropertyWrapper<_ValueType, typename GetTrait<PropertyAttribute, _FieldAttributes...>::AttributeType>,
				PropertyWrapper<_ValueType, Dummy>
			>;
	}

	//=====================================================
	// Field
	//  A wrapper around a value type that stores static type traits such as meta attributes.
	//  Template parameters:
	//  - _ValueType: The type of the value. For example, int, float, etc.
	//  - _FieldAttributes: A variadic template parameter that allows for multiple meta attributes to be passed in.
	template<typename _ValueType, IsFieldAttribute... _FieldAttributes>
	struct Field :
		Detail::GetPropertyWrapper<_ValueType, _FieldAttributes...>,
		MetaProperty<_FieldAttributes...>
	{
	private:
		static constexpr bool HasPropertyAttribute = Detail::HasTrait<PropertyAttribute, _FieldAttributes...>::Value;
		using PropertyWrapper = std::conditional_t <
			HasPropertyAttribute,
			Detail::PropertyWrapper<_ValueType, typename Detail::GetTrait<PropertyAttribute, _FieldAttributes...>::AttributeType>,
			Detail::PropertyWrapper<_ValueType, Detail::Dummy>
		>;
		using ObjectType = PropertyWrapper::ObjectType;

	public:
		using ValueType = PropertyWrapper::ValueType;

		Field& operator=( const Field& a_Other )
		{
			PropertyWrapper::Set( a_Other.Get() );
			return *this;
		}

		Field& operator=( const PropertyWrapper::ValueType& a_Value )
		{
			PropertyWrapper::Set( a_Value );
			return *this;
		}

		//======================================================
		// Defined Property Attribute
		//  These constructors are used when the field has a defined property attribute.
		//  a_Object must be a pointer to the object that owns the field. ( such as using 'this' )
		//==================================================

		constexpr Field( ObjectType* a_Object ) requires HasPropertyAttribute
			: PropertyWrapper( a_Object )
		{
		}

		constexpr Field( ObjectType* a_Object, const _ValueType& a_Value ) requires HasPropertyAttribute
			: PropertyWrapper( a_Object, a_Value )
		{
		}

		constexpr Field( ObjectType* a_Object, _ValueType&& a_Value ) requires HasPropertyAttribute
			: PropertyWrapper( a_Object, std::move( a_Value ) )
		{
		}

		//======================================================
		// No Defined Property Attribute
		//  These constructors are used when the field does not have a defined property attribute.
		//==================================================

		constexpr Field() requires !HasPropertyAttribute
			: PropertyWrapper()
		{
		}

		constexpr Field( const _ValueType& a_Value ) requires !HasPropertyAttribute
			: PropertyWrapper( a_Value )
		{
		}

		constexpr Field( _ValueType&& a_Value ) requires !HasPropertyAttribute
			: PropertyWrapper( std::move( a_Value ) )
		{
		}

		//======================================================
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

	template<auto _Getter>
		requires std::is_member_object_pointer_v<decltype(_Getter)> || apl::is_member_fn_v<decltype(_Getter)>
	struct PropertyGetter
	{
		using Getter = decltype(_Getter);
		static constexpr bool IsMemberObjPointer = std::is_member_object_pointer_v<Getter>;
		static constexpr bool IsMemberFn = apl::is_member_fn_v<Getter>;
		static constexpr bool IsMemberFnConst = apl::is_member_fn_const_v<Getter>;

		using ObjectType = std::conditional_t<IsMemberObjPointer,
			typename std::member_traits<decltype(_Getter)>::class_type,
			typename apl::member_fn<_Getter>::object_type
		>;

		using MemberType = std::conditional_t < IsMemberObjPointer,
			typename std::member_traits<decltype(_Getter)>::member_type,
			typename apl::member_fn<_Getter>::return_type
		>;

		using ReturnType = std::conditional_t < IsMemberObjPointer,
			MemberType,
			typename apl::member_fn<_Getter>::return_type
		>;

		static_assert(std::is_invocable_v<Getter, ObjectType>, "Getter is not invocable with the given object type.");

		static constexpr ReturnType GetValue( ObjectType& a_Object )
		{
			if constexpr ( IsMemberObjPointer )
			{
				using ValueType = std::decay_t<decltype(a_Object.*_Getter)>;
				// If the member pointer is to a Field, we need to use .Value to get the value.
				if constexpr ( IsField<ValueType> )
				{
					//// If the field has a defined getter attribute, we need to call it.
					//if constexpr ( ValueType::template HasMeta<GetterAttribute>() )
					//{
					//	return (a_Object.*_Getter).GetValue( a_Object );
					//}
					//else
					//{
					//	return (a_Object.*_Getter).Value;
					//}
					return (a_Object.*_Getter).Value;
				}
				else
				{
					return a_Object.*_Getter;
				}
			}
			else
			{
				// Using decltype(auto) as std::invoke can decay the return type.(e.g. int& -> int)
				return std::invoke( _Getter, a_Object );
			}
		}
	};

	template<auto _Getter, typename = void>
	struct IsGetterTrait : std::false_type {};

	template<auto _Getter>
	struct IsGetterTrait<_Getter, std::void_t<typename PropertyGetter<_Getter>::ObjectType>> : std::true_type {};

	template<auto _Getter>
	concept IsGetter = IsGetterTrait<_Getter>::value;


	template<auto _Setter>
		requires std::is_member_object_pointer_v<decltype(_Setter)> || apl::is_member_fn_v<decltype(_Setter)>
	struct PropertySetter
	{
		using Setter = decltype(_Setter);
		static constexpr bool IsMemberObjPointer = std::is_member_object_pointer_v<Setter>;
		static constexpr bool IsMemberFn = apl::is_member_fn_v<Setter>;
		static constexpr bool IsMemberFnConst = IsMemberFn ? apl::is_member_fn_const_v<Setter> : false;
		using ObjectType = std::conditional_t<IsMemberObjPointer,
			typename std::member_traits<decltype(_Setter)>::class_type,
			typename apl::member_fn<_Setter>::object_type
		>;
		using ArgType = std::conditional_t < IsMemberObjPointer,
			typename std::member_traits<decltype(_Setter)>::member_type,
			typename apl::arg_t<0u, typename apl::member_fn<_Setter>::args_type>
		>;
		using ReturnType = std::conditional_t < IsMemberObjPointer,
			void,
			typename apl::member_fn<_Setter>::return_type
		>;

		static constexpr ReturnType SetValue( ObjectType& a_Object, ArgType a_Value )
		{
			if constexpr ( IsMemberObjPointer )
			{
				using ValueType = std::decay_t<decltype(a_Object.*_Setter)>;
				// If the member pointer is to a Field, we need to use .Value to get the value.
				if constexpr ( IsField<ValueType> )
				{
					// If the field has a defined setter attribute, we need to call it.
					//if constexpr ( ValueType::template HasMeta<SetterAttribute>() )
					//{
					//	(a_Object.*_Setter).SetValue( a_Object, a_Value );
					//}
					//else
					//{
					//	(a_Object.*_Setter).Value = a_Value;
					//}
					(a_Object.*_Setter).Set( a_Value );
				}
				else
				{
					a_Object.*_Setter = a_Value;
				}
			}
			else if constexpr ( IsMemberFn )
			{
				if constexpr ( std::is_void_v<ReturnType> )
				{
					std::invoke( _Setter, a_Object, a_Value );
				}
				else
				{
					return std::invoke( _Setter, a_Object, a_Value );
				}
			}
		}
	};

	template<typename T>
	struct IsSetterTrait : std::false_type {};
	template<auto _Setter>
		requires std::is_member_object_pointer_v<decltype(_Setter)> || apl::is_member_fn_v<decltype(_Setter)>
	struct IsSetterTrait<PropertySetter<_Setter>> : std::true_type {};
	template<auto _Setter>
	concept IsSetter = IsSetterTrait<PropertySetter<_Setter>>::value;

	template<auto _Getter, auto _Setter>
		requires (std::is_same_v<typename PropertyGetter<_Getter>::ObjectType, typename PropertySetter<_Setter>::ObjectType>)
	struct Property : FieldAttribute
	{
		using Getter = Get<_Getter>;
		using Setter = Set<_Setter>;
		using ObjectType = typename Getter::ObjectType;
		using ValueType = typename Getter::ReturnType;
		using MemberType = typename Getter::MemberType;
		using ArgType = typename Setter::ArgType;
	};

	template<auto _Getter, auto _Setter>
	struct PropertyAttribute<Property<_Getter, _Setter>> : std::true_type {};

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
	{
		static constexpr bool IsMemberFn = apl::is_member_fn_v<decltype(_Function)>;
		static constexpr bool IsStaticFn = apl::is_static_fn_v<decltype(_Function)>;

		auto operator()( typename Detail::MemberFunction<_Function>::ObjectParamType a_Object, auto&&... a_Args ) const
			requires IsMemberFn
		{
			return Detail::MemberFunction<_Function>::Invoke( a_Object, std::forward<decltype(a_Args)>( a_Args )... );
		}
	};



	template<typename T>
	struct IsFunction_Trait : std::false_type {};
	template<auto _Function, IsFunctionAttribute... _FunctionAttributes>
		requires (apl::is_member_fn_v<decltype(_Function)> || apl::is_static_fn_v<decltype(_Function)>)
	struct IsFunction_Trait<Function<_Function, _FunctionAttributes...>> : std::true_type {};
	//=====================================================
	// Is Function
	//  A useful concept for checking if 'T' is a Function type.
	template<typename T>
	concept IsFunction = IsFunction_Trait<T>::value;



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

		template<typename _ValueType, typename _PropertyAttribute>
		struct PropertyWrapper : _PropertyAttribute
		{
			using Getter = typename _PropertyAttribute::Getter;
			using Setter = typename _PropertyAttribute::Setter;
			using ValueType = _ValueType;
			using ObjectType = typename _PropertyAttribute::ObjectType;
			using ArgType = typename _PropertyAttribute::ArgType;

			// Gets the value of the field without any processing (such as from Property<Get, Set>).
			// WARNING: Avoid using this directly, as this is meant for internal use of the owning class.
			ValueType& GetInternalValue() { return m_Value; }
			// Gets the value of the field without any processing (such as from Property<Get, Set>).
			// WARNING: Avoid using this directly, as this is meant for internal use of the owning class.
			const ValueType& GetInternalValue() const { return m_Value; }


			// Allow implicit conversion to value
			operator decltype(auto)() const
			{
				return Get();
			}

			operator decltype(auto)() requires !Getter::IsMemberFnConst
			{
				return Get();
			}

			// Compound assignment operators
			PropertyWrapper& operator+=( const ValueType & a_Rhs ) { Set( Get() + a_Rhs ); return *this; }
			PropertyWrapper& operator-=( const ValueType & a_Rhs ) { Set( Get() - a_Rhs ); return *this; }
			PropertyWrapper& operator*=( const ValueType & a_Rhs ) { Set( Get() * a_Rhs ); return *this; }
			PropertyWrapper& operator/=( const ValueType & a_Rhs ) { Set( Get() / a_Rhs ); return *this; }

			// Pre/post increment & decrement
			PropertyWrapper& operator++() { Set( Get() + 1 ); return *this; }
			PropertyWrapper operator++( int ) { ValueType temp = Get(); Set( temp + 1 ); return temp; }

			PropertyWrapper& operator--() { Set( Get() - 1 ); return *this; }
			PropertyWrapper operator--( int ) { ValueType temp = Get(); Set( temp - 1 ); return temp; }

			ValueType Get() requires ( Getter::IsMemberObjPointer || !apl::is_member_fn_const_v<Getter::Getter> )
			{
				return Getter::GetValue( *m_Object );
			}

			ValueType Get() const
			{
				return Getter::GetValue( *m_Object );
			}

			void Set( ArgType value ) const
			{
				Setter::SetValue( *m_Object, value );
			}

			// Constructors

			constexpr PropertyWrapper( ObjectType* a_Object )
				: m_Object( a_Object ) 
			{}

			constexpr PropertyWrapper( ObjectType* a_Object, const ValueType& a_Value )
				: m_Object( a_Object )
			{
				Set( a_Value );
			}

			constexpr PropertyWrapper( ObjectType* a_Object, ValueType&& a_Value )
				: m_Object( a_Object )
			{
				Set( std::move( a_Value ) );
			}

		private:
			ObjectType* m_Object = nullptr;
			ValueType m_Value;
		};
	}

#define REFLECT_TEST( _Type ) \
		private: \
			static inline const ::Tridium::Detail::Reflect<_Type> __InternalReflector{}; \
		public:
}