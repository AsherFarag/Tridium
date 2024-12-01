#pragma once
#include <unordered_map>
#include "entt.hpp"
#include "Meta.h"

namespace Tridium::Refl {

	class MetaRegistry final
	{
	public:
		static MetaRegistry& Get();

		static inline MetaType ResolveMetaType( const TypeInfo& a_TypeInfo );

		static inline MetaType ResolveMetaType( MetaIDType a_ID );

		static inline const char* GetCleanTypeName( const MetaType& a_MetaType );

		template<typename T>
		static MetaType ResolveMetaType();

		template<typename _MetaProperty>
		static bool TryGetMetaPropertyFromClass( const MetaType& a_ClassMetaType, _MetaProperty& o_Meta, MetaIDType a_MetaID );

		template<typename _Class, typename _MetaProperty>
		static bool TryGetMetaPropertyFromClass( _MetaProperty& o_Meta, MetaIDType a_MetaID );

	private:
		MetaRegistry() = default;
		MetaRegistry( const MetaRegistry& ) = delete;
		MetaRegistry& operator=( const MetaRegistry& ) = delete;
	};

}

#include "MetaRegistry.inl"