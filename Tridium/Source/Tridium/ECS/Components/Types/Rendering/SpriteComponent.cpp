#include "tripch.h"
#include "SpriteComponent.h"

namespace Tridium {

	SpriteComponent::SpriteComponent()
	{
		m_Shader = ShaderLibrary::GetShader( ShaderLibrary::GetShaderHandle( "Texture" ) );
	}
	SpriteComponent::SpriteComponent( const TextureHandle& a_Texture )
		: SpriteComponent()
	{
		m_Texture = a_Texture;
	}
}