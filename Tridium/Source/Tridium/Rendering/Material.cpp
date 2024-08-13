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

	Material::Material( const AssetRef<Shader>& a_Shader )
		: Material()
	{
		m_Shader = a_Shader;
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