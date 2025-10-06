#pragma once
#include <Tridium/Core/Enum.h>
#include <Tridium/Graphics/RHI/RHIDefinitions.h>
#include <Tridium/Graphics/RHI/RHIShader.h>

namespace Tridium {

	//=================================================================================================
	// RHI Shader Compiler Flags:
	// Defines flags that can be used to modify the behavior of the shader compiler.
	//=================================================================================================
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
	DEFINE_ENUM_BITMASK_OPERATORS( ERHIShaderCompilerFlags );

	//=================================================================================================
	// RHI Shader Optimization Level:
	// Defines the level of optimization to be used when compiling the shader.
	// Should be kept on Default, unless the shader code is required to be debugged.
	//=================================================================================================
	enum class ERHIShaderOptimizationLevel : uint8_t
	{
		Level1,
		Level2,
		Level3,
		Default = Level3
	};

	//=================================================================================================
	// Shader Reflection Binding: Describes a single resource binding in the shader.
	// This can be used to create an RHIShaderBinding.
	//=================================================================================================
	struct ShaderReflectionBinding
	{
		enum EType
		{
			Unknown = 0,
			ConstantBuffer,
			StructuredBuffer,
			StorageBuffer,
			Texture,
			Sampler,
		};

		String Name{};
		EType Type = EType::Unknown;
		uint32_t Slot = 0;
		uint32_t Space = 0;
		uint32_t Count = 1;
		uint32_t Size = 0; // Size in bytes. Only valid for constant buffers and inlined constants.
		ERHITextureDimension TextureDimension = ERHITextureDimension::Unknown; // Only valid for textures
	};

	//=================================================================================================
	// Shader Reflection Data: Contains reflection information about the shader.
	//=================================================================================================
	struct ShaderReflectionData
	{
		Array<ShaderReflectionBinding> Bindings{};
	};

	//=================================================================================================
	// RHI Shader Compiler Input:
	// Contains input parameters and the text source-code for the shader compiler.
	//=================================================================================================
	struct ShaderCompilerInput
	{
		//=============================================================================================
		// The shader source code to compile.
		StringView Source{};

		//=============================================================================================
		// The type of shader to compile.
		// If set to Unknown, the compiler will attempt to determine the type from the entry point.
		ERHIShaderType ShaderType = ERHIShaderType::Unknown;

		//=============================================================================================
		// The format the shader should be compiled to.
		ERHIShaderFormat Format = ERHIShaderFormat::Unknown;

		//=============================================================================================
		// The minimum shader model to compile for.
		ERHIShaderModel MinimumModel = ERHIShaderModel::SM_5_0;

		//=============================================================================================
		// Unique flags that modify the behavior of the compiler.
		EnumFlags<ERHIShaderCompilerFlags> Flags = ERHIShaderCompilerFlags::Default;

		//=============================================================================================
		// The optimization level to use when compiling the shader.
		ERHIShaderOptimizationLevel OptimizationLevel = ERHIShaderOptimizationLevel::Default;

		//=============================================================================================
		// If the ShaderType is set to unknown, the EntryPoint must follow the standard of <ShaderType>Main
		// E.g. "VSMain" for a vertex shader, "PSMain" for a pixel shader, etc.
		String EntryPoint{};

		//=============================================================================================
		// Preprocessor definitions and include directories.
		// e.g. Defines = { "USE_NORMAL_MAPS": "1", "MY_OTHER_DEFINE": "SomeValue" }
		UnorderedMap<String, String> Defines{};

		//=============================================================================================
		// Additional directories to search for include files.
		Array<String> IncludeDirectories{};

		//=============================================================================================
		// Custom arguments to pass to the compiler.
		// Note: These are directly passed to the compiler and may not be supported by all compilers.
		Array<String> CustomArguments{};

		//=============================================================================================
		// If true, the default arguments will be ignored and only the custom arguments will be used.
		bool OverrideDefaultArguments = false;

		//=============================================================================================
		// If true, the compiler will generate reflection data for the shader.
		// This can be obtained from ShaderCompilerOutput::Reflection.
		bool GenerateReflectionData = false;
	};

	//=================================================================================================
	// RHI Shader Compiler Output:
	//=================================================================================================
	struct ShaderCompilerOutput
	{
		//=============================================================================================
		// If the ShaderType from the compiler-input is set to Unknown,
		// this will be the type the compiler was able to determine.
		// Otherwise, this will be the same as the input type.
		ERHIShaderType ShaderType = ERHIShaderType::Unknown;

		//=============================================================================================
		// The compiled shader byte code.
		Array<byte_t> ByteCode{};

		//=============================================================================================
		// Reflection data generated by the compiler.
		// This will only be populated if the input 'GenerateReflectionData' is true.
		ShaderReflectionData Reflection{};
	};

	//=================================================================================================
	// RHI Shader Compiler:
	// Generates shader byte code for the RHI.
	// This class is responsible for preprocessing and compiling string source code into byte code.
	//=================================================================================================
	class RHIShaderCompiler final
	{
	public:
		static Expected<ShaderCompilerOutput, String> Compile( const ShaderCompilerInput& a_Input );
	};

} // namespace Tridium