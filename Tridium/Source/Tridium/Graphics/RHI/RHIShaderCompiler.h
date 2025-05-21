#pragma once
#include "RHIShader.h"
#include <Tridium/Core/Enum.h>

namespace Tridium {

	//=======================================================
	// RHI Shader Compiler Flags
	//  Defines flags that can be used to modify the behavior of the shader compiler.
	enum class ERHIShaderCompilerFlags : uint32_t
	{
		None = 0,
		RowMajor = 1 << 1,              // Pack matrices in row-major order ( Note: Default is column-major )
		DisableOptimization = 1 << 2,   // Disable optimization
		NoWarnings = 1 << 3,            // Disable warnings
		TreatWarningsAsErrors = 1 << 4, // Treat warnings as errors
		EnableDebugInfo = 1 << 5,       // Enable debug information ( Note: This is always disabled in release builds )
		DisableValidation = 1 << 6,     // Disable validation ( Note: This is always disabled in release builds )

		Default = None
	};
	ENUM_ENABLE_BITMASK_OPERATORS( ERHIShaderCompilerFlags );



	//=======================================================
	// RHI Shader Optimization Level
	//  Defines the level of optimization to be used when compiling the shader.
	//  Should be kept on Default, unless the shader code is required to be debugged.
	enum class ERHIShaderOptimizationLevel : uint8_t
	{
		Level1,
		Level2,
		Level3,
		Default = Level3
	};



	//=======================================================
	// RHI Shader Compiler Input
	//  Contains input parameters and the text source-code for the shader compiler.
	struct ShaderCompilerInput
	{
		StringView Source{};
		ERHIShaderType ShaderType                      = ERHIShaderType::Unknown;
		ERHIShaderFormat Format                        = ERHIShaderFormat::Unknown;
		ERHIShaderModel MinimumModel                   = ERHIShaderModel::SM_5_0;
		EnumFlags<ERHIShaderCompilerFlags> Flags       = ERHIShaderCompilerFlags::Default;
		ERHIShaderOptimizationLevel OptimizationLevel  = ERHIShaderOptimizationLevel::Default;

		// If the ShaderType is set to unknown, the EntryPoint must follow the standard of <ShaderType>Main
		// E.g. "VSMain" for a vertex shader, "PSMain" for a pixel shader, etc.
		String EntryPoint{};
		UnorderedMap<String, String> Defines{};
		Array<String> IncludeDirectories{};
		Array<String> CustomArguments{}; // Custom arguments to pass to the compiler. Note: These are directly passed to the compiler and may not be supported by all compilers.
		bool OverrideDefaultArguments = false; // If true, the default arguments will be ignored and only the custom arguments will be used.
	};

	//=======================================================
	// RHI Shader Compiler Output
	//  
	struct ShaderCompilerOutput
	{
		// If the ShaderType from the compiler-input is set to Unknown, this will be the type the compiler was able to determine.
		// Otherwise, this will be the same as the input type.
		ERHIShaderType ShaderType = ERHIShaderType::Unknown;
		Array<Byte> ByteCode{};
	};



	//=======================================================
	// RHI Shader Compiler
	//  Generates shader byte code for the RHI.
	//  This class is responsible for preprocessing and compiling string source code into byte code.
	class RHIShaderCompiler final
	{
	public:
		static Expected<ShaderCompilerOutput, String> Compile( const ShaderCompilerInput& a_Input );
	};

} // namespace Tridium