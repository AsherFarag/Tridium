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

	void Material::Bind()
	{
		auto diff = TextureLibrary::GetTexture( DiffuseTexture );
		if ( diff )
		{
			diff->Bind( 0 );
		}
		auto spec = TextureLibrary::GetTexture( SpecularTexture );
		if ( spec )
		{
			spec->Bind( 1 );
		}
		auto norm = TextureLibrary::GetTexture( NormalMap );
		if ( norm )
		{
			norm->Bind( 2 );
		}
		auto heig = TextureLibrary::GetTexture( HeightMap );
		if ( heig )
		{
			heig->Bind( 3 );
		}
	}

	void Material::Unbind()
	{
		auto diff = TextureLibrary::GetTexture( DiffuseTexture );
		if ( diff )
		{
			diff->Unbind( 0 );
		}
		auto spec = TextureLibrary::GetTexture( SpecularTexture );
		if ( spec )
		{
			spec->Unbind( 1 );
		}
		auto norm = TextureLibrary::GetTexture( NormalMap );
		if ( norm )
		{
			norm->Unbind( 2 );
		}
		auto heig = TextureLibrary::GetTexture( HeightMap );
		if ( heig )
		{
			heig->Unbind( 3 );
		}
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