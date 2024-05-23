#include "tripch.h"
#include "MeshComponent.h"

namespace Tridium {

	MeshComponent::MeshComponent()
	{
		m_Shader = ShaderLibrary::GetShader( "Default" );
	}
}