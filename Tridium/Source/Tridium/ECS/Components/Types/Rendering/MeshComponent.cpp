#include "tripch.h"
#include "MeshComponent.h"

namespace Tridium {

	MeshComponent::MeshComponent( const AssetRef<Mesh>& mesh )
		: m_Mesh( mesh )
	{
	}
}