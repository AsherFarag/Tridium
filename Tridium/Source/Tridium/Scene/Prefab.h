#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/ECS/ECS.h>

namespace Tridium {

	//=================================================================================================
	// Prefab Asset: An asset that contains a hierarchy of entities and their components.
	//=================================================================================================
	DEFINE_ASSET_TYPE( Prefab )
	{
	public:

		//=============================================================================================
		const EntityComponentRegistry& Registry() const { return m_Registry; }

		//=============================================================================================
		EntityID Root() const { return m_RootEntity; }

		//=============================================================================================
		bool Valid() const override
		{
			return m_Registry.Valid( m_RootEntity );
		}

		//=============================================================================================
		// Builds a Prefab from the specified root entity in the given registry.
		static Prefab Build( const EntityComponentRegistry& a_Registry, EntityID a_RootEntity );

		//=============================================================================================
		// Instantiates the prefab in the given destination registry.
		// Returns the EntityID of the root entity in the new registry.
		EntityID Instantiate( EntityComponentRegistry& a_DstRegistry ) const;

	protected:

		//=============================================================================================
		EntityComponentRegistry m_Registry{};
		EntityID m_RootEntity{ NullEntity };

	};

} // namespace Tridium