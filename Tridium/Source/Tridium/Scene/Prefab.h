#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/ECS/ECS.h>
#include <Tridium/Scene/Component.h>

namespace Tridium {

	//=================================================================================================
	// Prefab Asset: An asset that contains a hierarchy of entities and their components.
	//=================================================================================================
	DEFINE_ASSET_TYPE( Prefab )
	{
	public:

		//=============================================================================================
		bool Valid() const override { return m_Registry.Valid( m_RootEntity ); }

		//=============================================================================================
		// Builds a Prefab from the specified root entity in the given registry.
		static Prefab Build( const EntityComponentRegistry& a_Registry, EntityID a_RootEntity );

		//=============================================================================================
		const EntityComponentRegistry& Registry() const { return m_Registry; }

		//=============================================================================================
		EntityID Root() const { return m_RootEntity; }

		//=============================================================================================
		// Instantiates the prefab in the given destination registry.
		// Returns the EntityID of the root entity in the new registry.
		EntityID Instantiate( EntityComponentRegistry& a_DstRegistry ) const;

	protected:

		//=============================================================================================
		friend class PrefabBuilder;

		//=============================================================================================
		EntityComponentRegistry m_Registry{};
		EntityID m_RootEntity{ NullEntity };

	};

	class PrefabBuilder
	{
	public:

		struct EntityNode
		{
			//=========================================================================================
			EntityNode() = default;
			EntityNode( PrefabBuilder* a_Builder, EntityID a_EntityID )
				: m_Builder( a_Builder ), m_EntityID( a_EntityID ) {}

			//=========================================================================================
			EntityComponentRegistry& Registry() const 
			{ 
				ASSERT( m_Builder ); 
				return m_Builder->m_Prefab.m_Registry;
			}

			//=========================================================================================
			EntityID ID() const { return m_EntityID; }

			//=========================================================================================
			EntityNode AddChild()
			{
				EntityNode childNode{ m_Builder, Registry().Create() };

				HierarchyComponent& parentHierarchy = Registry().GetOrEmplace<HierarchyComponent>( m_EntityID );
				parentHierarchy.AddChild( Registry(), m_EntityID, childNode.m_EntityID );

				return childNode;
			}

			//=========================================================================================
			template<typename T, typename... _Args>
			EntityNode& AddComponent( _Args&&... a_Args )
			{
				Registry().EmplaceOrReplace<T>( m_EntityID, std::forward<_Args>( a_Args )... );
				return *this;
			}

			//=========================================================================================
			template<typename T>
			T& GetComponent() const
			{
				return Registry().Get<T>( m_EntityID );
			}

		private:

			//=========================================================================================
			PrefabBuilder* m_Builder = nullptr;
			EntityID m_EntityID{ NullEntity };

		};

		//=============================================================================================
		Prefab Build()
		{
			return std::move( m_Prefab );
		}

		//=============================================================================================
		EntityNode CreateRoot()
		{
			if ( m_Prefab.m_RootEntity != NullEntity )
			{
				ASSERT( false, "Root entity has already been created." );
				return EntityNode{};
			}

			m_Prefab.m_RootEntity = m_Prefab.m_Registry.Create();
			return { this, m_Prefab.m_RootEntity };
		}

	private:

		//=============================================================================================
		friend EntityNode;
		Prefab m_Prefab{};

	};

} // namespace Tridium