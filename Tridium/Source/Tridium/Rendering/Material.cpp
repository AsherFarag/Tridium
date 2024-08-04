#include "tripch.h"
#include "Material.h"

#include <Tridium/IO/MaterialSerializer.h>

namespace Tridium {
	SharedPtr<Material> Material::Load( const std::string& path )
	{
		auto mat = MakeShared<Material>();
		MaterialSerializer serializer( mat );
		if ( serializer.DeserializeText( path ) )
			return mat;

		return nullptr;
	}

	Material::Material()
		: BlendMode( EBlendMode::Additive ),
		Color( 1.0f, 1.0f, 1.0f, 1.0f ),
		Reflectivity( 0.0f ), Refraction( 1.0f )
	{

	}

	Material::Material( const SharedPtr<Shader>& shader )
		: Material()
	{
		m_Shader = shader;
	}

	void Material::Bind()
	{
		if ( BaseColorTexture ) {
			BaseColorTexture->Bind( 0 );
		} if ( NormalMapTexture ) {
			NormalMapTexture->Bind( 1 );
		} if ( MetallicTexture ) {
			MetallicTexture->Bind( 2 );
		} if ( RoughnessTexture ) {
			RoughnessTexture->Bind( 3 );
		} if ( EmissiveTexture ) {
			EmissiveTexture->Bind( 4 );
		}
	}

	void Material::Unbind()
	{
		if ( BaseColorTexture ) {
			BaseColorTexture->Unbind( 0 );
		} if ( NormalMapTexture ) {
			NormalMapTexture->Unbind( 1 );
		} if ( MetallicTexture ) {
			MetallicTexture->Unbind( 2 );
		} if ( RoughnessTexture ) {
			RoughnessTexture->Unbind( 3 );
		} if ( EmissiveTexture ) {
			EmissiveTexture->Unbind( 4 );
		}
	}

}