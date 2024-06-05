#include "tripch.h"
#include "SpriteComponent.h"
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	SpriteComponent::SpriteComponent()
	{
		m_Shader = ShaderLibrary::GetShader( "Texture" );
	}
	SpriteComponent::SpriteComponent( const Ref<Texture2D>& a_Texture )
		: m_Texture( a_Texture )
	{
		m_Shader = ShaderLibrary::GetShader( "Texture" );
	}

	SpriteComponent::SpriteComponent( const std::string& a_TexturePath )
		: SpriteComponent( Texture2D::Create( a_TexturePath ) )
	{
	}
}