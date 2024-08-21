#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {

	class Shader;
	struct ShaderMetaData;

	class ShaderLoader
	{
	public:
		static Shader* Load( const IO::FilePath& a_Path, const ShaderMetaData& a_MetaData );
	};

}