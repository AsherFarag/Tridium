#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {

	class Material;

	class MaterialSerializer
	{
	public:
		MaterialSerializer( const Ref<Material>& material );

		void SerializeText( const std::string& filepath );
		void SerializeBinary( const std::string& filepath ) {}

		bool DeserializeText( const std::string& filepath );
		bool DeserializeBinary( const std::string& filepath ) { return false; }

	private:
		Ref<Material> m_Material; 
	};

}