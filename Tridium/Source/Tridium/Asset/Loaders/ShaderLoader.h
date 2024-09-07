#pragma once
#include "AssetLoader.h"
#include <Tridium/Rendering/Shader.h>

namespace Tridium {

	class ShaderLoader
	{
		friend class ShaderLoaderInterface;
	public:
		static Shader* Load( const IO::FilePath& a_Path );

	private:
		static ShaderMetaData* ConstructMetaData();
		static Shader* RuntimeLoad( const IO::FilePath& a_Path );
		static Shader* DebugLoad( const IO::FilePath& a_Path, const ShaderMetaData* a_MetaData );
		static bool Save( const IO::FilePath& a_Path, const Shader* a_Asset );
	};

}