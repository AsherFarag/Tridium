#pragma once
#include "AssetLoader.h"
#include <Tridium/Rendering/Mesh.h>

namespace Tridium {

	class ModelLoader
	{
		friend class ModelLoaderInterface;
	public:
		static Mesh* Load( const IO::FilePath& a_Path );

	private:
		static ModelMetaData* ConstructMetaData();
		static Mesh* RuntimeLoad( const IO::FilePath& a_Path );
		static Mesh* DebugLoad( const IO::FilePath& a_Path, const ModelMetaData* a_MetaData );
		static bool Save( const IO::FilePath& a_Path, const Mesh* a_Asset );
	};
}