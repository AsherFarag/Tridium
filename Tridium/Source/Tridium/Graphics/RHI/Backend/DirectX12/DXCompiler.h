#pragma once
#include <Tridium/Graphics/RHI/RHIShaderCompiler.h>

namespace Tridium {

	class DXCompiler final
	{
	public:
		static ShaderCompilerOutput Compile( const ShaderCompilerInput& a_Input );
	};

} // namespace Tridium