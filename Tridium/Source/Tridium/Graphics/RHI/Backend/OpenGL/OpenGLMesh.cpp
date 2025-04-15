#include "tripch.h"
#include "OpenGLMesh.h"

namespace Tridium {

#if 0

    //////////////////////////////////////////////////////////////////////////
	// OpenGL Index Buffer
	//////////////////////////////////////////////////////////////////////////

    bool OpenGLIndexBuffer::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHIIndexBufferDescriptor>( a_Params );
		if ( !desc )
			return false;

		OpenGL4::CreateBuffers( 1, &m_Buffer );
		GLState::BindIndexBuffer( m_Buffer );
    #if RHI_DEBUG_ENABLED
		OpenGL4::ObjectLabel( GL_BUFFER, m_Buffer, desc->Name.size(), desc->Name.data() );
    #endif
		GLState::BindIndexBuffer( 0 );

		return m_Buffer != 0;
    }

    bool OpenGLIndexBuffer::Release()
    {
        if ( m_Buffer )
		{
			OpenGL1::DeleteBuffers( 1, &m_Buffer );
			m_Buffer = 0;
		}

		return true;
    }

	size_t OpenGLIndexBuffer::GetSizeInBytes() const
	{
		GLint prevBinding = 0;
		OpenGL1::GetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &prevBinding );
		OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_Buffer );
		GLint size = 0;
		OpenGL1::GetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size );
		OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, prevBinding );
		return size;
	}

	bool OpenGLIndexBuffer::Read( Array<Byte>& o_Data, size_t a_SrcOffset )
	{
		return false;
	}

	bool OpenGLIndexBuffer::IsReadable() const
	{
		return false;
	}

	bool OpenGLIndexBuffer::Write( const Span<const Byte>& a_Data, size_t a_DstOffset )
	{
		GLint prevBinding = 0;
		OpenGL1::GetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &prevBinding );
		OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_Buffer );
		GLint bufferSize = 0;
		OpenGL1::GetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize );
		if ( a_DstOffset + a_Data.size_bytes() > bufferSize )
		{
			CHECK( a_DstOffset == 0 ); // THis is just some really crappy code to see tiles updating in the world basically.
			OpenGL1::BufferData( GL_ELEMENT_ARRAY_BUFFER, a_Data.size_bytes(), a_Data.data(), GL_STATIC_DRAW );
		}
		else
		{
			OpenGL1::BufferSubData( GL_ELEMENT_ARRAY_BUFFER, a_DstOffset, a_Data.size_bytes(), a_Data.data() );
		}
		OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, prevBinding );
		return true;
	}

	bool OpenGLIndexBuffer::IsWritable() const
	{
		return true;
	}

	bool OpenGLIndexBuffer::IsReady() const
	{
		return true;
	}

	void OpenGLIndexBuffer::Wait()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	// OpenGL Vertex Buffer
	//////////////////////////////////////////////////////////////////////////

    bool OpenGLVertexBuffer::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHIVertexBufferDescriptor>( a_Params );
		if ( !desc )
			return false;

		OpenGL4::CreateBuffers( 1, &m_Buffer );
		GLState::BindVertexBuffer( m_Buffer );

        #if RHI_DEBUG_ENABLED
		OpenGL4::ObjectLabel( GL_BUFFER, m_Buffer, desc->Name.size(), desc->Name.data() );
        #endif

		GLState::BindVertexBuffer( 0 );

		return m_Buffer != 0;
    }

    bool OpenGLVertexBuffer::Release()
    {
		if ( m_Buffer != 0 )
        {
            OpenGL1::DeleteBuffers( 1, &m_Buffer );
			m_Buffer = 0;
        }

		return true;
    }


    bool OpenGLVertexBuffer::IsValid() const
    {
        bool valid = m_Buffer != 0;

        TODO( "Is this necessary?" );
        if ( valid )
        {
            GLint prevBinding = 0;
            OpenGL1::GetIntegerv( GL_ARRAY_BUFFER_BINDING, &prevBinding );
            OpenGL1::BindBuffer( GL_ARRAY_BUFFER, m_Buffer );
            GLint size = 0;
            OpenGL1::GetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size );
            valid = size > 0;
            OpenGL1::BindBuffer( GL_ARRAY_BUFFER, prevBinding );
        }

        return valid;
    }

    size_t OpenGLVertexBuffer::GetSizeInBytes() const
    {
        GLint prevBinding = 0;
        OpenGL1::GetIntegerv( GL_ARRAY_BUFFER_BINDING, &prevBinding );
        OpenGL1::BindBuffer( GL_ARRAY_BUFFER, m_Buffer );
        GLint size = 0;
        OpenGL1::GetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size );
        OpenGL1::BindBuffer( GL_ARRAY_BUFFER, prevBinding );
        return size;
    }

    bool OpenGLVertexBuffer::Read( Array<Byte>& o_Data, size_t a_SrcOffset )
    {
        return false;
    }

    bool OpenGLVertexBuffer::IsReadable() const
    {
        return false;
    }

    bool OpenGLVertexBuffer::Write( const Span<const Byte>& a_Data, size_t a_DstOffset )
    {
		GLint prevBinding = 0;
		OpenGL1::GetIntegerv( GL_ARRAY_BUFFER_BINDING, &prevBinding );
		OpenGL1::BindBuffer( GL_ARRAY_BUFFER, m_Buffer );

        GLint bufferSize = 0;
        OpenGL1::GetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize );
        if ( a_DstOffset + a_Data.size_bytes() > bufferSize )
        {
            CHECK( a_DstOffset == 0 ); // THis is just some really crappy code to see tiles updating in the world basically.
            OpenGL1::BufferData( GL_ARRAY_BUFFER, a_Data.size_bytes(), a_Data.data(), GL_STATIC_DRAW );
        }
        else
        {
            OpenGL1::BufferSubData( GL_ARRAY_BUFFER, a_DstOffset, a_Data.size_bytes(), a_Data.data() );
        }

        OpenGL1::BindBuffer( GL_ARRAY_BUFFER, prevBinding );
		return true;
    }

    bool OpenGLVertexBuffer::IsWritable() const
    {
		return true;
    }

	bool OpenGLVertexBuffer::IsReady() const
	{
		return true;
	}

	void OpenGLVertexBuffer::Wait()
	{
	}

#endif
}
