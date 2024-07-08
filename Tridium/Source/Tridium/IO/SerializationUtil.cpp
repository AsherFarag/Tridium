#include "tripch.h"
#include "SerializationUtil.h"

namespace Tridium {

	YAML::Emitter& operator<<( YAML::Emitter& out, const GUID& v )
	{
		out << v.ID();
		return out;
	}

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector3& v )
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector4& v )
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	TextureHandle GetTexture( const std::string& path )
	{
		TextureHandle handle;
		if ( TextureLibrary::GetTextureHandle( path, handle ) )
			return handle;

		if ( auto tex = TextureLoader::Import( path ) )
		{
			handle = TextureHandle::Create();
			tex->_SetHandle( handle );
			TextureLibrary::AddTexture( path, tex );
		}

		return handle;
	}

}