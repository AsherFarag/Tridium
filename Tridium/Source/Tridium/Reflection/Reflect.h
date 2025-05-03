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

	namespace Meta::Detail {

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
		static constexpr bool HasMeta()
		{
			return Meta::Detail::HasTrait<_Trait, _MetaAttributes...>::Value;
		}

		// Returns the first trait that matches the given template
		template<template<typename> typename _Trait> requires Meta::Detail::HasTrait<_Trait, _MetaAttributes...>::Value
		static constexpr auto GetMeta()
		{
			return Meta::Detail::GetTrait<_Trait, _MetaAttributes...>::Value();
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

	template<typename _ObjectType, auto _Getter, auto _Setter>
	struct Property : FieldAttribute
	{
		using ObjectType = _ObjectType;
		static constexpr auto Getter = _Getter;
		static constexpr auto Setter = _Setter;
	};

	template<typename _ObjectType, auto _Getter, auto _Setter>
	struct PropertyAttribute<Property<_ObjectType, _Getter, _Setter>> : std::true_type {};


	namespace Detail {

		//enum class EPropertyAccess
		//{
		//	Private,
		//	Public,
		//};

		//template<typename _ValueType, typename _PropertyType>
		//struct PropertyWrapper;

		//template<typename _ValueType>
		//struct PropertyWrapper<_ValueType, Dummy>
		//{
		//	using ObjectType = void;
		//	using ValueType = _ValueType;

		//	ValueType& Get()
		//	{
		//		return Value;
		//	}

		//	const ValueType& Get() const
		//	{
		//		return Value;
		//	}

		//	void Set( const ValueType& a_Value )
		//	{
		//		Value = a_Value;
		//	}

		//	void Set( ValueType&& a_Value )
		//	{
		//		Value = std::move( a_Value );
		//	}

		//	PropertyWrapper() = default;
		//	PropertyWrapper( const PropertyWrapper& a_Other ) : Value( a_Other.Value ) {}
		//	PropertyWrapper( PropertyWrapper&& a_Other ) : Value( std::move( a_Other.Value ) ) {}
		//	PropertyWrapper( const ValueType& a_Value ) : Value( a_Value ) {}
		//	PropertyWrapper( ValueType&& a_Value ) : Value( std::move( a_Value ) ) {}
		//	PropertyWrapper& operator=( const PropertyWrapper& a_Other ) { Value = a_Other.Value; return *this; }
		//	PropertyWrapper& operator=( PropertyWrapper&& a_Other ) { Value = std::move( a_Other.Value ); return *this; }
		//	PropertyWrapper& operator=( const ValueType& a_Value ) { Value = a_Value; return *this; }
		//	PropertyWrapper& operator=( ValueType&& a_Value ) { Value = std::move( a_Value ); return *this; }
		//	operator ValueType& () { return Value; }
		//	operator const ValueType& () const { return Value; }
		//	operator ValueType* () { return &Value; }
		//	operator const ValueType* () const { return &Value; }

		//	ValueType Value;
		//};

		//template<typename _ValueType, IsFieldAttribute... _FieldAttributes>
		//using GetPropertyWrapper = 
		//	std::conditional_t<HasTrait<PropertyAttribute, _FieldAttributes...>::Value,
		//		PropertyWrapper<_ValueType, typename GetTrait<PropertyAttribute, _FieldAttributes...>::AttributeType>,
		//		PropertyWrapper<_ValueType, Dummy>
		//	>;
	} // namespace Detail



	namespace Meta::Detail {

		enum class EPropertyAccess
		{
			Private = 0,
			Public,
		};

		enum class EPropertyAccessType
		{
			Unknown = 0,
			MemberObjectPointer,
			MemberFunctionPointer,
			PublicEnum,
			PrivateEnum
		};

		template<auto _Accessor>
		struct PropertyAccessInfo
		{
		private:
			static constexpr EPropertyAccessType GetAccessType()
			{
				if constexpr ( std::is_member_object_pointer_v<AccessorType> )
				{
					return EPropertyAccessType::MemberObjectPointer;
				}
				else if constexpr ( std::is_member_function_pointer_v<AccessorType> )
				{
					return EPropertyAccessType::MemberFunctionPointer;
				}
				else if constexpr ( std::is_same_v<std::decay_t<AccessorType>, EPropertyAccess> )
				{
					if constexpr ( _Accessor == EPropertyAccess::Private )
					{
						return EPropertyAccessType::PrivateEnum;
					}
					else if constexpr ( _Accessor == EPropertyAccess::Public )
					{
						return EPropertyAccessType::PublicEnum;
					}
					else
					{
						return EPropertyAccessType::Unknown;
					}
				}
				else
				{
					return EPropertyAccessType::Unknown;
				}
			}

		public:
			using AccessorType = decltype(_Accessor);

			// If it's a member object pointer -> MemberObjectPointer
			// If it's a member function pointer -> MemberFunctionPointer
			// If _Accessor type == EPropertyAccess
			//		If it's a EPropertyAccess::Private -> PrivateEnum
			//		If it's a EPropertyAccess::Public -> PublicEnum
			//		Otherwise -> Unknown
			// Otherwise -> Unknown
			static constexpr EPropertyAccessType AccessType = GetAccessType();
		};


		// Can be a member object pointer, member function pointer or an EPropertyAccess enum value.
		template<auto _Accessor>
		concept IsPropertyAccessor = PropertyAccessInfo<_Accessor>::AccessType != EPropertyAccessType::Unknown;

		template<typename _ObjectType, auto _Accessor>
		struct PropertyGetter
		{
			static constexpr EPropertyAccessType AccessType = PropertyAccessInfo<_Accessor>::AccessType;
			static constexpr bool IsCustom = AccessType == EPropertyAccessType::MemberFunctionPointer || AccessType == EPropertyAccessType::MemberObjectPointer;
			static constexpr bool IsConst = (AccessType != EPropertyAccessType::MemberFunctionPointer || apl::is_member_fn_const_v<decltype(_Accessor)>);

			// Non-const getter
			static constexpr decltype(auto) GetValue( _ObjectType& a_Object )
				requires IsCustom
			{
				if constexpr ( AccessType == EPropertyAccessType::MemberObjectPointer )
				{
					return a_Object.*_Accessor;
				}
				else if constexpr ( AccessType == EPropertyAccessType::MemberFunctionPointer )
				{
					return std::invoke( _Accessor, a_Object );
				}
			}

			// Const getter
			static constexpr decltype(auto) GetValue( const _ObjectType& a_Object )
				requires IsCustom&& IsConst
			{
				if constexpr ( AccessType == EPropertyAccessType::MemberObjectPointer )
				{
					return a_Object.*_Accessor;
				}
				else if constexpr ( AccessType == EPropertyAccessType::MemberFunctionPointer )
				{
					return std::invoke( _Accessor, a_Object );
				}
			}
		};

		template<EPropertyAccessType _AccessType, auto _Accessor>
		struct SetAccessorTraits
		{
			using ArgsType = void;
		};

		template<auto _Accessor>
		struct SetAccessorTraits<EPropertyAccessType::MemberObjectPointer, _Accessor>
		{
			using ArgsType = typename std::member_traits<decltype(_Accessor)>::member_type;
		};

		template<auto _Accessor>
		struct SetAccessorTraits<EPropertyAccessType::MemberFunctionPointer, _Accessor>
		{
			using ArgsType = apl::arg_t<0u, typename apl::member_fn<_Accessor>::args_type>;
		};

		template<typename _ArgType, typename _ObjectType, auto _Accessor>
		struct PropertySetterHelper
		{
			static constexpr EPropertyAccessType AccessType = PropertyAccessInfo<_Accessor>::AccessType;
			static constexpr bool IsCustom = AccessType == EPropertyAccessType::MemberFunctionPointer || AccessType == EPropertyAccessType::MemberObjectPointer;

			using ArgType = _ArgType;

			static constexpr void SetValue( _ObjectType& a_Object, ArgType a_Value ) requires !std::is_void_v<ArgType>
			{
				if constexpr ( AccessType == EPropertyAccessType::MemberObjectPointer )
				{
					a_Object.*_Accessor = a_Value;
				}
				else if constexpr ( AccessType == EPropertyAccessType::MemberFunctionPointer )
				{
					std::invoke( _Accessor, a_Object, a_Value );
				}
			}
		};

		template<typename _ObjectType, auto _Accessor>
		struct PropertySetterHelper<void, _ObjectType, _Accessor>
		{
			static constexpr EPropertyAccessType AccessType = PropertyAccessInfo<_Accessor>::AccessType;
			static constexpr bool IsCustom = AccessType == EPropertyAccessType::MemberFunctionPointer || AccessType == EPropertyAccessType::MemberObjectPointer;
			using ArgType = void;
		};

		template<typename _ObjectType, auto _Accessor>
		struct PropertySetter : PropertySetterHelper<typename SetAccessorTraits<PropertyAccessInfo<_Accessor>::AccessType, _Accessor>::ArgsType, _ObjectType, _Accessor>
		{
		};

		template<typename _ObjectType, typename _ValueType, auto _Getter, auto _Setter> requires IsPropertyAccessor<_Getter> && IsPropertyAccessor<_Setter>
		struct PropertyWrapper
		{
		protected:
			using ObjectType = _ObjectType;
			using ValueType = _ValueType;
			using Getter = PropertyGetter<ObjectType, _Getter>;
			using Setter = PropertySetter<ObjectType, _Setter>;
			using SetterArgType = std::conditional_t<Setter::IsCustom,
				typename Setter::ArgType,
				ValueType>;

			static constexpr bool RequiresObject = Getter::AccessType == EPropertyAccessType::MemberFunctionPointer || Setter::AccessType == EPropertyAccessType::MemberFunctionPointer;

			static constexpr bool HasNonConstGet = 
				( Getter::AccessType != EPropertyAccessType::MemberFunctionPointer && !Setter::IsCustom )
				|| !apl::is_member_fn_const_v<decltype(_Getter)>;
			static constexpr bool HasConstGet = Getter::IsConst;
			static constexpr bool HasPublicGet = Getter::AccessType != EPropertyAccessType::PrivateEnum;
			static constexpr bool HasPublicSet = Setter::AccessType != EPropertyAccessType::PrivateEnum;

			// Constructors

			PropertyWrapper( ObjectType* a_Object ) requires RequiresObject
				: m_Object( a_Object )
			{}

			PropertyWrapper( ObjectType* a_Object, const ValueType& a_Value ) requires RequiresObject
				: m_Object( a_Object ), Value( a_Value )
			{}

			PropertyWrapper( ObjectType* a_Object, ValueType&& a_Value ) requires RequiresObject
				: m_Object( a_Object ), Value( std::move( a_Value ) )
			{}


			PropertyWrapper() requires !RequiresObject = default;
			PropertyWrapper( const ValueType& a_Value ) requires !RequiresObject
				: Value( a_Value )
			{}

			PropertyWrapper( ValueType&& a_Value ) requires !RequiresObject
				: Value( std::move( a_Value ) )
			{}



			// Getters

			// Non-const getter on non-const object
			ValueType& Get() requires HasNonConstGet
			{
				if constexpr ( Getter::IsCustom )
				{
					return Getter::GetValue( *m_Object );
				}
				else if constexpr ( !Setter::IsCustom )
				{
					return Value;
				}
			}

			// Const Getter on non-const object
			const ValueType& Get() requires !HasConstGet && !HasNonConstGet
			{
				if constexpr ( Getter::IsCustom )
				{
					return Getter::GetValue( *m_Object );
				}
				else if constexpr ( !Setter::IsCustom )
				{
					return Value;
				}
			}

			// Const getter on const object
			const ValueType& Get() const requires HasConstGet
			{
				if constexpr ( Getter::IsCustom )
				{
					return Getter::GetValue( *m_Object );
				}
				else
				{
					return Value;
				}
			}

			// Setters

			// Copy setter
			void Set( const SetterArgType& a_Value )
			{
				if constexpr ( Setter::IsCustom )
				{
					Setter::SetValue( *m_Object, a_Value );
				}
				else
				{
					Value = a_Value;
				}
			}

			// Move setter
			void Set( SetterArgType&& a_Value )
			{
				if constexpr ( Setter::IsCustom )
				{
					Setter::SetValue( *m_Object, std::move( a_Value ) );
				}
				else
				{
					Value = std::move( a_Value );
				}
			}

		protected:
			ValueType Value;

		private:
			ObjectType* const m_Object = nullptr;
		};

		// If neither the Getter nor Setter are Member Functions/Member Objects, we can use a simpler wrapper.
		template<typename _ValueType, auto _Getter, auto _Setter>  requires IsPropertyAccessor<_Getter>&& IsPropertyAccessor<_Setter>
		struct PropertyWrapper<void, _ValueType, _Getter, _Setter>
		{
		protected:
			using ObjectType = void;
			using ValueType = _ValueType;
			using Getter = PropertyGetter<ObjectType, _Getter>;
			using Setter = PropertySetter<ObjectType, _Setter>;
			using SetterArgType = ValueType;
			static constexpr bool RequiresObject = false;
			static constexpr bool HasNonConstGet = true;
			static constexpr bool HasConstGet = true;
			static constexpr bool HasPublicGet = Getter::AccessType != EPropertyAccessType::PrivateEnum;
			static constexpr bool HasPublicSet = Setter::AccessType != EPropertyAccessType::PrivateEnum;

			PropertyWrapper() = default;

			PropertyWrapper( const ValueType& a_Value )
				: Value( a_Value )
			{}

			PropertyWrapper( ValueType&& a_Value )
				: Value( std::move( a_Value ) )
			{}

			// Getters

			// Non-const getter on non-const object
			ValueType& Get()
			{
				return Value;
			}

			// Const Getter on const object
			const ValueType& Get() const
			{
				return Value;
			}

			// Setters
			
			// Copy setter
			void Set( const SetterArgType& a_Value )
			{
				Value = a_Value;
			}

			// Move setter
			void Set( SetterArgType&& a_Value )
			{
				Value = std::move( a_Value );
			}

		protected:
			ValueType Value;
		};

		template<typename _ValueType, IsFieldAttribute... _FieldAttributes>
		struct PropertyWrapperTraits
		{
			static constexpr bool HasPropertyAttribute = HasTrait<PropertyAttribute, _FieldAttributes...>::Value;

			using ValueType = _ValueType;

			using PropertyAttribute = std::conditional_t<HasPropertyAttribute,
				typename GetTrait<PropertyAttribute, _FieldAttributes...>::AttributeType,
				void>;

			using ObjectType = typename PropertyAttribute::ObjectType;

			using PropertyWrapperType = std::conditional_t<HasPropertyAttribute,
				PropertyWrapper<ObjectType, ValueType, PropertyAttribute::Getter, PropertyAttribute::Setter>,
				PropertyWrapper<void, ValueType, EPropertyAccess::Private, EPropertyAccess::Private>>;
		};
	}

	//=====================================================
	// Field
	//  A wrapper around a value type that stores static type traits such as meta attributes.
	//  Template parameters:
	//  - _ValueType: The type of the value. For example, int, float, etc.
	//  - _FieldAttributes: A variadic template parameter that allows for multiple meta attributes to be passed in.
	template<typename _ValueType, IsFieldAttribute... _FieldAttributes>
	struct Field :
		Meta::Detail::PropertyWrapperTraits<_ValueType, _FieldAttributes...>::PropertyWrapperType,
		MetaProperty<_FieldAttributes...>
	{
	private:
		using PropertyWrapper = typename Meta::Detail::PropertyWrapperTraits<_ValueType, _FieldAttributes...>::PropertyWrapperType;
		using SetArgType = typename PropertyWrapper::SetterArgType;

		// We friend the owning object to allow access to Value.
		friend typename PropertyWrapper::ObjectType;

	public:
		using ValueType = _ValueType;

		//======================================================
		// Constructors for fields that require an object.
		//======================================================

		constexpr Field( typename PropertyWrapper::ObjectType* a_Object ) requires PropertyWrapper::RequiresObject
			: PropertyWrapper( a_Object )
		{}

		constexpr Field( typename PropertyWrapper::ObjectType* a_Object, const ValueType& a_Value ) requires PropertyWrapper::RequiresObject
			: PropertyWrapper( a_Object, a_Value )
		{}

		constexpr Field( typename PropertyWrapper::ObjectType* a_Object, ValueType&& a_Value ) requires PropertyWrapper::RequiresObject
			: PropertyWrapper( a_Object, std::move( a_Value ) )
		{}

		//======================================================
		// Constructors for fields that do not require an object.
		//======================================================

		constexpr Field() requires !PropertyWrapper::RequiresObject
			: PropertyWrapper()
		{}

		constexpr Field( const ValueType& a_Value ) requires !PropertyWrapper::RequiresObject
			: PropertyWrapper( a_Value )
		{}

		constexpr Field( ValueType&& a_Value ) requires !PropertyWrapper::RequiresObject
			: PropertyWrapper( std::move( a_Value ) )
		{}

		//======================================================
		// Public Accessors
		//======================================================

		// Getters

		decltype(auto) Get() requires PropertyWrapper::HasPublicGet && PropertyWrapper::HasNonConstGet
		{
			return PropertyWrapper::Get();
		}

		decltype(auto) Get() const requires PropertyWrapper::HasPublicGet && PropertyWrapper::HasConstGet
		{
			return PropertyWrapper::Get();
		}

		// Setters

		void Set( const SetArgType& a_Value ) requires PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( a_Value );
		}

		void Set( SetArgType&& a_Value ) requires PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( std::move( a_Value ) );
		}

		// Operator Overloads

		Field& operator=( const Field& a_Other ) requires PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( a_Other.Get() );
			return *this;
		}

		Field& operator=( const SetArgType& a_Value ) requires PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( a_Value );
			return *this;
		}

		Field& operator=( SetArgType&& a_Value ) requires PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( std::move( a_Value ) );
			return *this;
		}

		operator ValueType& () requires PropertyWrapper::HasPublicGet&& PropertyWrapper::HasNonConstGet
		{
			return PropertyWrapper::Get();
		}

		operator const ValueType& () const requires PropertyWrapper::HasPublicGet&& PropertyWrapper::HasConstGet
		{
			return PropertyWrapper::Get();
		}

	private:
		//======================================================
		// Private Accessors
		//======================================================

		// Getters

		decltype(auto) Get() requires !PropertyWrapper::HasPublicGet && PropertyWrapper::HasNonConstGet
		{
			return PropertyWrapper::Get();
		}

		decltype(auto) Get() const requires !PropertyWrapper::HasPublicGet && PropertyWrapper::HasConstGet
		{
			return PropertyWrapper::Get();
		}

		// Setters

		void Set( const SetArgType& a_Value ) requires !PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( a_Value );
		}

		void Set( SetArgType&& a_Value ) requires !PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( std::move( a_Value ) );
		}

		// Operator Overloads
		Field& operator=( const Field& a_Other ) requires !PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( a_Other.Get() );
			return *this;
		}

		Field& operator=( const SetArgType& a_Value ) requires !PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( a_Value );
			return *this;
		}

		Field& operator=( SetArgType&& a_Value ) requires !PropertyWrapper::HasPublicSet
		{
			PropertyWrapper::Set( std::move( a_Value ) );
			return *this;
		}

		operator ValueType&() requires !PropertyWrapper::HasPublicGet && PropertyWrapper::HasNonConstGet
		{
			return PropertyWrapper::Get();
		}

		operator const ValueType&() const requires !PropertyWrapper::HasPublicGet && PropertyWrapper::HasConstGet
		{
			return PropertyWrapper::Get();
		}
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

			// Allow implicit conversion to value
			operator decltype(auto)() const
			{
				return Get();
			}

			operator decltype(auto)() requires !Getter::IsMemberFnConst
			{
				return Get();
			}

			ValueType Get() requires ( Getter::IsMemberObjPointer || !apl::is_member_fn_const_v<Getter::Getter> )
			{
				return Getter::GetValue( *m_Object );
			}

			ValueType Get() const
			{
				return Getter::GetValue( *m_Object );
			}

			void Set( ArgType a_Value ) const
			{
				Setter::SetValue( *m_Object, a_Value );
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
			ObjectType* const m_Object = nullptr;
			ValueType Value;

			friend ObjectType;
		};
	}

#define REFLECT_TEST( _Type ) \
		private: \
			static inline const ::Tridium::Detail::Reflect<_Type> __InternalReflector{}; \
			using enum ::Tridium::Meta::Detail::EPropertyAccess; \
			template<auto _Getter, auto _Setter> \
			using Property = ::Tridium::Property<_Type, _Getter, _Setter>; \
		public:
}