#include "tripch.h"
#include "OpenGLSwapChain.h"


namespace Tridium {

    bool OpenGLSwapChain::Present()
    {
		if ( Window )
        {
            glfwSwapBuffers( Window );
            return true;
        }

        return false;
    }

	RHITextureRef OpenGLSwapChain::GetBackBuffer()
	{
		return RHITextureRef();
	}

    bool OpenGLSwapChain::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHISwapChainDescriptor>( a_Params );
		if ( !desc )
		{
			return false;
		}

		Window = glfwGetCurrentContext();
		if ( !Window )
		{
			return false;
		}

		glfwSwapInterval( desc->Flags.HasFlag( ERHISwapChainFlags::UseVSync ) ? 1 : 0 );
		return true;
    }

	bool OpenGLSwapChain::Release()
	{
		Window = nullptr;
		return true;
	}

} // namespace Tridium