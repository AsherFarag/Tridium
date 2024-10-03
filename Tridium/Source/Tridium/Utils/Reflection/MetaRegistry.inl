#include "MetaRegistry.h"

namespace Tridium::Refl {

	inline MetaRegistry& MetaRegistry::Get()
	{
		static MetaRegistry s_Instance;
		return s_Instance;
	}

	inline MetaType MetaRegistry::ResolveMetaType( const TypeInfo& a_TypeInfo )
	{
		return entt::resolve( a_TypeInfo );
	}

	inline MetaType MetaRegistry::ResolveMetaType( MetaIDType a_ID )
	{
		return entt::resolve( a_ID );
	}

	inline const char* MetaRegistry::GetCleanTypeName( MetaType a_MetaType )
	{
		if ( auto prop = a_MetaType.prop( Internal::CleanClassNamePropID ) )
		{
			return prop.value().cast<const char*>();
		}

		return "Unknown";
	}

	template<typename T>
	inline MetaType MetaRegistry::ResolveMetaType()
	{
		return entt::resolve<T>();
	}

	template<typename _MetaProperty>
	inline bool MetaRegistry::TryGetMetaPropertyFromClass( MetaType a_ClassID, _MetaProperty& o_Meta, MetaIDType a_MetaID )
	{
		if ( auto prop = a_ClassID.prop( a_MetaID ) )
		{
			if ( prop.value().try_cast<_MetaProperty>( ) )
			{
				o_Meta = prop.value().cast<_MetaProperty>();
				return true;
			}
		}

		return false;
	}

	template<typename _Class, typename _MetaProperty>
	inline bool MetaRegistry::TryGetMetaPropertyFromClass( _MetaProperty& o_Meta, MetaIDType a_MetaID )
	{
		static const auto meta = entt::resolve<_Class>();
		if ( auto prop = meta.prop( a_MetaID ) )
		{
			if ( prop.value().try_cast<_MetaProperty>( ) )
			{
				o_Meta = prop.value().cast<_MetaProperty>();
				return true;
			}
		}

		return false;
	}

} // namespace Tridium::Reflection