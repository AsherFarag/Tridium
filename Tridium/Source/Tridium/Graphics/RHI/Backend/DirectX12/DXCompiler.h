#pragma once
#include <Tridium/Graphics/RHI/RHIShaderCompiler.h>

namespace Tridium {

	class DXCompiler final
	{
	public:
		static ShaderCompilerOutput Compile( const ShaderCompilerInput& a_Input );

		// DirectX 12 requires compiled byte code for root signatures.
		static ShaderCompilerOutput CompileRootSignature( StringView s_Source, ERHIShaderFormat a_Format );
	};

} // namespace Tridium