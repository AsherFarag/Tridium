#include "Prefab.h"
#include <Tridium/Scene/Component.h>

#include <entt/core/hashed_string.hpp>
#include <entt/meta/resolve.hpp>

namespace Tridium {

    REGISTER_ASSET_TYPE( Prefab, AssetTypeInfo{}.SetName( "Prefab" ).SetIcon( EditorIcons::BoxArchive ) );

	//=============================================================================================
    static void RecursiveCopyEntity(
        bool a_KeepID,
        const EntityComponentRegistry& a_SrcRegistry,
        EntityComponentRegistry& a_DstRegistry,
        EntityID a_SrcEntity,
        EntityID a_DstEntity )
    {
        // Copy all components from the source entity to the destination entity
        for ( const auto& [id, srcStorage] : a_SrcRegistry.Storage() )
        {
            if ( !srcStorage.contains( a_SrcEntity ) )
                continue;

            auto dstStorage = a_DstRegistry.Storage( id );

            if ( !dstStorage )
            {
                using namespace entt::literals;
                entt::resolve( srcStorage.info() ).invoke(
                    "storage"_hs, {}, entt::forward_as_meta( a_DstRegistry.Underlying() ), id );
                dstStorage = a_DstRegistry.Storage( id );
            }

            dstStorage->push( a_DstEntity, srcStorage.value( a_SrcEntity ) );
        }

        // Recursively copy child entities if the entity has a hierarchy
        if ( a_DstRegistry.AllOf<HierarchyComponent>( a_DstEntity ) )
        {
            auto& dstHierarchy = a_DstRegistry.Get<HierarchyComponent>( a_DstEntity );
            EntityID childEntity = a_SrcRegistry.Get<HierarchyComponent>( a_SrcEntity ).FirstChild;
            EntityID prevNewChild = NullEntity;

            while ( childEntity != NullEntity )
            {
                EntityID newChildEntity = a_KeepID ? a_DstRegistry.Create( childEntity ) : a_DstRegistry.Create();
                RecursiveCopyEntity( a_KeepID, a_SrcRegistry, a_DstRegistry, childEntity, newChildEntity );

                auto& newChildHierarchy = a_DstRegistry.Get<HierarchyComponent>( newChildEntity );
                newChildHierarchy.Parent = a_DstEntity;

                // Set sibling links in the destination registry
                if ( prevNewChild != NullEntity )
                {
                    auto& prevSiblingHierarchy = a_DstRegistry.Get<HierarchyComponent>( prevNewChild );
                    prevSiblingHierarchy.NextSibling = newChildEntity;
                    newChildHierarchy.PrevSibling = prevNewChild;
                }
                else
                {
                    dstHierarchy.FirstChild = newChildEntity; // First child of the parent
                    newChildHierarchy.PrevSibling = NullEntity;
                }

                prevNewChild = newChildEntity;

                // Move to next sibling in the source
                childEntity = a_SrcRegistry.Get<HierarchyComponent>( childEntity ).NextSibling;
            }
        }
    }


    Prefab Prefab::Build( const EntityComponentRegistry& a_Registry, EntityID a_RootEntity )
    {
		Prefab prefab;

		if ( !a_Registry.Valid( a_RootEntity ) )
		{
			return prefab;
		}

		prefab.m_RootEntity = prefab.m_Registry.Create( a_RootEntity );
		RecursiveCopyEntity( true, a_Registry, prefab.m_Registry, a_RootEntity, prefab.m_RootEntity );

		return prefab;
    }

	EntityID Prefab::Instantiate( EntityComponentRegistry& a_DstRegistry ) const
	{
		if ( !Valid() )
		{
			return NullEntity;
		}

		EntityID newRootEntity = a_DstRegistry.Create( m_RootEntity );
		RecursiveCopyEntity( true, m_Registry, a_DstRegistry, m_RootEntity, newRootEntity );
		return newRootEntity;
	}

} // namespace Tridium