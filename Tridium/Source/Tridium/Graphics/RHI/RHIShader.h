#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Shader Module
	//  A shader module represents a compiled shader stage.
	DEFINE_RHI_RESOURCE( ShaderModule )
	{
		StringView Name; // The name of the shader module
		ERHIShaderType Type = ERHIShaderType::Unknown; // The Shader Stage
		Span<const Byte> Bytecode; // The compiled shader binary
		StringView Source;       // The source code of the shader
	};
	//=======================================================

} // namespace Tridium