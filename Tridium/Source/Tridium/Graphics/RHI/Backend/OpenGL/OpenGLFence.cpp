#include "tripch.h"
#include "OpenGLFence.h"

namespace Tridium {

    bool OpenGLFence::Commit( const RHIFenceDescriptor& a_Desc )
    {
		m_Desc = a_Desc;
		if ( a_Desc.Type != ERHIFenceType::CPUWaitOnly )
		{
			ASSERT( false, "Only CPUWaitOnly fences are supported in OpenGL" );
			return false;
		}

		return true;
    }

	bool OpenGLFence::Release()
	{
		while ( !m_PendingFences.empty() )
		{
			OpenGL4::DeleteSync( m_PendingFences.front().second );
			m_PendingFences.pop_front();
		}
		return true;
	}

	bool OpenGLFence::IsValid() const
	{
		return true;
	}

	const void* OpenGLFence::NativePtr() const
	{
		return nullptr;
	}

	uint64_t OpenGLFence::GetCompletedValue()
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

	void OpenGLFence::Signal( uint64_t a_Value )
	{
		ASSERT( Descriptor().Type == ERHIFenceType::General, "Signaling on a CPUWaitOnly fence, fence type must be ERHIFenceType::General" );
		ASSERT( false, "OpenGL does not support signaling fences" );
	}

	void OpenGLFence::Wait( uint64_t a_Value )
	{
		//ASSERT( Descriptor().Type == ERHIFenceType::General, "Waiting on a CPUWaitOnly fence, fence type must be ERHIFenceType::General" );
		while ( GetCompletedValue() < a_Value )
		{
			// Wait for the fence to be signaled
		}
	}

}
