#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Shader Module
	//  A shader module represents a compiled shader stage.
	RHI_RESOURCE_BASE_TYPE( ShaderModule )
	{
		StringView Name; // The name of the shader module
		ERHIShaderType Type = ERHIShaderType::Unknown; // The Shader Stage
		Span<const Byte> Binary; // The compiled shader binary
		StringView Source;       // The source code of the shader
	};
	//=======================================================

} // namespace Tridium