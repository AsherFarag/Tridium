#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {

	class Shader;
	struct ShaderMetaData;

	class ShaderLoader
	{
	public:
		static Shader* Load( const fs::path& a_Path, const ShaderMetaData& a_MetaData );
	};

}