#include "tripch.h"
#include <Tridium/Core/Platform.h>
#include "DXCompiler.h"
#include "D3D12Common.h"
#include <dxcapi.h>  // DXC Compiler API

namespace Tridium {

	// Internal state for the DXC compiler.
	struct DXCInternalState
	{
		DxcCreateInstanceProc CreateInstance = nullptr;

		bool IsValid() const { return CreateInstance != nullptr; }

		DXCInternalState( const String& a_Modifier = "" )
		{
		#if CONFIG_WINDOWS
			const String fileName = "dxcompiler" + a_Modifier + ".dll";
		#elif CONFIG_LINUX
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

		// Arguments that will be passed to the compiler.
		Array<WString> args = {
		};

		if ( a_Input.Flags.HasFlag( ERHIShaderCompilerFlags::DisableOptimization ) )
		{
			// Disable optimization flag
			args.PushBack( L"-Od" );
		}

		// Add flags based on the shader format.
		switch ( a_Input.Format )
		{
			case ERHIShaderFormat::HLSL6:
			case ERHIShaderFormat::HLSL6_XBOX:
			{
				// Declare that 
				args.PushBack( L"-rootsig-define" );
				args.PushBack( L"TRIDIUM_DEFAULT_ROOT_SIGNATURE" );
				break;
			}
			case ERHIShaderFormat::SPIRV:
			{
				args.PushBack( L"-spirv" );
				break;
			}
			default:
			{
				ASSERT_LOG( false, "DXCompiler::Compile: Unsupported shader format." );
				return {};
			}
		}

		// Convert the shader type and model to a string for the compiler.
		// E.g. "vs_5_0" for a vertex shader with shader model 5.0.
		args.PushBack( L"-T" );
		args.PushBack( GetShaderModelFlag( a_Input.ShaderType, a_Input.MinimumModel ) );

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

		Array<const wchar_t*> argsRaw;
		argsRaw.Reserve( args.Size() );
		for ( const WString& arg : args )
		{
			argsRaw.PushBack( arg.c_str() );
		}

		ComPtr<IDxcResult> dxcResult;
		HRESULT hr = dxcCompiler->Compile(
			&sourceBuffer,
			argsRaw.Data(),
			static_cast<uint32_t>( argsRaw.Size() ),
			includeHandler.Get(),
			IID_PPV_ARGS( &dxcResult )
		);

		if ( FAILED( hr ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to compile shader" );
			return {};
		}

		ComPtr<IDxcBlobUtf8> errorBlob;
		if ( FAILED( dxcResult->GetOutput( DXC_OUT_ERRORS, IID_PPV_ARGS( &errorBlob ), nullptr ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to get error blob" );
			return {};
		}

		if ( errorBlob )
		{
			output.Error = String( errorBlob->GetStringPointer() );
		}

		ComPtr<IDxcBlob> shaderBlob;
		if ( FAILED( dxcResult->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( &shaderBlob ), nullptr ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to get shader blob" );
			return output;
		}

		if ( shaderBlob && shaderBlob->GetBufferSize() > 0 )
		{
			output.ByteCode.Resize( shaderBlob->GetBufferSize() );
			std::memcpy( output.ByteCode.Data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize() );
		}

		return output;
    }

	ShaderCompilerOutput DXCompiler::CompileRootSignature( StringView s_Source, ERHIShaderFormat a_Format )
	{
		ShaderCompilerOutput output;
		DXCInternalState& dxcState = ( a_Format == ERHIShaderFormat::HLSL6_XBOX ) ? GetDXCInternalState_XBOX() : GetDXCInternalState();
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
		// Arguments that will be passed to the compiler.
		Array<WString> args = {
			L"-rootsig-define",
			L"TRIDIUM_DEFAULT_ROOT_SIGNATURE"
		};
		// Convert the shader type and model to a string for the compiler.
		// E.g. "vs_5_0" for a vertex shader with shader model 5.0.
		args.PushBack( L"-T" );
		args.PushBack( L"rootsig_1_0" );
		// Include handler
		ComPtr<IDxcIncludeHandler> includeHandler;
		if ( FAILED( dxcUtils->CreateDefaultIncludeHandler( &includeHandler ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create DXC Include Handler" );
			return {};
		}
		DxcBuffer sourceBuffer;
		sourceBuffer.Ptr = s_Source.data();
		sourceBuffer.Size = s_Source.size();
		sourceBuffer.Encoding = DXC_CP_UTF8;
		Array<const wchar_t*> argsRaw;
		argsRaw.Reserve( args.Size() );
		for ( const WString& arg : args )
		{
			argsRaw.PushBack( arg.c_str() );
		}

		ComPtr<IDxcResult> dxcResult;
		HRESULT hr = dxcCompiler->Compile(
			&sourceBuffer,
			argsRaw.Data(),
			static_cast<uint32_t>( argsRaw.Size() ),
			includeHandler.Get(),
			IID_PPV_ARGS( &dxcResult )
		);

		if ( FAILED( hr ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to compile shader" );
			return {};
		}

		ComPtr<IDxcBlobUtf8> errorBlob;
		if ( FAILED( dxcResult->GetOutput( DXC_OUT_ERRORS, IID_PPV_ARGS( &errorBlob ), nullptr ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to get error blob" );
			return {};
		}

		if ( errorBlob )
		{
			output.Error = String( errorBlob->GetStringPointer() );
		}

		ComPtr<IDxcBlob> shaderBlob;
		if ( FAILED( dxcResult->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( &shaderBlob ), nullptr ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to get shader blob" );
			return output;
		}

		if ( shaderBlob && shaderBlob->GetBufferSize() > 0 )
		{
			output.ByteCode.Resize( shaderBlob->GetBufferSize() );
			std::memcpy( output.ByteCode.Data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize() );
		}

		return output;
	}

}
