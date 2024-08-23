#include "tripch.h"
#include "Material.h"

#include <Tridium/IO/MaterialSerializer.h>

namespace Tridium {

	Material::Material()
		: BlendMode( EBlendMode::Additive ),
		Color( 1.0f, 1.0f, 1.0f, 1.0f )
	{

	}

	Material::Material( const AssetRef<Shader>& a_Shader )
		: Material()
	{
		m_Shader = a_Shader;
	}

	void Material::Bind()
	{
	}

	void Material::Unbind()
	{
	}

	bool Material::AddProperty( const std::string& a_Name, const Property& a_Property )
	{
		if ( m_Properties.find(a_Name) != m_Properties.end() )
			return false;

		m_Properties.insert( { a_Name, a_Property } );
		return true;
	}

	bool Material::SetProperty( const std::string& a_Name, const Property& a_Property )
	{
		auto it = m_Properties.find( a_Name );
		if (  it == m_Properties.end() )
		{
			TE_CORE_WARN( "Attempting to set non-existent property '{0}' on a material!", a_Name );
			return false;
		}

		if ( it->second.Type != a_Property.Type )
		{
			TE_CORE_WARN( "Attempting to set material property '{0}' with a different type!", a_Name );
			return false;
		}

		it->second.Value = a_Property.Value;
		return true;
	}

	Property* Material::GetProperty( const std::string& a_Name )
	{
		auto it = m_Properties.find( a_Name );
		if ( it == m_Properties.end() )
			return nullptr;

		return &( it->second );
	}

}