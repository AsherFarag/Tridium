#pragma once
#include <unordered_map>
#include "entt.hpp"
#include "Meta.h"
#include <Tridium/IO/TextSerializer.h>

namespace Tridium::Refl {

	namespace Internal {

		// Serialize function signature typedef.
		typedef void ( *SerializeFunc )( IO::Archive& a_Archive, const MetaAny& a_Data );

		// ID for the serialize function.
		constexpr MetaIDType YAMLSerializeFuncID = entt::hashed_string( "YAMLSerializeFuncID" ).value();
		constexpr MetaIDType CleanClassNamePropID = entt::hashed_string( "CleanClassName" ).value();
	}

	class MetaRegistry final
	{
	public:
		static MetaRegistry& Get();

		static inline MetaType ResolveMetaType( const TypeInfo& a_TypeInfo );

		static inline MetaType ResolveMetaType( MetaIDType a_ID );

		static inline const char* GetCleanTypeName( MetaType a_MetaType );

		template<typename T>
		static MetaType ResolveMetaType();

		template<typename _MetaProperty>
		static bool TryGetMetaPropertyFromClass( MetaType a_ClassID, _MetaProperty& o_Meta, MetaIDType a_MetaID );

		template<typename _Class, typename _MetaProperty>
		static bool TryGetMetaPropertyFromClass( _MetaProperty& o_Meta, MetaIDType a_MetaID );

	private:
		MetaRegistry() = default;
		MetaRegistry( const MetaRegistry& ) = delete;
		MetaRegistry& operator=( const MetaRegistry& ) = delete;
	};

}

#include "MetaRegistry.inl"