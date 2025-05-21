#pragma once
#include <Tridium/Graphics/RHI/RHIShaderCompiler.h>

namespace Tridium::D3D12 {

	class DXCompiler final
	{
	public:
		static Expected<ShaderCompilerOutput, String> Compile( const ShaderCompilerInput& a_Input );
	};

} // namespace Tridium