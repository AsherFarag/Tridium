#pragma once
#include "RHIShader.h"
#include <Tridium/Core/EnumFlags.h>

namespace Tridium {

	//=======================================================
	// RHI Shader Compiler Input
	struct ShaderCompilerInput
	{
		StringView Source{};
		ERHIShaderType ShaderType{ ERHIShaderType::Unknown };
		ERHIShaderFormat Format{ ERHIShaderFormat::Unknown };
		ERHIShaderModel MinimumModel{ ERHIShaderModel::SM_5_0 };
		EnumFlags<ERHIShaderCompilerFlags::Type> Flags{ ERHIShaderCompilerFlags::Default };
	};
	//=======================================================

	//=======================================================
	// RHI Shader Compiler Output
	struct ShaderCompilerOutput
	{
		ERHIShaderType ShaderType{ ERHIShaderType::Unknown };
		Array<Byte> ByteCode{};
		String Error{};

		bool IsValid() const { return ByteCode.Size() > 0; }
	};
	//=======================================================



	//=======================================================
	// RHI Shader Compiler
	//  Generates shader byte code for the RHI.
	//  This class is responsible for preprocessing, compiling and linking shaders.
	class RHIShaderCompiler final
	{
	public:
		static ShaderCompilerOutput Compile( const ShaderCompilerInput& a_Input );
	};
	//=======================================================

} // namespace Tridium