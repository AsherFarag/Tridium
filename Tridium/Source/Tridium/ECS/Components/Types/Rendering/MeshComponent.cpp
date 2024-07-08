#include "tripch.h"
#include "MeshComponent.h"
#include <Tridium/Rendering/Shader.h>

namespace Tridium {

	MeshComponent::MeshComponent()
	{
	}

	MeshComponent::MeshComponent( const MeshHandle& meshHandle )
		: MeshComponent()
	{
		m_Mesh = meshHandle;
	}

	void MeshComponent::SetMesh( const MeshHandle& meshHandle )
	{
		m_Mesh = meshHandle;
	}

	void MeshComponent::SetMaterial( const MaterialHandle& materialHandle )
	{
		m_Material = materialHandle;
	}
}