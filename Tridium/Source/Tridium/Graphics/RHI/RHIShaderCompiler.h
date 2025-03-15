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

	enum class ERHIShaderOptimizationLevel : uint8_t
	{
		Level1,
		Level2,
		Level3,
		Default = Level3
	};



	//=======================================================
	// RHI Shader Compiler Input
	struct ShaderCompilerInput
	{
		StringView Source{};
		ERHIShaderType ShaderType                      = ERHIShaderType::Unknown;
		ERHIShaderFormat Format                        = ERHIShaderFormat::Unknown;
		ERHIShaderModel MinimumModel                   = ERHIShaderModel::SM_5_0;
		EnumFlags<ERHIShaderCompilerFlags> Flags       = ERHIShaderCompilerFlags::Default;
		ERHIShaderOptimizationLevel OptimizationLevel  = ERHIShaderOptimizationLevel::Default;

		String EntryPoint{}; // Default is "main"
		UnorderedMap<String, String> Defines{};
		Array<String> IncludeDirectories{};
		Array<String> CustomArguments{}; // Custom arguments to pass to the compiler. Note: These are directly passed to the compiler and may not be supported by all compilers.
	};

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
	// RHI Shader Compiler
	//  Generates shader byte code for the RHI.
	//  This class is responsible for preprocessing, compiling and linking shaders.
	class RHIShaderCompiler final
	{
	public:
		static ShaderCompilerOutput Compile( const ShaderCompilerInput& a_Input );
	};

} // namespace Tridium