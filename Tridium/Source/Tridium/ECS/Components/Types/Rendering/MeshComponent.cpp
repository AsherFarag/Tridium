#include "tripch.h"
#include "MeshComponent.h"

namespace Tridium {

	MeshComponent::MeshComponent()
	{
		m_Shader = ShaderLibrary::GetShader( "Default" );
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
}