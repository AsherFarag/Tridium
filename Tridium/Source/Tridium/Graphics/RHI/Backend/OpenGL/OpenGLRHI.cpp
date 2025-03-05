#include "tripch.h"
#include "OpenGLDynamicRHI.h"
#include "OpenGLCommon.h"
#include <iostream>

// Resources
#include "OpenGLSampler.h"
#include "OpenGLTexture.h"
#include "OpenGLMesh.h"
#include "OpenGLPipelineState.h"
#include "OpenGLSwapChain.h"
#include "OpenGLResourceAllocator.h"
#include "OpenGLCommandList.h"
#include "OpenGLShader.h"

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

		return true;
	}

	bool OpenGLDynamicRHI::Shutdown()
	{
		return true;
	}

	bool OpenGLDynamicRHI::ExecuteCommandList( RHICommandListRef a_CommandList )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// FENCE FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	RHIFence OpenGLDynamicRHI::CreateFence() const
	{
		return RHIFence();
	}

	ERHIFenceState OpenGLDynamicRHI::GetFenceState( RHIFence a_Fence ) const
	{
		return ERHIFenceState();
	}

	void OpenGLDynamicRHI::FenceSignal( RHIFence a_Fence )
	{
	}

	//////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHISamplerRef OpenGLDynamicRHI::CreateSampler( const RHISamplerDescriptor& a_Desc, const RHIResourceAllocatorRef& a_Allocator )
	{
		RHISamplerRef sampler = RHIResource::Create<OpenGLSampler>();
		CHECK( sampler->Commit( &a_Desc ) );
		return sampler;
	}

	RHITextureRef OpenGLDynamicRHI::CreateTexture( const RHITextureDescriptor& a_Desc, const RHIResourceAllocatorRef& a_Allocator )
	{
		RHITextureRef tex = RHIResource::Create<OpenGLTexture>();
		CHECK( tex->Commit( &a_Desc ) );
		return tex;
	}

	RHIIndexBufferRef OpenGLDynamicRHI::CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc )
	{
		RHIIndexBufferRef ib = RHIResource::Create<OpenGLIndexBuffer>();
		CHECK( ib->Commit( &a_Desc ) );
		return ib;
	}

	RHIVertexBufferRef OpenGLDynamicRHI::CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc )
	{
		RHIVertexBufferRef vb = RHIResource::Create<OpenGLVertexBuffer>();
		CHECK( vb->Commit( &a_Desc ) );
		return vb;
	}

	RHIGraphicsPipelineStateRef OpenGLDynamicRHI::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc )
	{
		RHIGraphicsPipelineStateRef pso = RHIResource::Create<OpenGLGraphicsPipelineState>();
		CHECK( pso->Commit( &a_Desc ) );
		return pso;
	}

	RHICommandListRef OpenGLDynamicRHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		RHICommandListRef cl = RHIResource::Create<OpenGLCommandList>();
		CHECK( cl->Commit( &a_Desc ) );
		return cl;
	}

	RHIShaderModuleRef OpenGLDynamicRHI::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		RHIShaderModuleRef shader = RHIResource::Create<OpenGLShaderModule>();
		CHECK( shader->Commit( &a_Desc ) );
		return shader;
	}

	RHIShaderBindingLayoutRef OpenGLDynamicRHI::CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc )
	{
		return RHIShaderBindingLayoutRef();
	}

	RHIResourceAllocatorRef OpenGLDynamicRHI::CreateResourceAllocator( const RHIResourceAllocatorDescriptor& a_Desc )
	{
		RHIResourceAllocatorRef ra = RHIResource::Create<OpenGLResourceAllocator>();
		CHECK( ra->Commit( &a_Desc ) );
		return ra;
	}

	RHISwapChainRef OpenGLDynamicRHI::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
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