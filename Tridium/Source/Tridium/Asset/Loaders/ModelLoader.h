#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {

	class Mesh;
	struct ModelMetaData;

	class ModelLoader
	{
	public:
		static Mesh* Load( const IO::FilePath& a_Path, const ModelMetaData& a_MetaData );
	};

}