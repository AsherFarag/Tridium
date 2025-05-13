#include "tripch.h"
#include "OpenGLDynamicRHI.h"
#include "OpenGLCommon.h"
#include <iostream>

#include <Tridium/Graphics/RHI/RHI.h>

// Resources
#include "OpenGLSampler.h"
#include "OpenGLTexture.h"
#include "OpenGLBuffer.h"
#include "OpenGLPipelineState.h"
#include "OpenGLSwapChain.h"
#include "OpenGLCommandList.h"
#include "OpenGLShader.h"
#include "OpenGLShaderBindings.h"
#include "OpenGLFence.h"

namespace Tridium {

#if RHI_DEBUG_ENABLED
	void APIENTRY DebugCallback(
		GLenum a_Source, GLenum a_Type, GLuint a_ID,
		GLenum a_Severity,
		GLsizei a_Length, const GLchar* a_Message, const void* a_UserParam );
#endif

	//////////////////////////////////////////////////////////////////////////
	// CORE RHI FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	bool OpenGLDynamicRHI::Init( const RHIConfig& a_Config )
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
			OpenGL4::DebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
		}
	#endif

		OpenGL1::Enable( GL_DEPTH_TEST );
		OpenGL1::DepthMask( GL_TRUE );
		TODO( "Should we be doing this here?" );
		OpenGL1::Enable( GL_MULTISAMPLE );
		OpenGL1::Enable( GL_BLEND );
		OpenGL1::BlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		OpenGL1::Enable( GL_TEXTURE_CUBE_MAP_SEAMLESS );

		const GLubyte* stringVer = OpenGL1::GetString( GL_VERSION );
		LOG( LogCategory::OpenGL, Info, "OpenGL Version: {0}", ReinterpretCast<const char*>( stringVer ) );

		return true;
	}

	bool OpenGLDynamicRHI::Shutdown()
	{
		GLState::ClearState();
		return true;
	}

	bool OpenGLDynamicRHI::ExecuteCommandList( RHICommandListRef a_CommandList )
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHIFenceRef OpenGLDynamicRHI::CreateFence( const RHIFenceDescriptor& a_Desc )
	{
		return  RHI::CreateNativeResource<RHIFence_OpenGLImpl>( a_Desc );
	}

	RHISamplerRef OpenGLDynamicRHI::CreateSampler( const RHISamplerDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHISampler_OpenGLImpl>( a_Desc );
	}

	RHITextureRef OpenGLDynamicRHI::CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
	{
		return RHI::CreateNativeResource<RHITexture_OpenGLImpl>( a_Desc, a_SubResourcesData );
	}

	RHIBufferRef OpenGLDynamicRHI::CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data )
	{
		return RHI::CreateNativeResource<RHIBuffer_OpenGLImpl>( a_Desc, a_Data );
	}

	RHIGraphicsPipelineStateRef OpenGLDynamicRHI::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIGraphicsPipelineState_OpenGLImpl>( a_Desc );
	}

	RHICommandListRef OpenGLDynamicRHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHICommandList_OpenGLImpl>( a_Desc );
	}

	RHIShaderModuleRef OpenGLDynamicRHI::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIShaderModule_OpenGLImpl>( a_Desc );
	}

	RHIBindingLayoutRef OpenGLDynamicRHI::CreateBindingLayout( const RHIBindingLayoutDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIBindingLayout_OpenGLImpl>( a_Desc );
	}

	RHIBindingSetRef OpenGLDynamicRHI::CreateBindingSet( const RHIBindingSetDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIBindingSet_OpenGLImpl>( a_Desc );
	}

	RHISwapChainRef OpenGLDynamicRHI::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHISwapChain_OpenGLImpl>( a_Desc );
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

	void OpenGLDynamicRHI::DumpDebug()
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