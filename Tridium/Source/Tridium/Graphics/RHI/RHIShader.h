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

		ERHIShaderType Type = ERHIShaderType::Unknown; // The Shader Stage.
		Span<const Byte> Bytecode;                     // The compiled shader binary.
		StringView Source;                             // The source code of the shader.
		StringView EntryPoint = s_DefaultEntryPoint;   // The entry point of the shader. E.g. "main"
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHIShaderModule )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIShaderModule, ERHIResourceType::ShaderModule );
		RHIShaderModule( const DescriptorType& a_Desc )
			: m_Desc( a_Desc ) {}
	};

} // namespace Tridium