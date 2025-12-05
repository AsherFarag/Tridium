#include "tripch.h"
#include <Tridium/Core/Platform.h>
#include <Tridium/Engine/Engine.h>
#include <Tridium/Graphics/RHI/Backend/DirectX12/RHI_D3D12Impl.h>
#include <Tridium/Graphics/RHI/Backend/DirectX12/DXCompiler.h>
#include <Tridium/Graphics/RHI/Backend/DirectX12/D3D12.h>

#include <dxcapi.h>  // DXC Compiler API
#include <d3d12shader.h>  // D3D12 Shader Reflection
#include <SPIRV-Cross/spirv_cross.hpp>
#include <SPIRV-Cross/spirv_glsl.hpp>

#pragma comment(lib, "dxcompiler.lib")

namespace Tridium::D3D12 {

	// Internal state for the DXC compiler.
	struct DXCInternalState
	{
		DxcCreateInstanceProc CreateInstance = nullptr;

		bool Valid() const { return CreateInstance != nullptr; }

		DXCInternalState( const String& a_Modifier = "" )
		{
			TODO( "Hacky. How should we decide and get the directory?" );
		#if CONFIG_SHIPPING
			const FilePath pathToDXC = ""; // Shipping builds should have DXC packaged in the same directory as the executable.
		#else
			const FilePath pathToDXC = "../Dependencies/dxc/bin/x64";
		#endif

		#if CONFIG_PLATFORM_WINDOWS
			const String fileName = "dxcompiler" + a_Modifier + ".dll";
		#elif CONFIG_PLATFORM_LINUX
			const String fileName = "libdxcompiler" + a_Modifier + ".so";
		#else
			#error "Unsupported platform"
		#endif

			// Load the DXC DLL.
			Platform::Module dxcModule = Platform::LoadDynamicLibrary( ( pathToDXC / fileName ).ToString().c_str() );
			if ( dxcModule == nullptr )
			{
				LOG( LogCategory::DirectX, Error, "Failed to load {0}", fileName );
				return;
			}

			// Get the entry point.
			CreateInstance = ReinterpretCast<DxcCreateInstanceProc>( Platform::GetProcAddress( dxcModule, "DxcCreateInstance" ) );
			if ( CreateInstance == nullptr )
			{
				LOG( LogCategory::DirectX, Error, "Failed to get DxcCreateInstance entry point" );
				return;
			}

			// Initialize the DXC compiler.
			ComPtr<IDxcUtils> dxcUtils;
			ComPtr<IDxcCompiler3> dxcCompiler;
			if ( FAILED( CreateInstance( CLSID_DxcUtils, IID_PPV_ARGS( dxcUtils.GetAddressOf() ) ) ) )
			{
				LOG( LogCategory::DirectX, Error, "Failed to create DXC Utils" );
				return;
			}

			if ( FAILED( CreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( dxcCompiler.GetAddressOf() ) ) ) )
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

	Expected<Pair<ERHIShaderType, StringView>, String> GetShaderTypeAndEntryPoint( const ShaderCompilerInput& a_Input );
	WStringView GetShaderModelFlag( ERHIShaderType a_Type, ERHIShaderModel a_Model );
	Expected<Array<WString>, String> CreateCompilerArguments( const ShaderCompilerInput& a_Input, ERHIShaderType a_ShaderType, StringView a_EntryPoint );
	Expected<void, String> SpirVPostProcess( const ShaderCompilerInput& a_Input, ShaderCompilerOutput& a_Output );

	static bool IsHLSLFormat( ERHIShaderFormat a_Format )
	{
		return a_Format == ERHIShaderFormat::HLSL6 || a_Format == ERHIShaderFormat::HLSL6_XBOX;
	}

	static bool IsSPIRVFormat( ERHIShaderFormat a_Format )
	{
		return a_Format == ERHIShaderFormat::SPIRV || a_Format == ERHIShaderFormat::SPIRV_OpenGL;
	}

    Expected<ShaderCompilerOutput, String> DXCompiler::Compile( const ShaderCompilerInput& a_Input )
    {
		auto typeAndEntryPoint = GetShaderTypeAndEntryPoint( a_Input );
		if ( typeAndEntryPoint.IsError() )
		{
			return Unexpected( std::format( "Failed to get shader type and entry point because '{}'", typeAndEntryPoint.Error() ) );
		}

		const auto [shaderType, entryPoint] = typeAndEntryPoint.Value();

		DXCInternalState& dxcState = ( a_Input.Format == ERHIShaderFormat::HLSL6_XBOX ) ? GetDXCInternalState_XBOX() : GetDXCInternalState();

		// Check if the DXC compiler is available.
		if ( !dxcState.Valid() )
		{
			return Unexpected( "DXC compiler is not available" );
		}

		// Create the DXC compiler.
		ComPtr<IDxcUtils> dxcUtils;
		ComPtr<IDxcCompiler3> dxcCompiler;

		if ( FAILED( dxcState.CreateInstance( CLSID_DxcUtils, IID_PPV_ARGS( dxcUtils.GetAddressOf() ) ) ) )
		{
			return Unexpected( "Failed to create DXC Utils" );
		}

		if ( FAILED( dxcState.CreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( dxcCompiler.GetAddressOf() ) ) ) )
		{
			return Unexpected( "Failed to create DXC Compiler" );
		}

		// Include handler
		ComPtr<IDxcIncludeHandler> includeHandler;
		if ( FAILED( dxcUtils->CreateDefaultIncludeHandler( &includeHandler ) ) )
		{
			return Unexpected( "Failed to create DXC Include Handler" );
		}

		DxcBuffer sourceBuffer;
		sourceBuffer.Ptr = a_Input.Source.data();
		sourceBuffer.Size = a_Input.Source.size();
		sourceBuffer.Encoding = DXC_CP_UTF8;

		// Create the arguments to pass to the compiler.
		Expected<Array<WString>, String> args = CreateCompilerArguments( a_Input, shaderType, entryPoint );
		if ( args.IsError() )
		{
			return Unexpected( std::format( "Failed to create compiler arguments '{}'", args.Error() ) );
		}

		Array<const wchar_t*> argsRaw;
		argsRaw.Reserve( args->Size() );
		for ( const WString& arg : args.Value() )
		{
			argsRaw.PushBack( arg.c_str() );
		}

		for ( uint32_t i = 0; i < argsRaw.Size(); ++i )
		{
			const wchar_t* p = argsRaw[i];
			size_t len = ( p ? wcslen( p ) : 0 );
			//LOG( LogCategory::RHI, Debug, "arg[{}] ptr = {:#p}, len = {}", i, p, len );
		}

		// COMPILE
		ComPtr<IDxcResult> dxcResult;
		HRESULT hr = dxcCompiler->Compile(
			&sourceBuffer,
			argsRaw.Data(),
			Cast<uint32_t>( argsRaw.Size() ),
			includeHandler.Get(),
			IID_PPV_ARGS( dxcResult.GetAddressOf() )
		);

		// Check if compilation failed.
		if ( FAILED( hr ) )
		{
			return Unexpected( "Failed to compile shader" );
		}

		// Get the error ( if it exists )
		ComPtr<IDxcBlobUtf8> errorBlob;
		if ( FAILED( dxcResult->GetOutput( DXC_OUT_ERRORS, IID_PPV_ARGS( errorBlob.GetAddressOf() ), nullptr ) ) )
		{
			return Unexpected( "Failed to compile shader and retrieve the corresponding error" );
		}

		if ( errorBlob != nullptr && errorBlob->GetStringLength() > 0 )
		{
			return Unexpected( errorBlob->GetStringPointer() );
		}

		ComPtr<IDxcBlob> shaderBlob;
		if ( FAILED( dxcResult->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( shaderBlob.GetAddressOf() ), nullptr ) )
			|| shaderBlob == nullptr )
		{
			return Unexpected( "Failed to get shader blob" );
		}

		if ( shaderBlob->GetBufferSize() <= 0 )
		{
			return Unexpected( "Shader blob is empty" );
		}

		ShaderCompilerOutput output;
		// Copy the shader blob to the output.
		output.ByteCode.Resize( shaderBlob->GetBufferSize() );
		std::memcpy( output.ByteCode.Data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize() );

		// Get reflection data from the DXIL if requested and if the format is HLSL.
		if ( a_Input.GenerateReflectionData && IsHLSLFormat( a_Input.Format ) )
		{
			ComPtr<IDxcBlob> reflectionBlob;
			ComPtr<ID3D12ShaderReflection> shaderReflection;

			if ( SUCCEEDED( dxcResult->GetOutput( DXC_OUT_REFLECTION, IID_PPV_ARGS( &reflectionBlob ), nullptr ) ) )
			{
				DxcBuffer reflectionData;
				reflectionData.Ptr = reflectionBlob->GetBufferPointer();
				reflectionData.Size = reflectionBlob->GetBufferSize();
				reflectionData.Encoding = 0;

				// Create the shader reflection.
				dxcUtils->CreateReflection( &reflectionData, IID_PPV_ARGS( &shaderReflection ) );

				D3D12_SHADER_DESC desc{};
				shaderReflection->GetDesc( &desc );

				// Collect the shader bindings
				output.Reflection.Bindings.Reserve( desc.BoundResources );
				for ( UINT i = 0; i < desc.BoundResources; i++ )
				{
					D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
					shaderReflection->GetResourceBindingDesc( i, &bindDesc );

					ShaderReflectionBinding& binding = output.Reflection.Bindings.EmplaceBack();
					binding.Name = bindDesc.Name;
					binding.Slot = bindDesc.BindPoint;
					binding.Space = bindDesc.Space;
					binding.Count = bindDesc.BindCount;

					// Get the size of the binding.
					if ( bindDesc.Type == D3D_SIT_CBUFFER )
					{
						if ( ID3D12ShaderReflectionConstantBuffer* constantBuffer = shaderReflection->GetConstantBufferByName( bindDesc.Name ) )
						{
							D3D12_SHADER_BUFFER_DESC bufferDesc{};
							if ( SUCCEEDED( constantBuffer->GetDesc( &bufferDesc ) ) )
							{
								binding.Size = bufferDesc.Size;
							}
						}
					}

					switch ( bindDesc.Type )
					{
						case D3D_SIT_CBUFFER:           binding.Type = ShaderReflectionBinding::ConstantBuffer; break;
						case D3D_SIT_TEXTURE:           binding.Type = ShaderReflectionBinding::Texture; break;
						case D3D_SIT_SAMPLER:           binding.Type = ShaderReflectionBinding::Sampler; break;
						case D3D_SIT_UAV_RWTYPED:
						case D3D_SIT_UAV_RWSTRUCTURED:
						case D3D_SIT_UAV_RWBYTEADDRESS: binding.Type = ShaderReflectionBinding::StorageBuffer; break;
						case D3D_SIT_STRUCTURED:
						case D3D_SIT_BYTEADDRESS:       binding.Type = ShaderReflectionBinding::StructuredBuffer; break;
						default: ASSERT( false );       binding.Type = ShaderReflectionBinding::Unknown; break;
					}

					if ( binding.Type == ShaderReflectionBinding::Texture )
					{
						switch ( bindDesc.Dimension )
						{
						case D3D_SRV_DIMENSION_TEXTURE1D:          binding.TextureDimension = ERHITextureDimension::Texture1D; break;
						case D3D_SRV_DIMENSION_TEXTURE1DARRAY:     binding.TextureDimension = ERHITextureDimension::Texture1DArray; break;
						case D3D_SRV_DIMENSION_TEXTURE2D:          binding.TextureDimension = ERHITextureDimension::Texture2D; break;
						case D3D_SRV_DIMENSION_TEXTURE2DARRAY:     binding.TextureDimension = ERHITextureDimension::Texture2DArray; break;
						case D3D_SRV_DIMENSION_TEXTURE2DMS:        binding.TextureDimension = ERHITextureDimension::Texture2D; break;
						case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:   binding.TextureDimension = ERHITextureDimension::Texture2DArray; break;
						case D3D_SRV_DIMENSION_TEXTURE3D:          binding.TextureDimension = ERHITextureDimension::Texture3D; break;
						case D3D_SRV_DIMENSION_TEXTURECUBE:        binding.TextureDimension = ERHITextureDimension::TextureCube; break;
						case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:   binding.TextureDimension = ERHITextureDimension::TextureCubeArray; break;
						default:                                   binding.TextureDimension = ERHITextureDimension::Unknown; break;
						}
					}
				}
			}
		}

		if ( IsSPIRVFormat( a_Input.Format ) )
		{
			auto result = SpirVPostProcess( a_Input, output );
			if ( result.IsError() )
			{
				return Unexpected( std::format( "Failed to post-process SPIR-V because '{}'", result.Error() ) );
			}
		}

		return output;
    }

	Expected<Pair<ERHIShaderType, StringView>, String> GetShaderTypeAndEntryPoint( const ShaderCompilerInput& a_Input )
	{
		StringView entryPoint = a_Input.EntryPoint;
		if ( entryPoint.empty() )
		{
			if ( a_Input.ShaderType == ERHIShaderType::Unknown )
			{
				// Attempt to parse the entry point from the source code.
				TODO( "Hack to find entry point" );
				size_t index = a_Input.Source.find( "SMain(" );
				const size_t entryPointLength = 6; // Length of "VSMain"
				if ( index == String::npos || index == 0 )
				{
					return Unexpected( "Failed to find entry point" );
				}

				entryPoint = StringView{ &a_Input.EntryPoint.at( index - 1 ), entryPointLength };
			}
			else
			{
				// Set the entry point to the default one for the shader type.
				switch ( a_Input.ShaderType )
				{
				case ERHIShaderType::Vertex:        entryPoint = "VSMain"; break;
				case ERHIShaderType::Hull:          entryPoint = "HSMain"; break;
				case ERHIShaderType::Domain:        entryPoint = "DSMain"; break;
				case ERHIShaderType::Geometry:      entryPoint = "GSMain"; break;
				case ERHIShaderType::Pixel:         entryPoint = "PSMain"; break;
				case ERHIShaderType::Compute:       entryPoint = "CSMain"; break;
				case ERHIShaderType::Amplification: entryPoint = "ASMain"; break;
				case ERHIShaderType::Mesh:          entryPoint = "MSMain"; break;
				case ERHIShaderType::RayGeneration: entryPoint = "RGMain"; break;
				case ERHIShaderType::Intersection:  entryPoint = "ISMain"; break;
				case ERHIShaderType::AnyHit:        entryPoint = "AHMain"; break;
				case ERHIShaderType::ClosestHit:    entryPoint = "CHMain"; break;
				case ERHIShaderType::Miss:          entryPoint = "MissMain"; break;
				case ERHIShaderType::Callable:      entryPoint = "CallMain"; break;
				default: ASSERT( false ); return Unexpected( "Unknown shader type" );
				}

				// Check if the entry point is valid.
				if ( a_Input.Source.find( entryPoint ) == String::npos )
				{
					return Unexpected( std::format( "Expected Entry point '{}' not found", entryPoint ) );
				}
			}
		}

		ERHIShaderType shaderType = a_Input.ShaderType;
		if ( shaderType == ERHIShaderType::Unknown )
		{
			// Attempt to parse the shader type from the entry point.
			if ( entryPoint.starts_with( "VS" ) )
			{
				shaderType = ERHIShaderType::Vertex;
			}
			else if ( entryPoint.starts_with( "HS" ) )
			{
				shaderType = ERHIShaderType::Hull;
			}
			else if ( entryPoint.starts_with( "DS" ) )
			{
				shaderType = ERHIShaderType::Domain;
			}
			else if ( entryPoint.starts_with( "GS" ) )
			{
				shaderType = ERHIShaderType::Geometry;
			}
			else if ( entryPoint.starts_with( "PS" ) )
			{
				shaderType = ERHIShaderType::Pixel;
			}
			else if ( entryPoint.starts_with( "CS" ) )
			{
				shaderType = ERHIShaderType::Compute;
			}
			else if ( entryPoint.starts_with( "AS" ) )
			{
				shaderType = ERHIShaderType::Amplification;
			}
			else if ( entryPoint.starts_with( "MS" ) )
			{
				shaderType = ERHIShaderType::Mesh;
			}
			else if ( entryPoint.starts_with( "RG" ) )
			{
				shaderType = ERHIShaderType::RayGeneration;
			}
			else if ( entryPoint.starts_with( "IS" ) )
			{
				shaderType = ERHIShaderType::Intersection;
			}
			else if ( entryPoint.starts_with( "AH" ) )
			{
				shaderType = ERHIShaderType::AnyHit;
			}
			else if ( entryPoint.starts_with( "CH" ) )
			{
				shaderType = ERHIShaderType::ClosestHit;
			}
			else if ( entryPoint.starts_with( "Miss" ) )
			{
				shaderType = ERHIShaderType::Miss;
			}
			else if ( entryPoint.starts_with( "Call" ) )
			{
				shaderType = ERHIShaderType::Callable;
			}
			else
			{
				return Unexpected( "Unknown shader type" );
			}
		}

		return Pair{ shaderType, entryPoint };
	}

	WStringView GetShaderModelFlag( ERHIShaderType a_Type, ERHIShaderModel a_Model )
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
		case ERHIShaderType::Compute:
		{
			switch ( a_Model )
			{
			default:                      return L"cs_6_0";
			case ERHIShaderModel::SM_6_0: return L"cs_6_0";
			case ERHIShaderModel::SM_6_1: return L"cs_6_1";
			case ERHIShaderModel::SM_6_2: return L"cs_6_2";
			case ERHIShaderModel::SM_6_3: return L"cs_6_3";
			case ERHIShaderModel::SM_6_4: return L"cs_6_4";
			case ERHIShaderModel::SM_6_5: return L"cs_6_5";
			case ERHIShaderModel::SM_6_6: return L"cs_6_6";
			}
		}
		case ERHIShaderType::Amplification:
		{
			// Amplification shaders require SM 6.5+
			switch ( a_Model )
			{
			default:                      return L"as_6_5";
			case ERHIShaderModel::SM_6_5: return L"as_6_5";
			case ERHIShaderModel::SM_6_6: return L"as_6_6";
			}
		}
		case ERHIShaderType::Mesh:
		{
			// Mesh shaders require SM 6.5+
			switch ( a_Model )
			{
			default:                      return L"ms_6_5";
			case ERHIShaderModel::SM_6_5: return L"ms_6_5";
			case ERHIShaderModel::SM_6_6: return L"ms_6_6";
			}
		}
		case ERHIShaderType::RayGeneration:
		{
			// Ray tracing shaders require SM 6.3+
			return L"lib_6_3";
		}
		case ERHIShaderType::Intersection:
		case ERHIShaderType::AnyHit:
		case ERHIShaderType::ClosestHit:
		case ERHIShaderType::Miss:
		case ERHIShaderType::Callable:
		{
			// All ray tracing shaders use library target
			return L"lib_6_3";
		}
		}

		ASSERT( false, "GetShaderModelFlag: Unsupported shader type." );
		return L"";
	};

	Expected<Array<WString>, String> CreateCompilerArguments( const ShaderCompilerInput& a_Input, ERHIShaderType a_ShaderType, StringView a_EntryPoint )
	{
		// Arguments that will be passed to the compiler.
		Array<WString> args; args.Reserve( a_Input.CustomArguments.Size() );

		// Add custom arguments.
		for ( const auto& customArg : a_Input.CustomArguments )
		{
			args.EmplaceBack( ToWString( customArg ) );
		}

		// Only use custom arguments and return here.
		if ( a_Input.OverrideDefaultArguments )
		{
			return args;
		}

		args.EmplaceBack( L"-res_may_alias" );

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

				// Only vertex-like shaders (Vertex, Hull, Domain, Geometry) support invert-y
				// Pixel, Compute, Mesh, Amplification, and Ray Tracing shaders don't support invert-y
				const bool supportsInvertY = ( a_Input.ShaderType == ERHIShaderType::Vertex ||
				                               a_Input.ShaderType == ERHIShaderType::Hull ||
				                               a_Input.ShaderType == ERHIShaderType::Domain ||
				                               a_Input.ShaderType == ERHIShaderType::Geometry );
				if ( supportsInvertY )
				{
					args.EmplaceBack( L"-fvk-invert-y" ); // Make vulkan and opengl have the same coordinate system as D3D (Y-up)
				}

				// Shift the registers
				//args.EmplaceBack( L"-fvk-t-shift" ); args.EmplaceBack( TO_LSTRING( RHI_SRV_BINDING_SLOT_OFFSET ) ); args.EmplaceBack( L"0" );
				//args.EmplaceBack( L"-fvk-u-shift" ); args.EmplaceBack( TO_LSTRING( RHI_UAV_BINDING_SLOT_OFFSET ) ); args.EmplaceBack( L"0" );
				//args.EmplaceBack( L"-fvk-s-shift" ); args.EmplaceBack( TO_LSTRING( RHI_SAMPLER_BINDING_SLOT_OFFSET ) ); args.EmplaceBack( L"0" );
				break;
			}
		}

		// Set the shader model profile.
		// Convert the shader type and model to a string for the compiler.
		// E.g. "vs_5_0" for a vertex shader with shader model 5.0.
		args.EmplaceBack( L"-T" );
		args.EmplaceBack( GetShaderModelFlag( a_ShaderType, a_Input.MinimumModel ) );

		#if RHI_DEBUG_ENABLED
		if ( a_Input.Flags.HasFlag( ERHIShaderCompilerFlags::EnableDebugInfo ) )
		{
			args.EmplaceBack( L"-Zi" );
		}
		#endif // RHI_DEBUG_ENABLED

		// Disable validation
		if ( a_Input.Flags.HasFlag( ERHIShaderCompilerFlags::DisableValidation ) )
		{
			args.EmplaceBack( L"-Vd" );
		}

		// Pack matrices in column-major or row-major order.
		if ( a_Input.Flags.HasFlag( ERHIShaderCompilerFlags::RowMajor ) )
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
		args.EmplaceBack( L"-E" );
		args.EmplaceBack( a_EntryPoint.begin(), a_EntryPoint.end() );

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
		TODO( "Temp" );
		FilePath shaderPath = FilePath::CurrentPath();
		shaderPath = shaderPath / "../Tridium/Source/Tridium/Shaders";
		args.EmplaceBack( L"-I" ); args.EmplaceBack( shaderPath.ToWString() );
		args.EmplaceBack( L"-I" ); args.EmplaceBack( ( shaderPath / "Families" ).ToWString());
		for ( const auto& includeDir : a_Input.IncludeDirectories )
		{
			args.EmplaceBack( L"-I" );
			args.EmplaceBack( includeDir.begin(), includeDir.end() );
		}

		return args;
	}

	Expected<void, String> SpirVPostProcess( const ShaderCompilerInput& a_Input, ShaderCompilerOutput& a_Output )
	{
		if ( a_Input.Format == ERHIShaderFormat::SPIRV_OpenGL )
		{
			// Create GLSL from the SPIR-V bytecode using SPIRV-Cross

			// We create the compiler on the heap as it's too large for the stack.
			auto glslCompilerPtr = MakeUnique<spirv_cross::CompilerGLSL>( 
				ReinterpretCast<const uint32_t*>( a_Output.ByteCode.Data() ),
				a_Output.ByteCode.Size() / sizeof( uint32_t ) 
			);

			spirv_cross::CompilerGLSL& glslCompiler = *glslCompilerPtr;
			spirv_cross::CompilerGLSL::Options options;
			options.version = 450;
			options.es = false;
			glslCompiler.set_common_options( options );

			// Build dummy samplers for combined images to avoid compilation errors.
			const spirv_cross::VariableID dummySamplerID = glslCompiler.build_dummy_sampler_for_combined_images();

			// OpenGL doesn't support separate textures and samplers, so we need to combine them.
			glslCompiler.build_combined_image_samplers();

			spirv_cross::ShaderResources shaderResources = glslCompiler.get_shader_resources();
			for ( const auto& resource : shaderResources.uniform_buffers )
			{
				TODO( "We are setting the interface name of the block as I cant use the instance name for shader bindings. Hack" );
				glslCompiler.set_name( resource.base_type_id,
					glslCompiler.get_block_fallback_name( resource.id ) 
				);
			}

			for ( const auto& resource : shaderResources.storage_buffers )
			{
				TODO( "We are setting the interface name of the block as I cant use the instance name for shader bindings. Hack" );
				glslCompiler.set_name( resource.base_type_id,
					glslCompiler.get_block_fallback_name( resource.id ) 
				);
			}

			// Textures and samplers are combined in GLSL, so we need to keep track of them and set the correct names
			auto combinedSamplers = glslCompiler.get_combined_image_samplers();
			UnorderedMap<spirv_cross::VariableID, spirv_cross::VariableID> seenImageIDs;
			seenImageIDs.reserve( combinedSamplers.size() );
			for ( auto& sampler : combinedSamplers )
			{
				if ( dummySamplerID == sampler.sampler_id )
				{
					// Ignore dummy samplers created by SPIRV-Cross for combined image samplers.
					continue;
				}

				if ( seenImageIDs.contains( sampler.image_id ) )
				{
					return Unexpected( std::format( "Textures bound to multiple samplers are not supported - Use COMBINED_SAMPLER() in HLSL code."
						" Sampler : {}, Image: {}", glslCompiler.get_name( sampler.combined_id ), glslCompiler.get_name( sampler.image_id ) ) );
				}

				seenImageIDs[sampler.image_id] = sampler.sampler_id;
				const String& texName = glslCompiler.get_name( sampler.image_id );
				// Set the name of the combined sampler to the texture name.
				// This is helpful for setting Texture Shader Inputs via the RHICommandList_OpenGLImpl.
				glslCompiler.set_name( sampler.combined_id, texName );
			}

			for ( auto& sampler : combinedSamplers )
			{
				if ( dummySamplerID != sampler.sampler_id )
				{
					continue;
				}
				
				// Remap combined samplers that use dummy samplers to the original combined sampler name.
				glslCompiler.set_decoration( sampler.combined_id, spv::DecorationBinding, glslCompiler.get_decoration( sampler.image_id, spv::DecorationBinding ) );
			}


			// Get the reflection data if requested.
			if ( a_Input.GenerateReflectionData )
			{
				a_Output.Reflection.Bindings.Reserve( shaderResources.uniform_buffers.size()
					+ shaderResources.storage_buffers.size()
					+ shaderResources.sampled_images.size() );

				// Uniform buffers -> Constant Buffers
				for ( const auto& resource : shaderResources.uniform_buffers )
				{
					ShaderReflectionBinding& binding = a_Output.Reflection.Bindings.EmplaceBack();
					binding.Name = glslCompiler.get_name( resource.id );
					binding.Slot = glslCompiler.get_decoration( resource.id, spv::DecorationBinding );
					binding.Space = glslCompiler.get_decoration( resource.id, spv::DecorationDescriptorSet );
					binding.Count = 1;
					binding.Type = ShaderReflectionBinding::ConstantBuffer;
					binding.Size = glslCompiler.get_declared_struct_size( glslCompiler.get_type( resource.base_type_id ) );
				}

				// Storage buffers/Structured buffers
				for ( const auto& resource : shaderResources.storage_buffers )
				{
					ShaderReflectionBinding& binding = a_Output.Reflection.Bindings.EmplaceBack();
					binding.Name = glslCompiler.get_name( resource.id );
					binding.Slot = glslCompiler.get_decoration( resource.id, spv::DecorationBinding ) - RHI_UAV_BINDING_SLOT_OFFSET;
					binding.Space = glslCompiler.get_decoration( resource.id, spv::DecorationDescriptorSet );
					binding.Count = 1;
					binding.Size = 0; // Size is unknown for storage buffers.

					const auto flags = glslCompiler.get_buffer_block_flags( resource.id );
					const bool readonly = flags.get( spv::DecorationNonWritable );
					const bool writeonly = flags.get( spv::DecorationNonReadable );

					if ( readonly && !writeonly )
						binding.Type = ShaderReflectionBinding::StructuredBuffer; // read-only
					else
						binding.Type = ShaderReflectionBinding::StorageBuffer;    // read/write
				}

				const auto ConvertSpirvDimToRHI = []( spv::Dim dim ) -> ERHITextureDimension
				{
					switch ( dim )
					{
					case spv::Dim1D:        return ERHITextureDimension::Texture1D;
					case spv::Dim2D:        return ERHITextureDimension::Texture2D;
					case spv::Dim3D:        return ERHITextureDimension::Texture3D;
					case spv::DimCube:      return ERHITextureDimension::TextureCube;
					case spv::DimRect:      return ERHITextureDimension::Texture2D; // No direct equivalent
					default:                return ERHITextureDimension::Unknown;
					}
				};

				for ( const auto& resource : shaderResources.separate_images )
				{
					if ( seenImageIDs.contains( resource.id ) )
					{
						continue; // Skip textures that are part of combined samplers.
					}

					ShaderReflectionBinding& binding = a_Output.Reflection.Bindings.EmplaceBack();
					binding.Name = glslCompiler.get_name( resource.id );
					binding.Slot = glslCompiler.get_decoration( resource.id, spv::DecorationBinding ) - RHI_SRV_BINDING_SLOT_OFFSET;
					binding.Space = glslCompiler.get_decoration( resource.id, spv::DecorationDescriptorSet );
					binding.Count = 1;
					binding.Type = ShaderReflectionBinding::Texture;
					binding.Size = 0;
					binding.TextureDimension = ConvertSpirvDimToRHI( glslCompiler.get_type( resource.type_id ).image.dim );
				}

				// Sampled images -> Combined Samplers
				for ( const auto& c : glslCompiler.get_combined_image_samplers() )
				{
					if ( dummySamplerID == c.sampler_id )
					{
						// Ignore dummy samplers created by SPIRV-Cross for combined image samplers.
						continue;
					}

					ShaderReflectionBinding binding;
					binding.Name = glslCompiler.get_name( c.combined_id );

					// Use the original binding from the texture part, not the GLSL output
					binding.Slot = glslCompiler.get_decoration( c.image_id, spv::DecorationBinding ) - RHI_SRV_BINDING_SLOT_OFFSET;
					binding.Space = glslCompiler.get_decoration( c.image_id, spv::DecorationDescriptorSet );

					binding.Type = ShaderReflectionBinding::Texture;
					binding.Count = 1;
					binding.Size = 0;

					binding.TextureDimension = ConvertSpirvDimToRHI( glslCompiler.get_type_from_variable( c.image_id ).image.dim );

					a_Output.Reflection.Bindings.EmplaceBack( binding );
				}


			}

			String glsl = glslCompiler.compile();
			TODO( "First we copy the dxblob into ByteCode, then we copy into spirv cross, then we compile into a string and then copy back into ByteCode. We can optimize this" );
			a_Output.ByteCode.Resize( glsl.size() );
			std::memcpy( a_Output.ByteCode.Data(), glsl.data(), glsl.size() );
		}
		else // Vulkan Spir-V
		{
			TODO( "Vulkan SPIR-V" );
			NOT_IMPLEMENTED;
		}
	
		return {};
	}

}
