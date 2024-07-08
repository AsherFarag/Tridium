#include "tripch.h"
#include "Material.h"

#include <Tridium/IO/MaterialSerializer.h>

namespace Tridium {

	Material::Material()
		: BlendMode( EBlendMode::Additive ),
		Ambient( 0.0f, 0.0f, 0.0f, 1.0f ), Diffuse( 0.0f, 0.0f, 0.0f, 1.0f ), Specular( 1.0f, 1.0f, 1.0f, 1.0f ),
		Opacity( 1.0f ), Reflectivity( 0.0f ), Refraction( 1.0f )
	{

	}

	Material::Material( const ShaderHandle& shader )
		: Material()
	{
		m_Shader = shader;
	}

	TextureHandle Material::GetTexture( const std::string& name ) const
	{
		auto it = m_Textures.find( name );
		if ( it != m_Textures.end() )
			return it->second;

		return {};
	}

	bool Material::GetTexture( const std::string& name, TextureHandle& outTextureHandle ) const
	{
		auto it = m_Textures.find( name );
		if ( it != m_Textures.end() )
		{
			outTextureHandle = it->second;
			return true;
		}

		return false;
	}

	Ref<Material> MaterialLoader::Import( const std::string& filePath )
	{
		auto mat = MakeRef<Material>();
		MaterialSerializer serializer( mat );
		if ( serializer.DeserializeText( filePath ) )
			return mat;

		return false;
	}

}