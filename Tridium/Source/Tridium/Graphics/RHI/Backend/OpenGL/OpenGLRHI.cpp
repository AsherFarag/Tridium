#include "tripch.h"
#include "OpenGLRHI.h"
#include "OpenGLCommon.h"
#include <iostream>

// Resources
#include "OpenGLSampler.h"
#include "OpenGLTexture.h"
#include "OpenGLMesh.h"
#include "OpenGLPipelineState.h"
#include "OpenGLSwapChain.h"

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

	bool OpenGLRHI::Init( const RHIConfig& a_Config )
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

		return true;
	}

	bool OpenGLRHI::Shutdown()
	{
		return true;
	}

	bool OpenGLRHI::ExecuteCommandList( RHICommandListRef a_CommandList )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// FENCE FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	RHIFence OpenGLRHI::CreateFence() const
	{
		return RHIFence();
	}

	ERHIFenceState OpenGLRHI::GetFenceState( RHIFence a_Fence ) const
	{
		return ERHIFenceState();
	}

	void OpenGLRHI::FenceSignal( RHIFence a_Fence )
	{
	}

	//////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHISamplerRef OpenGLRHI::CreateSampler( const RHISamplerDescriptor& a_Desc )
	{
		RHISamplerRef sampler = RHIResource::Create<OpenGLSampler>();
		CHECK( sampler->Commit( &a_Desc ) );
		return sampler;
	}

	RHITextureRef OpenGLRHI::CreateTexture( const RHITextureDescriptor& a_Desc )
	{
		RHITextureRef tex = RHIResource::Create<OpenGLTexture>();
		CHECK( tex->Commit( &a_Desc ) );
		return tex;
	}

	RHIIndexBufferRef OpenGLRHI::CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc )
	{
		RHIIndexBufferRef ib = RHIResource::Create<OpenGLIndexBuffer>();
		CHECK( ib->Commit( &a_Desc ) );
		return ib;
	}

	RHIVertexBufferRef OpenGLRHI::CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc )
	{
		RHIVertexBufferRef vb = RHIResource::Create<OpenGLVertexBuffer>();
		CHECK( vb->Commit( &a_Desc ) );
		return vb;
	}

	RHIPipelineStateRef OpenGLRHI::CreatePipelineState( const RHIPipelineStateDescriptor& a_Desc )
	{
		return RHIPipelineStateRef();
	}

	RHICommandListRef OpenGLRHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		return RHICommandListRef();
	}

	RHIShaderModuleRef OpenGLRHI::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		return RHIShaderModuleRef();
	}

	RHIShaderBindingLayoutRef OpenGLRHI::CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc )
	{
		return RHIShaderBindingLayoutRef();
	}

	RHISwapChainRef OpenGLRHI::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		RHISwapChainRef sc = RHIResource::Create<OpenGLSwapChain>();
		CHECK( sc->Commit( &a_Desc ) );
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
			s_OpenGLDebugMessages.EmplaceBack( "Error: " + String( a_Message ) );
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			s_OpenGLDebugMessages.EmplaceBack( "Warning: " + String( a_Message ) );
			break;
		case GL_DEBUG_SEVERITY_LOW:
			s_OpenGLDebugMessages.EmplaceBack( "Info: " + String( a_Message ) );
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			s_OpenGLDebugMessages.EmplaceBack( "Debug: " + String( a_Message ) );
			break;
		}
	}

	void OpenGLRHI::DumpDebug()
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