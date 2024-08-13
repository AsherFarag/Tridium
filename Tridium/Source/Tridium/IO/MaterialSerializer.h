#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Rendering/Material.h>

namespace Tridium {

	class Material;

	class MaterialSerializer
	{
	public:
		MaterialSerializer( const AssetRef<Material>& a_Material );

		void SerializeText( const std::string& a_Path );
		void SerializeBinary( const std::string& a_Path ) {}

		bool DeserializeText( const std::string& a_Path );
		bool DeserializeBinary( const std::string& a_Path ) { return false; }

	private:
		AssetRef<Material> m_Material; 
	};

}