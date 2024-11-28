#include "tripch.h"
#include "MeshComponent.h"
#include <Tridium/Rendering/Mesh.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( StaticMeshComponent )
		BASE( Component )
		PROPERTY( Mesh, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( Materials, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( CastShadows, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( StaticMeshComponent )

	StaticMeshComponent::StaticMeshComponent()
		: Mesh( AssetHandle::InvalidID )
	{

	}
}