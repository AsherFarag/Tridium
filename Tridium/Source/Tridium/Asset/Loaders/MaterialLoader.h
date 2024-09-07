#pragma once
#include "AssetLoader.h"
#include <Tridium/Rendering/Material.h>

namespace Tridium {

	class MaterialLoader
	{
		friend class MaterialLoaderInterface;
	public:
		static Material* Load( const IO::FilePath& a_Path );

	private:
		static MaterialMetaData* ConstructMetaData();
		static Material* RuntimeLoad( const IO::FilePath& a_Path );
		static Material* DebugLoad( const IO::FilePath& a_Path, const MaterialMetaData* a_MetaData );
		static bool Save( const IO::FilePath& a_Path, const Material* a_Asset );
	};

}