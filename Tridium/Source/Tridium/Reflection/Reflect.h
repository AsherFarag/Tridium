#pragma once
#include <Tridium/Core/Enum.h>
#include <type_traits>

namespace Tridium {

	enum class EFieldFlag
	{
		None = 0,
		EditAnywhere = 1 << 0,
		VisibleAnywhere = 1 << 1,
		Serialize = 1 << 2,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( EFieldFlag );

	struct MetaAttribute {};

	template<typename T>
	concept IsMetaAttribute = std::is_base_of_v<MetaAttribute, T> || std::is_same_v<T, MetaAttribute>;

	//===========================
	// Field
	//  A wrapper around a value type that stores static type traits such as meta attributes.
	template<typename _ValueType, EFieldFlag _Flags = EFieldFlag::None, IsMetaAttribute... _MetaAttributes>
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
				// Fold-expression to return first matching meta attribute
				return (Match<_MetaAttributes>(), ...);
			}
		};

	public:
		using FieldType = Field<_ValueType, _Flags, _MetaAttributes...>;
		using ValueType = _ValueType;
		static constexpr EnumFlags<EFieldFlag> Flags = _Flags;
		ValueType Value;

		template<template<typename> typename _Trait>
		static consteval bool Has()
		{
			return HasTrait<_Trait>::Value;
		}

		template<template<typename> typename _Trait>
		static consteval auto Get()
		{
			return GetTrait<_Trait>::Value();
		}
	};

	template<typename T>
	struct IsField_Trait : std::false_type {};
	template<typename _ValueType, EFieldFlag _Flags, IsMetaAttribute... _MetaAttributes>
	struct IsField_Trait<Field<_ValueType, _Flags, _MetaAttributes...>> : std::true_type {};
	template<typename T>
	concept IsField = IsField_Trait<T>::value;

	template<size_t _Min, size_t _Max>
	struct Range : MetaAttribute
	{
		static constexpr size_t Min = _Min;
		static constexpr size_t Max = _Max;
	};

	template<typename T> struct RangeAttribute : std::false_type {};
	template<size_t _Min, size_t _Max> struct RangeAttribute<Range<_Min, _Max>> : std::true_type {};

	template<typename T>
	struct StaticReflector
	{
		StaticReflector()
		{
			std::cout << "Reflecting type: " << GetTypeName<T>() << std::endl;
			T dummy;
			ForEachField( dummy,
				[]( StringView name, auto&& field )
				{
					std::cout << "Field name: " << name << std::endl;
					using FieldType = std::decay_t<decltype(field)>;
					if constexpr ( IsField<FieldType> )
					{
						std::cout << "Field type: " << GetTypeName<typename FieldType::ValueType>() << std::endl;
					}
					else
					{
						std::cout << "Field type: " << GetTypeName<FieldType>() << std::endl;
					}
				}
			);
		}
	};

	template<typename T, typename... _MetaAttributes>
	struct Reflect
	{
		using enum EFieldFlag;
	private:
		static inline StaticReflector<T> s_StaticReflector{};
	};

#define REFLECT_TEST( _Type ) \
		static inline const Reflect<_Type> s_Reflect; \
		using enum EFieldFlag
}