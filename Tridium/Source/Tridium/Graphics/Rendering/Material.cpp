#include "tripch.h"
#include "Material.h"

namespace Tridium {

#if 0

	Material::Material()
		: BlendMode( EBlendMode::Additive )
	{

	}

	Material::Material( const AssetHandle& a_Shader )
		: Material()
	{
		m_Shader = a_Shader;
	}

	void Material::Bind()
	{
		//if ( !m_Shader )
		//	return;

		//for ( auto& [name, prop] : m_Properties )
		//{
		//	switch ( prop.Type )
		//	{
		//	case EPropertyType::Int:
		//	{
		//		m_Shader->SetInt( name.c_str(), std::get<int>( prop.Value ) );
		//		break;
		//	}
		//	case EPropertyType::IntArray:
		//	{
		//		auto& arr = std::get<std::vector<int>>( prop.Value );
		//		m_Shader->SetInt( name.c_str(), arr.size(), arr.data() );
		//		break;
		//	}
		//	case EPropertyType::Float:
		//	{
		//		m_Shader->SetFloat( name.c_str(), std::get<float>( prop.Value ) );
		//		break;
		//	}
		//	case EPropertyType::FloatArray:
		//	{
		//		auto& arr = std::get<std::vector<float>>( prop.Value );
		//		m_Shader->SetFloat( name.c_str(), arr.size(), arr.data() );
		//		break;
		//	}
		//	case EPropertyType::Color:
		//	{
		//		m_Shader->SetFloat4( name.c_str(), std::get<Color>( prop.Value ) );
		//		break;
		//	}
		//	case EPropertyType::ColorArray:
		//	{
		//		auto& arr = std::get<std::vector<float>>( prop.Value );
		//		m_Shader->SetFloat( name.c_str(), arr.size(), arr.data() );
		//		break;
		//	}
		//	case EPropertyType::Vector4:
		//		break;
		//	case EPropertyType::Vector4Array:
		//		break;
		//	case EPropertyType::Matrix4:
		//		break;
		//	case EPropertyType::Matrix4Array:
		//		break;
		//	case EPropertyType::Texture:
		//		break;
		//	case EPropertyType::None:
		//		break;
		//	default:
		//		break;
		//	}
		//}
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

	bool Material::RemoveProperty( const std::string& a_Name )
	{
		auto it = m_Properties.find( a_Name );
		if ( it == m_Properties.end() )
			return false;

		m_Properties.erase( it );
		return true;
	}

	//bool Material::SetProperty( const std::string& a_Name, const Property& a_Property )
	//{
	//	auto it = m_Properties.find( a_Name );
	//	if (  it == m_Properties.end() )
	//	{
	//		LOG_WARN( "Attempting to set non-existent property '{0}' on a material!", a_Name );
	//		return false;
	//	}

	//	if ( it->second.Type != a_Property.Type )
	//	{
	//		LOG_WARN( "Attempting to set material property '{0}' with a different type!", a_Name );
	//		return false;
	//	}

	//	it->second.Value = a_Property.Value;
	//	return true;
	//}

#pragma region Property Getters

#define GET_PROPERTY_BODY(type)\
	auto it = m_Properties.find( a_Name );\
	if ( it == m_Properties.end() )\
		return nullptr;\
	return std::get_if<(int)EPropertyType::type>( &it->second.Value );


	int* Material::GetInt( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( Int );
	}

	std::vector<int>* Material::GetIntVector( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( IntArray );
	}

	float* Material::GetFloat( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( Float );
	}

	std::vector<float>* Material::GetFloatArray( const std::string& a_Name ) 
	{ 
		GET_PROPERTY_BODY( FloatArray );
	}

	Color* Material::GetColor( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( Color );
	}

	std::vector<Color>* Material::GetColorArray( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( ColorArray );
	}

	Vector4* Material::GetVector4( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( Vector4 );
	}

	std::vector<Vector4>* Material::GetVector4Array( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( Vector4Array );
	}

	Matrix4* Material::GetMatrix4( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( Matrix4 );
	}

	std::vector<Matrix4>* Material::GetMatrix4Array( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( Matrix4Array );
	}

	AssetHandle* Material::GetTexture( const std::string& a_Name )
	{
		GET_PROPERTY_BODY( Texture );
	}

#undef GET_PROPERTY_BODY

#pragma endregion

#pragma region Property Setters

#define SET_PROPERTY_BODY(type)\
	auto it = m_Properties.find( a_Name );\
	if ( it == m_Properties.end() )\
	{\
		LOG_WARN( "Attempting to set non-existent property '{0}' on a material!", a_Name );\
		return false;\
	}\
	if ( it->second.Type != EPropertyType::type )\
	{\
		LOG_WARN( "Attempting to set material property '{0}' with a different type!", a_Name );\
		return false;\
	}\
	std::get<(int)EPropertyType::type>(it->second.Value) = a_Value;\
	return true;

	bool Material::SetInt( const std::string& a_Name, int a_Value )
	{
		SET_PROPERTY_BODY( Int );
	}

	bool Material::SetIntArray( const std::string& a_Name, const std::vector<int>& a_Value )
	{
		SET_PROPERTY_BODY( IntArray );
	}

	bool Material::SetFloat( const std::string& a_Name, float a_Value )
	{
		SET_PROPERTY_BODY( Float );
	}

	bool Material::SetFloatArray( const std::string& a_Name, const std::vector<float>& a_Value )
	{
		SET_PROPERTY_BODY( FloatArray );
	}

	bool Material::SetColor( const std::string& a_Name, const Color& a_Value )
	{
		SET_PROPERTY_BODY( Color );
	}

	bool Material::SetColorArray( const std::string& a_Name, const std::vector<Color>& a_Value )
	{
		SET_PROPERTY_BODY( ColorArray );
	}

	bool Material::SetVector4( const std::string& a_Name, const Vector4& a_Value )
	{
		SET_PROPERTY_BODY( Vector4 );
	}

	bool Material::SetVector4Array( const std::string& a_Name, const std::vector<Vector4>& a_Value )
	{
		SET_PROPERTY_BODY( Vector4Array );
	}

	bool Material::SetMatrix4( const std::string& a_Name, const Matrix4& a_Value )
	{
		SET_PROPERTY_BODY( Matrix4 );
	}

	bool Material::SetMatrix4Array( const std::string& a_Name, const std::vector<Matrix4>& a_Value )
	{
		SET_PROPERTY_BODY( Matrix4Array );
	}

	bool Material::SetTexture( const std::string& a_Name, const AssetHandle& a_Value )
	{
		SET_PROPERTY_BODY( Texture );
	}

#undef SET_PROPERTY_BODY

#pragma endregion

#endif
}