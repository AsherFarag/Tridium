#include "tripch.h"
#include "MeshComponent.h"
#include <Tridium/Reflection/Reflection.h>
#include <Tridium/Graphics/Rendering/Mesh.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( StaticMeshComponent, Scriptable )
		BASE( Component )
		PROPERTY( Mesh, Serialize | EditAnywhere )
		PROPERTY( Materials, Serialize | EditAnywhere )
		PROPERTY( CastShadows, Serialize | EditAnywhere )
	END_REFLECT_COMPONENT( StaticMeshComponent )

	StaticMeshComponent::StaticMeshComponent()
		: Mesh( AssetHandle::InvalidID )
	{

	}
}