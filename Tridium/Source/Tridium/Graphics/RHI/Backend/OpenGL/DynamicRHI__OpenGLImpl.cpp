#include "tripch.h"
#include "RHI_OpenGLImpl.h"
#include <iostream>


namespace Tridium::OpenGL {

#if RHI_DEBUG_ENABLED
	void APIENTRY DebugCallback(
		GLenum a_Source, GLenum a_Type, GLuint a_ID,
		GLenum a_Severity,
		GLsizei a_Length, const GLchar* a_Message, const void* a_UserParam );
#endif

	//////////////////////////////////////////////////////////////////////////
	// CORE RHI FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	bool DynamicRHI_OpenGLImpl::Init( const RHIConfig& a_Config )
	{
		int status = gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress );
		if ( status == 0 )
		{
			ASSERT( false, "Failed to initialize GLAD!" );
			return false;
		}

	#if RHI_DEBUG_ENABLED
		if ( a_Config.UseDebug )
		{
			// Enable OpenGL debug output
			OpenGL4::Enable( GL_DEBUG_OUTPUT );
			OpenGL4::Enable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
			OpenGL4::DebugMessageCallback( DebugCallback, nullptr );
			// Disable Detailed info logs
			OpenGL4::DebugMessageControl( GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE );
		}
	#endif

		OpenGL1::Enable( GL_DEPTH_TEST );
		OpenGL1::DepthMask( GL_TRUE );
		TODO( "Should we be doing this here?" );
		OpenGL1::Enable( GL_MULTISAMPLE );
		OpenGL1::Enable( GL_BLEND );
		OpenGL1::BlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		OpenGL1::Enable( GL_TEXTURE_CUBE_MAP_SEAMLESS );

		return true;
	}

	bool DynamicRHI_OpenGLImpl::Shutdown()
	{
		GLState::ClearState();
		return true;
	}

	bool DynamicRHI_OpenGLImpl::ExecuteCommandList( RHICommandListRef a_CommandList )
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHIFenceRef DynamicRHI_OpenGLImpl::CreateFence( const RHIFenceDescriptor& a_Desc )
	{
		return  RHI::CreateNativeResource<RHIFence_OpenGLImpl>( a_Desc );
	}

	RHISamplerRef DynamicRHI_OpenGLImpl::CreateSampler( const RHISamplerDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHISampler_OpenGLImpl>( a_Desc );
	}

	RHITextureRef DynamicRHI_OpenGLImpl::CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
	{
		return RHI::CreateNativeResource<RHITexture_OpenGLImpl>( a_Desc, a_SubResourcesData );
	}

	RHIBufferRef DynamicRHI_OpenGLImpl::CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data )
	{
		return RHI::CreateNativeResource<RHIBuffer_OpenGLImpl>( a_Desc, a_Data );
	}

	RHIGraphicsPipelineStateRef DynamicRHI_OpenGLImpl::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIGraphicsPipelineState_OpenGLImpl>( a_Desc );
	}

	RHICommandListRef DynamicRHI_OpenGLImpl::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHICommandList_OpenGLImpl>( a_Desc );
	}

	RHIShaderModuleRef DynamicRHI_OpenGLImpl::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIShaderModule_OpenGLImpl>( a_Desc );
	}

	RHIBindingLayoutRef DynamicRHI_OpenGLImpl::CreateBindingLayout( const RHIBindingLayoutDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIBindingLayout_OpenGLImpl>( a_Desc );
	}

	RHIBindingSetRef DynamicRHI_OpenGLImpl::CreateBindingSet( const RHIBindingSetDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIBindingSet_OpenGLImpl>( a_Desc );
	}

	RHISwapChainRef DynamicRHI_OpenGLImpl::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHISwapChain_OpenGLImpl>( a_Desc );
	}

	GPUInfo DynamicRHI_OpenGLImpl::GetGPUInfo() const
	{
		GPUInfo gpuInfo{};

		// Vendor ID
		{
			TODO( "This is a temp hack and does not handle all Vendors" );
			static const UnorderedMap<StringView, EGPUVendorID> vendorMap =
			{
				{ "NVIDIA Corporation", EGPUVendorID::Nvidia },
				{ "Intel", EGPUVendorID::Intel },
				{ "AMD", EGPUVendorID::Amd },  { "ATI", EGPUVendorID::Amd },
				{ "Apple", EGPUVendorID::Apple },
				{ "Qualcomm", EGPUVendorID::Qualcomm },
				{ "ARM", EGPUVendorID::Arm },
				{ "Microsoft", EGPUVendorID::Microsoft },
			};

			const GLubyte* vendor = OpenGL1::GetString( GL_VENDOR );
			StringView vendorStr = ReinterpretCast<const char*>( vendor );
			auto it = vendorMap.find( vendorStr );
			if ( it != vendorMap.end() )
			{
				gpuInfo.VendorID = Cast<uint32_t>( it->second );
			}
			else
			{
				gpuInfo.VendorID = Cast<uint32_t>( EGPUVendorID::Unknown );
				LOG( LogCategory::RHI, Error, "Unknown GPU Vendor: {0}", vendorStr );
			}
		}

		// Device Name
		{
			const GLubyte* deviceName = OpenGL1::GetString( GL_RENDERER );
			gpuInfo.DeviceName = ReinterpretCast<const char*>( deviceName );
		}

		// Driver Version
		{
			const GLubyte* driverVersion = OpenGL1::GetString( GL_VERSION );
			gpuInfo.DriverVersion = ReinterpretCast<const char*>( driverVersion );
		}

		// VRAM Total
		{
			switch ( Cast<EGPUVendorID>( gpuInfo.VendorID ) )
			{
				case EGPUVendorID::Nvidia:
				{
				#ifndef GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 
					#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
				#endif
					int vram = 0;
					OpenGL1::GetIntegerv( GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vram );
					gpuInfo.VRAMBytes = Cast<size_t>( vram ) * 1024 * 1024;
				}
				break;
				case EGPUVendorID::Amd:
				{
				#ifndef TEXTURE_FREE_MEMORY_ATI
					#define TEXTURE_FREE_MEMORY_ATI 0x87FC
				#endif
					int vram = 0;
					OpenGL1::GetIntegerv( TEXTURE_FREE_MEMORY_ATI, &vram );
					gpuInfo.VRAMBytes = Cast<size_t>( vram ) * 1024 * 1024;
				}
				break;
			}
		}

		// Features
		{
			// Get the highest shader model supported
			StringView versionStr = ReinterpretCast<const char*>( OpenGL1::GetString( GL_SHADING_LANGUAGE_VERSION ) );
			char majorVersion = 0;
			char minorVersion = 0;
			if ( versionStr.size() >= 4 )
			{
				// Version string format: "4.50 NVIDIA 470.57.02"
				// Major version == 4
				// Minor version == 50
				majorVersion = versionStr[0];
				minorVersion = versionStr[2];
				switch ( majorVersion )
				{
				case '4':
				{
					switch ( minorVersion )
					{
					case '3':
					{
						// GLSL 4.3 is equivalent to D3D Shader Model 5.0
						// From: https://www.khronos.org/opengl/wiki/Detecting_the_Shader_Model
						gpuInfo.DeviceFeatures.HighestShaderModel = ERHIShaderModel::SM_5_0;
						break;
					}
					default:
					{
						TODO( "Add some better version checking here" );
						gpuInfo.DeviceFeatures.HighestShaderModel = ERHIShaderModel::SM_6_0;
						break;
					}
					}
					break;
				}
				default:
				{
					// Default to the minimum shader model
					LOG( LogCategory::RHI, Error, "Unknown shader model version in '{0}', setting Highest Shader Model to SM_5_0", versionStr );
					gpuInfo.DeviceFeatures.HighestShaderModel = ERHIShaderModel::SM_5_0;
					break;
				}
				}

				LOG( LogCategory::RHI, Info, 
					"OpenGL Shader Model '{0}.{1}', setting Highest Shader Model to 'ERHIShaderModel::{2}'",
					majorVersion, minorVersion, ToString( gpuInfo.DeviceFeatures.HighestShaderModel ) );
			}
			else
			{
				gpuInfo.DeviceFeatures.HighestShaderModel = ERHIShaderModel::SM_5_0;
				LOG( LogCategory::RHI, Error, "Failed to parse shader model version: '{0}', setting Highest Shader Model to SM_5_0", versionStr );
			}

			constexpr auto SupportsFeature = []( const char* a_Name ) -> bool
				{
					GLint nExtensions = 0;
					OpenGL1::GetIntegerv( GL_NUM_EXTENSIONS, &nExtensions );
					for ( GLint i = 0; i < nExtensions; ++i )
					{
						const char* ext = (const char*)OpenGL3::GetStringi( GL_EXTENSIONS, i );
						if ( std::strcmp( ext, a_Name ) == 0 )
							return true;
					}

					return false;
				};

		#define InitFeature( _Feature, _Support ) \
			gpuInfo.DeviceFeatures.Features[Cast<uint8_t>( _Feature )].SetSupport( ( _Support ) ? ERHIFeatureSupport::Supported : ERHIFeatureSupport::Unsupported )

			using enum ERHIFeature;
			InitFeature( ComputeShaders, SupportsFeature( "GL_ARB_compute_shader" ) );
			InitFeature( MeshShaders, SupportsFeature( "GL_NV_mesh_shader" ) );
			InitFeature( Tesselation, SupportsFeature( "GL_ARB_tessellation_shader" ) );
			InitFeature( RayTracing, SupportsFeature( "GL_NV_ray_tracing" ) );
			InitFeature( BindlessResources, SupportsFeature( "GL_ARB_bindless_texture" ) );

		#undef InitFeature
		}

		return gpuInfo;
	}

	//////////////////////////////////////////////////////////////////////////
    // DEBUG
	//////////////////////////////////////////////////////////////////////////

#if RHI_DEBUG_ENABLED

	static Array<String> s_OpenGLDebugMessages;

	void APIENTRY DebugCallback(
		GLenum a_Source, GLenum a_Type, GLuint a_ID,
		GLenum a_Severity,
		GLsizei a_Length, const GLchar* a_Message, const void* a_UserParam )
	{
		switch ( a_Severity )
		{
		case GL_DEBUG_SEVERITY_HIGH:
			LOG( LogCategory::OpenGL, Error, a_Message );
			s_OpenGLDebugMessages.EmplaceBack( "Error: " + String( a_Message ) );
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			LOG( LogCategory::OpenGL, Warn, a_Message );
			s_OpenGLDebugMessages.EmplaceBack( "Warning: " + String( a_Message ) );
			break;
		case GL_DEBUG_SEVERITY_LOW:
			LOG( LogCategory::OpenGL, Info, a_Message );
			s_OpenGLDebugMessages.EmplaceBack( "Info: " + String( a_Message ) );
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			LOG( LogCategory::OpenGL, Debug, a_Message );
			s_OpenGLDebugMessages.EmplaceBack( "Debug: " + String( a_Message ) );
			break;
		}
	}

	void DynamicRHI_OpenGLImpl::DumpDebug()
	{
		std::cout << "OpenGL Debug Dump:\n";
		for ( const String& message : s_OpenGLDebugMessages )
		{
			std::cout << '	' << message << '\n';
		}
		std::cout << std::endl;

		s_OpenGLDebugMessages.Clear();
	}

#endif // RHI_DEBUG_ENABLED

} // namespace Tridium