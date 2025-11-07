#include "Prefab.h"
#include <Tridium/Scene/Component.h>

#include <entt/core/hashed_string.hpp>
#include <entt/meta/resolve.hpp>

namespace Tridium {

    REGISTER_ASSET_TYPE( Prefab, AssetTypeInfo{}.SetName( "Prefab" ).SetIcon( EditorIcons::BoxArchive ) );


    struct TransformAccessor : TransformComponent
    {
        friend void RecursiveCopyEntity(
            bool a_KeepID,
            const EntityComponentRegistry& a_SrcRegistry,
            EntityComponentRegistry& a_DstRegistry,
            Entity a_SrcEntity,
            Entity a_DstEntity );
    };

	//=============================================================================================
    static void RecursiveCopyEntity(
        bool a_KeepID,
        const EntityComponentRegistry& a_SrcRegistry,
        EntityComponentRegistry& a_DstRegistry,
        Entity a_SrcEntity,
        Entity a_DstEntity )
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
        if ( a_DstRegistry.AllOf<TransformComponent>( a_DstEntity ) )
        {
            auto& dstTransform = (TransformAccessor&)a_DstRegistry.Get<TransformComponent>( a_DstEntity );
            Entity childEntity = ( (TransformAccessor&)a_SrcRegistry.Get<TransformComponent>( a_SrcEntity ) ).m_FirstChild;
            Entity prevNewChild = NullEntity;

            while ( childEntity != NullEntity )
            {
                Entity newChildEntity = a_KeepID ? a_DstRegistry.Create( childEntity ) : a_DstRegistry.Create();
                RecursiveCopyEntity( a_KeepID, a_SrcRegistry, a_DstRegistry, childEntity, newChildEntity );

                auto& newChildTransform = (TransformAccessor&)a_DstRegistry.Get<TransformComponent>( newChildEntity );
                newChildTransform.m_Parent = a_DstEntity;

                // Set sibling links in the destination registry
                if ( prevNewChild != NullEntity )
                {
                    auto& prevSiblingTransform = (TransformAccessor&)a_DstRegistry.Get<TransformComponent>( prevNewChild );
                    prevSiblingTransform.m_NextSibling = newChildEntity;
                    newChildTransform.m_PrevSibling = prevNewChild;
                }
                else
                {
                    dstTransform.m_FirstChild = newChildEntity; // First child of the parent
                    newChildTransform.m_PrevSibling = NullEntity;
                }

                prevNewChild = newChildEntity;

                // Move to next sibling in the source
                childEntity = ( (TransformAccessor&)a_SrcRegistry.Get<TransformComponent>( childEntity ) ).m_NextSibling;
            }
        }
    }


    Prefab Prefab::Build( const EntityComponentRegistry& a_Registry, Entity a_RootEntity )
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

	Entity Prefab::Instantiate( EntityComponentRegistry& a_DstRegistry ) const
	{
		if ( !Valid() )
		{
			return NullEntity;
		}

		Entity newRootEntity = a_DstRegistry.Create( m_RootEntity );
		RecursiveCopyEntity( true, m_Registry, a_DstRegistry, m_RootEntity, newRootEntity );
		return newRootEntity;
	}

} // namespace Tridium