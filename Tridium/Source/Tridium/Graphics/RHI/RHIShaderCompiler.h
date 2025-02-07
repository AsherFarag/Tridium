#pragma once
#include "RHIShader.h"

namespace Tridium {

	//======================================
	// RHI Shader Compiler
	//  Generates shader code for the RHI.
	//  This class is responsible for preprocessing, compiling and linking shaders.
	//======================================
	class RHIShaderCompiler final
	{
	public:

		RHIShaderModuleDescriptor Compile( StringView a_Source, ERHIShaderType a_ShaderType = ERHIShaderType::Unknown )
		{
			return {};
		}

	private:
		//======================================
		// Preprocessor
		//  Processes the shader source code, handling includes, macros, #if, etc.
		struct Preprocessor
		{
			String Preprocess( StringView a_Source )
			{
				return {};
			}
		};
		//======================================
	};
}