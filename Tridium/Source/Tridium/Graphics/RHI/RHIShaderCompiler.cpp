#include "tripch.h"
#include "RHIShaderCompiler.h"

// Backends
#if RHI_ENABLE_SHADER_COMPILER
    #if RHI_ENABLE_SHADER_COMPILER_DX
        #include "Backend/DirectX12/DXCompiler.h"
    #endif // RHI_ENABLE_SHADER_COMPILER_DX
#endif // RHI_ENABLE_SHADER_COMPILER

namespace Tridium {

    ShaderCompilerOutput RHIShaderCompiler::Compile( const ShaderCompilerInput& a_Input )
    {
		ShaderCompilerOutput output;

        #if RHI_ENABLE_SHADER_COMPILER

        switch ( a_Input.Format )
        {

        #if RHI_ENABLE_SHADER_COMPILER_DX
            case ERHIShaderFormat::HLSL6:
            case ERHIShaderFormat::SPIRV:
            case ERHIShaderFormat::HLSL6_XBOX:
            {
                output = DXCompiler::Compile( a_Input );
				break;
            }
        #endif // RHI_ENABLE_SHADER_COMPILER_DX

            default:	
            {
				ASSERT_LOG( false, "RHIShaderCompiler::Compile: Unsupported shader format." );
                break;
            }
        }

        #endif // RHI_ENABLE_SHADER_COMPILER

		return output;
    }

} // namespace Tridium