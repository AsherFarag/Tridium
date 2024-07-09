#include "tripch.h"
#include "Material.h"

#include <Tridium/IO/MaterialSerializer.h>

namespace Tridium {

	Material::Material()
		: BlendMode( EBlendMode::Additive ),
		Color( 1.0f, 1.0f, 1.0f, 1.0f ),
		Reflectivity( 0.0f ), Refraction( 1.0f )
	{

	}

	Material::Material( const ShaderHandle& shader )
		: Material()
	{
		m_Shader = shader;
	}

	void Material::Bind()
	{
		auto col = TextureLibrary::GetTexture( BaseColorTexture );
		if ( col )
		{
			col->Bind( 0 );
		}
		auto norm = TextureLibrary::GetTexture( NormalMapTexture );
		if ( norm )
		{
			norm->Bind( 1 );
		}
		auto metal = TextureLibrary::GetTexture( MetallicTexture );
		if ( metal )
		{
			metal->Bind( 2 );
		}
		auto rough = TextureLibrary::GetTexture( RoughnessTexture );
		if ( rough )
		{
			rough->Bind( 3 );
		}
		auto emissive = TextureLibrary::GetTexture( EmissiveTexture );
		if ( emissive )
		{
			emissive->Bind( 4 );
		}
	}

	void Material::Unbind()
	{
		auto col = TextureLibrary::GetTexture( BaseColorTexture );
		if ( col )
		{
			col->Unbind( 0 );
		}
		auto norm = TextureLibrary::GetTexture( NormalMapTexture );
		if ( norm )
		{
			norm->Unbind( 1 );
		}
		auto metal = TextureLibrary::GetTexture( MetallicTexture );
		if ( metal )
		{
			metal->Unbind( 2 );
		}
		auto rough = TextureLibrary::GetTexture( RoughnessTexture );
		if ( rough )
		{
			rough->Unbind( 3 );
		}
		auto emissive = TextureLibrary::GetTexture( EmissiveTexture );
		if ( emissive )
		{
			emissive->Unbind( 4 );
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