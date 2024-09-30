#include "tripch.h"
#include "MeshComponent.h"

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( MeshComponent )
		BASE( Component )
		PROPERTY( m_Mesh, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( m_Material, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( MeshComponent )

	MeshComponent::MeshComponent( const AssetRef<Mesh>& mesh )
		: m_Mesh( mesh )
	{
	}
}