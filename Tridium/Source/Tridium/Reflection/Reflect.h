#pragma once
#include <Tridium/Core/Enum.h>
#include <type_traits>

namespace Tridium {

	enum class EFieldFlags
	{
		None = 0,
		EditAnywhere = 1 << 0,
		VisibleAnywhere = 1 << 1,
		Serialize = 1 << 2,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( EFieldFlags );

	// False if has EditAnywhere and VisibleAnywhere
	template<EFieldFlags _Flags>
	concept IsValidFieldFlags = requires (EFieldFlags _Flags)
	{
		requires EnumFlags( _Flags ).HasFlag( EFieldFlags::EditAnywhere ) || EnumFlags( _Flags ).HasFlag( EFieldFlags::VisibleAnywhere );
	};

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
	// Field
	//  A wrapper around a value type that stores static type traits such as meta attributes.
	//  Template parameters:
	//  - _ValueType: The type of the value. For example, int, float, etc.
	//  - _Flags: The flags for the field, must be from EFieldFlags. For example, EditAnywhere, VisibleAnywhere, Serialize, etc.
	//  - _MetaAttributes: A variadic template parameter that allows for multiple meta attributes to be passed in.
	template<typename _ValueType, EFieldFlags _Flags = EFieldFlags::None, IsMetaAttribute... _MetaAttributes>
	struct Field : _MetaAttributes...
	{
	private:
		template<template<typename> typename _Trait>
		struct HasTrait
		{
			static constexpr bool Value = (_Trait<_MetaAttributes>::value || ...);
		};

		template<template<typename> typename _Trait>
		struct GetTrait
		{
			template<typename T>
			static consteval auto Match()
			{
				if constexpr ( _Trait<T>::value )
					return T{};
				else
					return nullptr;
			}

			static consteval auto Value()
			{
				return (Match<_MetaAttributes>(), ...);
			}
		};

	public:
		using FieldType = Field<_ValueType, _Flags, _MetaAttributes...>;
		using ValueType = _ValueType;
		static constexpr EnumFlags<EFieldFlags> Flags = _Flags;
		ValueType Value;

		template<template<typename> typename _Trait>
		static consteval bool Has()
		{
			return HasTrait<_Trait>::Value;
		}

		template<template<typename> typename _Trait> requires HasTrait<_Trait>::Value
		static consteval auto Get()
		{
			return GetTrait<_Trait>::Value();
		}
	};

	template<typename T>
	struct IsField_Trait : std::false_type {};
	template<typename _ValueType, EFieldFlags _Flags, IsMetaAttribute... _MetaAttributes>
	struct IsField_Trait<Field<_ValueType, _Flags, _MetaAttributes...>> : std::true_type {};
	//=====================================================
	// Is Field
	//  A useful concept for checking if 'T' is a Field type.
	template<typename T>
	concept IsField = IsField_Trait<T>::value;

	//////////////////////////////////////////////////////////////////
	// Meta Attributes
	//////////////////////////////////////////////////////////////////

	//=====================================================
	// Range - Editor Only
	//  Describes a valid range for a field.
	//  In the editor, this field will show a slider for the range.
	template<auto _Min, auto _Max>
		requires (std::is_arithmetic_v<decltype(_Min)>&& std::is_arithmetic_v<decltype(_Max)>)
			  && (_Min < _Max)
	struct Range : MetaAttribute
	{
		static constexpr auto Min = _Min;
		static constexpr auto Max = _Max;
	};
	template<typename T> struct RangeAttribute : std::false_type {};
	template<auto _Min, auto _Max> struct RangeAttribute<Range<_Min, _Max>> : std::true_type {};

	template<typename T>
	struct StaticReflector
	{
		StaticReflector()
		{
			// Create some runtime reflection data for the type.
		}
	};

	template<typename T, typename... _MetaAttributes>
	struct Reflect
	{
		using enum EFieldFlags;
	private:
		static inline StaticReflector<T> s_StaticReflector{};
	};

#define REFLECT_TEST( _Type ) \
		static inline const Reflect<_Type> s_Reflect; \
		using enum EFieldFlags
}