#include "tripch.h"
#include "OpenGLRHI.h"
#include "OpenGLCommon.h"
#include <iostream>

// Resources
#include "OpenGLTexture.h"
//#include "OpenGLIndexBuffer.h"
//#include "OpenGLVertexBuffer.h"
//#include "OpenGLPipelineState.h"

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

	#if RHI_DEBUG_ENABLED
		if ( a_Config.UseDebug )
		{
			// Enable OpenGL debug output
			glEnable( GL_DEBUG_OUTPUT );
			glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
			glDebugMessageCallback( DebugCallback, nullptr );
			glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
		}
	#endif
		return status;
	}

	bool OpenGLRHI::Shutdown()
	{
		return true;
	}

	bool OpenGLRHI::Present()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		if ( GLFWwindow* window = glfwGetCurrentContext() )
		{
			glfwSwapBuffers( window );
			return true;
		}

		return false;
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

	RHITextureRef OpenGLRHI::CreateTexture( const RHITextureDescriptor& a_Desc )
	{
		RHITextureRef tex = RHIResource::Create<OpenGLTexture>();
		CHECK( tex->Commit( &a_Desc ) );
		return tex;
	}

	RHIIndexBufferRef OpenGLRHI::CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc )
	{
		return RHIIndexBufferRef();
	}

	RHIVertexBufferRef OpenGLRHI::CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc )
	{
		return RHIVertexBufferRef();
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