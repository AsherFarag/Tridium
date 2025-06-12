#pragma once
#include "RHIResource.h"

namespace Tridium {
	
	//=======================================================
	// RHI Shader Module
	//  A shader module represents a compiled shader stage.
	//=======================================================

	struct RHIShaderModuleDesc
	{
		using ResourceType = class IRHIShaderModule;
		ERHIShaderType Type = ERHIShaderType::Unknown; // The Shader Stage.
		Span<const Byte> Bytecode{};                     // The compiled shader binary.
		StringView Source{};                             // The source code of the shader.
		String Name{};

		constexpr auto& SetType( ERHIShaderType a_Type ) { Type = a_Type; return *this; }
		constexpr auto& SetBytecode( Span<const Byte> a_Bytecode ) { Bytecode = a_Bytecode; return *this; }
		constexpr auto& SetSource( StringView a_Source ) { Source = a_Source; return *this; }
		constexpr auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	class IRHIShaderModule : public IRHIObject
	{
		RHI_OBJECT_INTERFACE_BODY( ShaderModule )
		IRHIShaderModule( const DescriptorType& a_Desc )
			: m_Desc( a_Desc ) {}

		virtual ~IRHIShaderModule() = default;
	};

} // namespace Tridium