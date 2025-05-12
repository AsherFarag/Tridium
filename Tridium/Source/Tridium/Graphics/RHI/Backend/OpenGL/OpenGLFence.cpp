#include "tripch.h"
#include "OpenGLFence.h"

namespace Tridium {

   RHIFence_OpenGLImpl::RHIFence_OpenGLImpl( const RHIFenceDescriptor& a_Desc )
	   : RHIFence( a_Desc )
    {
		ASSERT( a_Desc.Type == ERHIFenceType::CPUWaitOnly, "Only CPUWaitOnly fences are supported in OpenGL" );
    }

	bool RHIFence_OpenGLImpl::Release()
	{
		while ( !m_PendingFences.empty() )
		{
			OpenGL4::DeleteSync( m_PendingFences.front().second );
			m_PendingFences.pop_front();
		}
		return true;
	}

	bool RHIFence_OpenGLImpl::IsValid() const
	{
		return true;
	}

	const void* RHIFence_OpenGLImpl::NativePtr() const
	{
		return nullptr;
	}

	uint64_t RHIFence_OpenGLImpl::GetCompletedValue()
	{
		while ( !m_PendingFences.empty() )
		{
			auto& fence = m_PendingFences.front();

			if ( GL_ALREADY_SIGNALED == OpenGL4::ClientWaitSync( fence.second, GL_SYNC_FLUSH_COMMANDS_BIT, 0 ) )
			{
				OpenGL4::DeleteSync( fence.second );
				m_PendingFences.pop_front();
			}
			else
			{
				break;
			}
		}

		return m_PendingFences.empty() ? InvalidRHIFenceValue : m_PendingFences.front().first;
	}

	void RHIFence_OpenGLImpl::Signal( uint64_t a_Value )
	{
		ASSERT( Descriptor().Type == ERHIFenceType::General, "Signaling on a CPUWaitOnly fence, fence type must be ERHIFenceType::General" );
		ASSERT( false, "OpenGL does not support signaling fences" );
	}

	void RHIFence_OpenGLImpl::Wait( uint64_t a_Value )
	{
		//ASSERT( Descriptor().Type == ERHIFenceType::General, "Waiting on a CPUWaitOnly fence, fence type must be ERHIFenceType::General" );
		while ( GetCompletedValue() < a_Value )
		{
			// Wait for the fence to be signaled
		}
	}

}
