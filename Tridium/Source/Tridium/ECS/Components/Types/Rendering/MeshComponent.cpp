#include "tripch.h"
#include "MeshComponent.h"
#include <Tridium/Reflection/Reflection.h>
#include <Tridium/Graphics/oldRendering/Mesh.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( OldStaticMeshComponent, Scriptable )
		BASE( NativeScriptComponent )
		PROPERTY( Mesh, Serialize | EditAnywhere )
		PROPERTY( Materials, Serialize | EditAnywhere )
		PROPERTY( CastShadows, Serialize | EditAnywhere )
	END_REFLECT_COMPONENT( OldStaticMeshComponent )

	OldStaticMeshComponent::OldStaticMeshComponent()
		: Mesh( OldAssetHandle::InvalidID )
	{

	}
}