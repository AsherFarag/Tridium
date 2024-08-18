#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {

	class Mesh;
	struct ModelMetaData;

	class ModelLoader
	{
	public:
		static Mesh* Load( const fs::path& a_Path, const ModelMetaData& a_MetaData );
	};

}