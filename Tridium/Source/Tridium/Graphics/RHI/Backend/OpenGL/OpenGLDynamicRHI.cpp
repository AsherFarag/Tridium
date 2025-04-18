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
#include "OpenGLShaderBindingLayout.h"
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
			ASSERT_LOG( false, "Failed to initialize GLAD!" );
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
		LOG( LogCategory::OpenGL, Info, "OpenGL Version: {0}", reinterpret_cast<const char*>( stringVer ) );

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
		RHIFenceRef fence = RHI::CreateNativeResource<OpenGLFence>();
		CHECK( fence->Commit( a_Desc ) );
		return fence;
	}

	RHISamplerRef OpenGLDynamicRHI::CreateSampler( const RHISamplerDescriptor& a_Desc )
	{
		RHISamplerRef sampler = RHI::CreateNativeResource<OpenGLSampler>();
		CHECK( sampler->Commit( a_Desc ) );
		return sampler;
	}

	RHITextureRef OpenGLDynamicRHI::CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
	{
		if ( a_Desc.Dimension != ERHITextureDimension::Texture2D || a_Desc.Depth != 1 || a_Desc.Mips != 1 )
		{
			TODO( "Only 2D textures are supported!" );
			return nullptr;
		}

		RHITextureRef tex = RHI::CreateNativeResource<OpenGLTexture>( a_Desc );
		auto* glTex = static_cast<OpenGLTexture*>( tex.get() );
		glTex->Commit( a_Desc );

		glTex->GLFormat = RHIToGLTextureFormat( a_Desc.Format );
		if ( !glTex->GLFormat.IsValid() )
		{
			return nullptr;
		}

		// Generate a texture handle
		glTex->TextureObj.Create();
		OpenGL1::BindTexture( GL_TEXTURE_2D, glTex->TextureObj );
		glTex->TextureObj.SetName( a_Desc.Name );
		if ( !glTex->TextureObj.Valid() )
		{
			return nullptr;
		}

		TODO( "This only works for 2D textures and not multiple subresources!" );
		// Set the texture data
		OpenGL4::TextureStorage2D( glTex->TextureObj, 1, glTex->GLFormat.InternalFormat, a_Desc.Width, a_Desc.Height );
		if ( a_SubResourcesData.size() > 0 )
		{
			OpenGL4::TextureSubImage2D( glTex->TextureObj, 0, 0, 0, a_Desc.Width, a_Desc.Height,
				glTex->GLFormat.Format, glTex->GLFormat.Type, a_SubResourcesData[0].Data.data() );
		}

		return tex;
	}

	RHIBufferRef OpenGLDynamicRHI::CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data )
	{
		return RHI::CreateNativeResource<OpenGLBuffer>( a_Desc );
	}

	RHIGraphicsPipelineStateRef OpenGLDynamicRHI::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc )
	{
		RHIGraphicsPipelineStateRef pso = RHI::CreateNativeResource<OpenGLGraphicsPipelineState>();
		CHECK( pso->Commit( a_Desc ) );
		return pso;
	}

	RHICommandListRef OpenGLDynamicRHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<OpenGLCommandList>( a_Desc );
	}

	RHIShaderModuleRef OpenGLDynamicRHI::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		RHIShaderModuleRef shader = RHI::CreateNativeResource<OpenGLShaderModule>();
		CHECK( shader->Commit( a_Desc ) );
		return shader;
	}

	RHIShaderBindingLayoutRef OpenGLDynamicRHI::CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc )
	{
		RHIShaderBindingLayoutRef sbl = RHI::CreateNativeResource<OpenGLShaderBindingLayout>();
		CHECK( sbl->Commit( a_Desc ) );
		return sbl;
	}

	RHISwapChainRef OpenGLDynamicRHI::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		RHISwapChainRef sc = RHI::CreateNativeResource<OpenGLSwapChain>();
		CHECK( sc->Commit( a_Desc ) );
		return sc;
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