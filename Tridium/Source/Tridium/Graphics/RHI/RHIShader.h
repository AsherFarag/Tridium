#pragma once
#include "RHIResource.h"

namespace Tridium {
	
	//=======================================================
	// RHI Shader Module
	//  A shader module represents a compiled shader stage.
	//=======================================================

	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHIShaderModuleDescriptor, RHIShaderModule )
	{
		static constexpr StringView s_DefaultEntryPoint = "main";

		StringView Name;                               // The name of the shader module.
		ERHIShaderType Type = ERHIShaderType::Unknown; // The Shader Stage.
		Span<const Byte> Bytecode;                     // The compiled shader binary.
		StringView Source;                             // The source code of the shader.
		StringView EntryPoint = s_DefaultEntryPoint;   // The entry point of the shader. E.g. "main"
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHIShaderModule )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIShaderModule, ERHIResourceType::ShaderModule );
		virtual bool Commit( const RHIShaderModuleDescriptor & a_Descriptor ) = 0;
	};

} // namespace Tridium