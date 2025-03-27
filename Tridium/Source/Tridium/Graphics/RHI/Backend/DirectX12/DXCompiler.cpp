#include "tripch.h"
#include <Tridium/Core/Platform.h>
#include "DXCompiler.h"
#include "D3D12Common.h"
#include <dxcapi.h>  // DXC Compiler API
#include <d3d12shader.h>  // D3D12 Shader Reflection
#include <Tridium/Engine/Engine.h>

#pragma comment(lib, "dxcompiler.lib")

namespace Tridium {

	// Internal state for the DXC compiler.
	struct DXCInternalState
	{
		DxcCreateInstanceProc CreateInstance = nullptr;

		bool IsValid() const { return CreateInstance != nullptr; }

		DXCInternalState( const String& a_Modifier = "" )
		{
		#if CONFIG_PLATFORM_WINDOWS
			const String fileName = "dxcompiler" + a_Modifier + ".dll";
		#elif CONFIG_PLATFORM_LINUX
			const String fileName = "libdxcompiler" + a_Modifier + ".so";
		#else
			#error "Unsupported platform"
		#endif

			// Load the DXC DLL.
			Platform::Module dxcModule = Platform::LoadDynamicLibrary( fileName.c_str() );
			if ( dxcModule == nullptr )
			{
				LOG( LogCategory::DirectX, Error, "Failed to load {0}", fileName );
				return;
			}

			// Get the entry point.
			CreateInstance = reinterpret_cast<DxcCreateInstanceProc>( Platform::GetProcAddress( dxcModule, "DxcCreateInstance" ) );
			if ( CreateInstance == nullptr )
			{
				LOG( LogCategory::DirectX, Error, "Failed to get DxcCreateInstance entry point" );
				return;
			}

			// Initialize the DXC compiler.
			ComPtr<IDxcUtils> dxcUtils;
			ComPtr<IDxcCompiler3> dxcCompiler;
			if ( FAILED( CreateInstance( CLSID_DxcUtils, IID_PPV_ARGS( &dxcUtils ) ) ) )
			{
				LOG( LogCategory::DirectX, Error, "Failed to create DXC Utils" );
				return;
			}

			if ( FAILED( CreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( &dxcCompiler ) ) ) )
			{
				LOG( LogCategory::DirectX, Error, "Failed to create DXC Compiler" );
				return;
			}

		}
	};

	static DXCInternalState& GetDXCInternalState()
	{
		static DXCInternalState s_DXCInternalState;
		return s_DXCInternalState;
	}

	static DXCInternalState& GetDXCInternalState_XBOX()
	{
		static DXCInternalState s_DXCInternalState_XBOX( "_xs" );
		return s_DXCInternalState_XBOX;
	}

	const wchar_t* GetShaderModelFlag( ERHIShaderType a_Type, ERHIShaderModel a_Model );
	Optional<Array<WString>> CreateCompilerArguments( const ShaderCompilerInput& a_Input );
	void GetReflectionData( const ComPtr<IDxcBlob>& a_ReflectionBlob, ShaderCompilerOutput& a_Output );

    ShaderCompilerOutput DXCompiler::Compile( const ShaderCompilerInput& a_Input )
    {
        ShaderCompilerOutput output;

		DXCInternalState& dxcState = ( a_Input.Format == ERHIShaderFormat::HLSL6_XBOX ) ? GetDXCInternalState_XBOX() : GetDXCInternalState();

		// Check if the DXC compiler is available.
		if ( !dxcState.IsValid() )
		{
			LOG( LogCategory::DirectX, Error, "DXC compiler is not available" );
			return {};
		}

		// Create the DXC compiler.
		ComPtr<IDxcUtils> dxcUtils;
		ComPtr<IDxcCompiler3> dxcCompiler;

		if ( FAILED( dxcState.CreateInstance( CLSID_DxcUtils, IID_PPV_ARGS( &dxcUtils ) ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create DXC Utils" );
			return {};
		}

		if ( FAILED( dxcState.CreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( &dxcCompiler ) ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create DXC Compiler" );
			return {};
		}

		// Include handler
		ComPtr<IDxcIncludeHandler> includeHandler;
		if ( FAILED( dxcUtils->CreateDefaultIncludeHandler( &includeHandler ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create DXC Include Handler" );
			return {};
		}

		DxcBuffer sourceBuffer;
		sourceBuffer.Ptr = a_Input.Source.data();
		sourceBuffer.Size = a_Input.Source.size();
		sourceBuffer.Encoding = DXC_CP_UTF8;


		// Create the arguments to pass to the compiler.
		Optional<Array<WString>> args = CreateCompilerArguments( a_Input );
		if ( !args.has_value() )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create compiler arguments" );
			return {};
		}

		Array<const wchar_t*> argsRaw;
		argsRaw.Reserve( args->Size() );
		for ( const WString& arg : args.value() )
		{
			argsRaw.PushBack( arg.c_str() );
		}

		// COMPILE
		ComPtr<IDxcResult> dxcResult;
		HRESULT hr = dxcCompiler->Compile(
			&sourceBuffer,
			argsRaw.Data(),
			static_cast<uint32_t>( argsRaw.Size() ),
			includeHandler.Get(),
			IID_PPV_ARGS( &dxcResult )
		);

		// Check if compilation failed.
		if ( FAILED( hr ) )
		{
			output.Error = "Failed to compile shader";
			return output;
		}

		// Get the error( if it exists ) and shader blobs.
		ComPtr<IDxcBlobUtf8> errorBlob;
		if ( FAILED( dxcResult->GetOutput( DXC_OUT_ERRORS, IID_PPV_ARGS( &errorBlob ), nullptr ) ) )
		{
			output.Error = "Failed to get error blob";
			return output;
		}

		if ( errorBlob )
		{
			output.Error = String( errorBlob->GetStringPointer() );
		}

		ComPtr<IDxcBlob> shaderBlob;
		if ( FAILED( dxcResult->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( &shaderBlob ), nullptr ) ) )
		{
			return output;
		}

		// Get the reflection data.
		//ComPtr<IDxcBlob> reflectionBlob;
		//if ( FAILED( dxcResult->GetOutput( DXC_OUT_REFLECTION, IID_PPV_ARGS( &reflectionBlob ), nullptr ) ) )
		//{
		//	output.Error = "Failed to get reflection blob";
		//	return output;
		//}
		//GetReflectionData( reflectionBlob, output );

		// Copy the shader blob to the output.
		if ( shaderBlob && shaderBlob->GetBufferSize() > 0 )
		{
			output.ByteCode.Resize( shaderBlob->GetBufferSize() );
			std::memcpy( output.ByteCode.Data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize() );
		}

		return output;
    }

	const wchar_t* GetShaderModelFlag( ERHIShaderType a_Type, ERHIShaderModel a_Model )
	{
		switch ( a_Type )
		{
		case ERHIShaderType::Vertex:
		{
			switch ( a_Model )
			{
			default:                      return L"vs_6_0";
			case ERHIShaderModel::SM_6_1: return L"vs_6_1";
			case ERHIShaderModel::SM_6_2: return L"vs_6_2";
			case ERHIShaderModel::SM_6_3: return L"vs_6_3";
			case ERHIShaderModel::SM_6_4: return L"vs_6_4";
			case ERHIShaderModel::SM_6_5: return L"vs_6_5";
			case ERHIShaderModel::SM_6_6: return L"vs_6_6";
			}
		}
		case ERHIShaderType::Hull:
		{
			switch ( a_Model )
			{
			default:                      return L"hs_6_0";
			case ERHIShaderModel::SM_6_1: return L"hs_6_1";
			case ERHIShaderModel::SM_6_2: return L"hs_6_2";
			case ERHIShaderModel::SM_6_3: return L"hs_6_3";
			case ERHIShaderModel::SM_6_4: return L"hs_6_4";
			case ERHIShaderModel::SM_6_5: return L"hs_6_5";
			case ERHIShaderModel::SM_6_6: return L"hs_6_6";
			}
		}
		case ERHIShaderType::Domain:
		{
			switch ( a_Model )
			{
			default:                      return L"ds_6_0";
			case ERHIShaderModel::SM_6_1: return L"ds_6_1";
			case ERHIShaderModel::SM_6_2: return L"ds_6_2";
			case ERHIShaderModel::SM_6_3: return L"ds_6_3";
			case ERHIShaderModel::SM_6_4: return L"ds_6_4";
			case ERHIShaderModel::SM_6_5: return L"ds_6_5";
			case ERHIShaderModel::SM_6_6: return L"ds_6_6";
			}
		}
		case ERHIShaderType::Geometry:
		{
			switch ( a_Model )
			{
			default:                      return L"gs_6_0";
			case ERHIShaderModel::SM_6_1: return L"gs_6_1";
			case ERHIShaderModel::SM_6_2: return L"gs_6_2";
			case ERHIShaderModel::SM_6_3: return L"gs_6_3";
			case ERHIShaderModel::SM_6_4: return L"gs_6_4";
			case ERHIShaderModel::SM_6_5: return L"gs_6_5";
			case ERHIShaderModel::SM_6_6: return L"gs_6_6";
			}
		}
		case ERHIShaderType::Pixel:
		{
			switch ( a_Model )
			{
			default:                      return L"ps_6_0";
			case ERHIShaderModel::SM_6_0: return L"ps_6_0";
			case ERHIShaderModel::SM_6_1: return L"ps_6_1";
			case ERHIShaderModel::SM_6_2: return L"ps_6_2";
			case ERHIShaderModel::SM_6_3: return L"ps_6_3";
			case ERHIShaderModel::SM_6_4: return L"ps_6_4";
			case ERHIShaderModel::SM_6_5: return L"ps_6_5";
			case ERHIShaderModel::SM_6_6: return L"ps_6_6";
			}
		}
		}

		ASSERT_LOG( false, "GetShaderModelFlag: Unsupported shader type." );
		return L"";
	};

	Optional< Array<WString> > CreateCompilerArguments( const ShaderCompilerInput& a_Input )
	{
		// Arguments that will be passed to the compiler.
		Array<WString> args = {
			L"-res_may_alias",
		};

		// Set the optimization level.
		if ( a_Input.Flags.HasFlag( ERHIShaderCompilerFlags::DisableOptimization ) )
		{
			// Disable optimization flag
			args.EmplaceBack( L"-Od" );
		}
		else
		{
			// Optimization level flag
			switch ( a_Input.OptimizationLevel )
			{
				case ERHIShaderOptimizationLevel::Level1: args.EmplaceBack( L"-O1" ); break;
				case ERHIShaderOptimizationLevel::Level2: args.EmplaceBack( L"-O2" ); break;
				case ERHIShaderOptimizationLevel::Level3: args.EmplaceBack( L"-O3" ); break;
			}
		}

		// Add flags based on the shader format.
		switch ( a_Input.Format )
		{
			case ERHIShaderFormat::HLSL6:
			case ERHIShaderFormat::HLSL6_XBOX:
			{
				break;
			}
			case ERHIShaderFormat::SPIRV:
			case ERHIShaderFormat::SPIRV_OpenGL:
			{
				args.EmplaceBack( L"-spirv" );        // Target SPIR-V
				args.EmplaceBack( L"-fspv-target-env=vulkan1.1" ); // Target Vulkan 1.1
				args.EmplaceBack( L"-fvk-use-dx-layout" );         // Use DX layout
				args.EmplaceBack( L"-fvk-use-dx-position-w" );     // Use DX position.w for SV_Position

				// Pixel and Compute don't support invert-y
				if ( a_Input.ShaderType != ERHIShaderType::Pixel && a_Input.ShaderType != ERHIShaderType::Compute )
				{
					//args.EmplaceBack( L"-fvk-invert-y" ); // Make vulkan and opengl have the same coordinate system as D3D (Y-up)
				}

				args.EmplaceBack( L"-fvk-t-shift" ); args.EmplaceBack( L"1000" ); args.EmplaceBack( L"0" );
				args.EmplaceBack( L"-fvk-u-shift" ); args.EmplaceBack( L"2000" ); args.EmplaceBack( L"0" );
				args.EmplaceBack( L"-fvk-s-shift" ); args.EmplaceBack( L"3000" ); args.EmplaceBack( L"0" );
				break;
			}
			default:
			{
				ASSERT_LOG( false, "DXCompiler::Compile: Unsupported shader format." );
				return {};
			}
		}

		// Set the shader model profile.
		// Convert the shader type and model to a string for the compiler.
		// E.g. "vs_5_0" for a vertex shader with shader model 5.0.
		args.EmplaceBack( L"-T" );
		args.EmplaceBack( GetShaderModelFlag( a_Input.ShaderType, a_Input.MinimumModel ) );

		#if RHI_DEBUG_ENABLED
		if ( a_Input.Flags.HasFlag( ERHIShaderCompilerFlags::EnableDebugInfo ) )
		{
			args.EmplaceBack( L"-Zi" );
		}
		#endif

		// Disable validation
		if ( a_Input.Flags.HasFlag( ERHIShaderCompilerFlags::DisableValidation ) )
		{
			args.EmplaceBack( L"-Vd" );
		}

		// Pack matrices in column-major or row-major order.
		if ( a_Input.Flags.HasFlag( ERHIShaderCompilerFlags::RowMajor ) 
			|| a_Input.Format == ERHIShaderFormat::HLSL6
			|| a_Input.Format == ERHIShaderFormat::HLSL6_XBOX )
		{
			args.EmplaceBack( L"-Zpr" );
		}
		// Default is column-major
		else
		{
			args.EmplaceBack( L"-Zpc" );
		}

		////////////////////////////////////////
		// ENTRY POINT
		////////////////////////////////////////

		// Add the entry point.
		if ( !a_Input.EntryPoint.empty() )
		{
			args.EmplaceBack( L"-E" );
			args.EmplaceBack( a_Input.EntryPoint.begin(), a_Input.EntryPoint.end() );
		}
		else
		{
			// Default entry point
			args.EmplaceBack( L"-E" );
			args.EmplaceBack( L"main" );
		}

		////////////////////////////////////////
		// DEFINITIONS
		////////////////////////////////////////

		// Do we support combined samplers?
		args.EmplaceBack( L"-D" );
		args.EmplaceBack( L"SUPPORT_COMBINED_SAMPLERS" );
		#if RHI_SUPPORT_COMBINED_SAMPLERS
		args.EmplaceBack( L"1" );
		#else
		args.EmplaceBack( L"0" );
		#endif

		// Set up the defines specified in the input.
		for ( const auto& define : a_Input.Defines )
		{
			args.EmplaceBack( L"-D" );
			args.EmplaceBack( define.first.begin(), define.first.end() );
			args.EmplaceBack( define.second.begin(), define.second.end() );
		}

		////////////////////////////////////////
		// INCLUDE DIRECTORIES
		////////////////////////////////////////

		// Set up the include directories.
		args.EmplaceBack( L"-I" );
		TODO( "Temp" );
		FilePath shaderPath = FilePath::CurrentPath();
		shaderPath = shaderPath / "../Tridium/Shaders/Shaders";
		args.EmplaceBack( shaderPath.ToWString() );
		for ( const auto& includeDir : a_Input.IncludeDirectories )
		{
			args.EmplaceBack( L"-I" );
			args.EmplaceBack( includeDir.begin(), includeDir.end() );
		}

		////////////////////////////////////////
		// CUSTOM ARGUMENTS
		////////////////////////////////////////

		// Add custom arguments.
		for ( const auto& customArg : a_Input.CustomArguments )
		{
			args.EmplaceBack( customArg.begin(), customArg.end() );
		}

		return args;
	}

	void GetReflectionData( const ComPtr<IDxcBlob>& a_ReflectionBlob, ShaderCompilerOutput& a_Output )
	{
		ComPtr<IDxcContainerReflection> reflection;
		ComPtr<ID3D12ShaderReflection> shaderReflection;
		uint32_t shaderIndex = 0;

		// Get the reflection interface.
		if ( FAILED( DxcCreateInstance( CLSID_DxcContainerReflection, IID_PPV_ARGS( &reflection ) ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create DXC Container Reflection" );
			return;
		}

		// Load the compiled shader.
		if ( FAILED( reflection->Load( a_ReflectionBlob.Get() ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to load reflection data" );
			return;
		}

		// Find the shader reflection in the container.
		if ( FAILED( reflection->FindFirstPartKind( DXC_PART_DXIL, &shaderIndex ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to find shader reflection" );
			return;
		}

		// Get the reflection interface.
		if ( FAILED( reflection->GetPartReflection( shaderIndex, IID_PPV_ARGS( &shaderReflection ) ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to get shader reflection" );
			return;
		}

		// Get the shader description.
		D3D12_SHADER_DESC shaderDesc;
		if ( FAILED( shaderReflection->GetDesc( &shaderDesc ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to get shader description" );
			return;
		}

		// Get the input parameters.
		for ( uint32_t i = 0; i < shaderDesc.InputParameters; ++i )
		{
			D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
			if ( FAILED( shaderReflection->GetInputParameterDesc( i, &paramDesc ) ) )
			{
				LOG( LogCategory::DirectX, Error, "Failed to get input parameter description" );
				return;
			}
			LOG( LogCategory::DirectX, Debug, "Input parameter: {0}", paramDesc.SemanticName );
		}
	}

}
