#include "tripch.h"
#include "MeshComponent.h"

namespace Tridium {

	MeshComponent::MeshComponent( const SharedPtr<Mesh>& mesh )
		: m_Mesh( mesh )
	{
	}
}