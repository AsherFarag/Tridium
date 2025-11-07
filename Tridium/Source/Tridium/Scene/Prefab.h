#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/ECS/Registry.h>
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
		static Prefab Build( const EntityComponentRegistry& a_Registry, Entity a_RootEntity );

		//=============================================================================================
		const EntityComponentRegistry& Registry() const { return m_Registry; }

		//=============================================================================================
		Entity Root() const { return m_RootEntity; }

		//=============================================================================================
		// Instantiates the prefab in the given destination registry.
		// Returns the Entity of the root entity in the new registry.
		Entity Instantiate( EntityComponentRegistry& a_DstRegistry ) const;

	protected:

		//=============================================================================================
		friend class PrefabBuilder;

		//=============================================================================================
		EntityComponentRegistry m_Registry{};
		Entity m_RootEntity{ NullEntity };

	};

	class PrefabBuilder
	{
	public:

		struct EntityNode
		{
			//=========================================================================================
			EntityNode() = default;
			EntityNode( PrefabBuilder* a_Builder, Entity a_Entity )
				: m_Builder( a_Builder ), m_Entity( a_Entity ) {}

			//=========================================================================================
			EntityComponentRegistry& Registry() const 
			{ 
				ASSERT( m_Builder ); 
				return m_Builder->m_Prefab.m_Registry;
			}

			//=========================================================================================
			Entity ID() const { return m_Entity; }

			//=========================================================================================
			EntityNode AddChild()
			{
				EntityNode childNode{ m_Builder, Registry().Create() };

				TransformComponent& parentTransform = Registry().GetOrEmplace<TransformComponent>( m_Entity );
				parentTransform.AddChild( Registry(), m_Entity, childNode.m_Entity );

				return childNode;
			}

			//=========================================================================================
			template<typename T, typename... _Args>
			EntityNode& AddComponent( _Args&&... a_Args )
			{
				Registry().EmplaceOrReplace<T>( m_Entity, std::forward<_Args>( a_Args )... );
				return *this;
			}

			//=========================================================================================
			template<typename T>
			T& GetComponent() const
			{
				return Registry().Get<T>( m_Entity );
			}

		private:

			//=========================================================================================
			PrefabBuilder* m_Builder = nullptr;
			Entity m_Entity{ NullEntity };

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