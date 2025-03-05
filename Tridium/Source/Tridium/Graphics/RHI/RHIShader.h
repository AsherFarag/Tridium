#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Shader Module
	//  A shader module represents a compiled shader stage.
	DEFINE_RHI_RESOURCE( ShaderModule )
	{
		static constexpr StringView s_DefaultEntryPoint = "main";

		StringView Name;                               // The name of the shader module.
		ERHIShaderType Type = ERHIShaderType::Unknown; // The Shader Stage.
		Span<const Byte> Bytecode;                     // The compiled shader binary.
		StringView Source;                             // The source code of the shader.
		StringView EntryPoint = s_DefaultEntryPoint;   // The entry point of the shader. E.g. "main"
	};
	//=======================================================

} // namespace Tridium