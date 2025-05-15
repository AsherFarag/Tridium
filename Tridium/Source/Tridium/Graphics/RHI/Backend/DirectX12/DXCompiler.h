#pragma once
#include <Tridium/Graphics/RHI/RHIShaderCompiler.h>

namespace Tridium::D3D12 {

	class DXCompiler final
	{
	public:
		static ShaderCompilerOutput Compile( const ShaderCompilerInput& a_Input );
	};

} // namespace Tridium