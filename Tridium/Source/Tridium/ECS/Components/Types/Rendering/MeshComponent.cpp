#include "tripch.h"
#include "MeshComponent.h"

namespace Tridium {

	MeshComponent::MeshComponent()
	{
		m_Shader = ShaderLibrary::GetShader( "Default" );
	}

	void MeshComponent::SetMesh( const MeshHandle& meshHandle )
	{
		m_Mesh = meshHandle;
	}
}