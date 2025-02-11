#include "tripch.h"
#include "OpenGLMesh.h"

namespace Tridium {

    //////////////////////////////////////////////////////////////////////////
	// OpenGLIndexBuffer
	//////////////////////////////////////////////////////////////////////////

    bool OpenGLIndexBuffer::Commit( const void* a_Params )
    {
        return false;
    }

    bool OpenGLIndexBuffer::Release()
    {
        return false;
    }

    bool OpenGLIndexBuffer::IsValid() const
    {
        return false;
    }

    const void* OpenGLIndexBuffer::NativePtr() const
    {
        return nullptr;
    }



	//////////////////////////////////////////////////////////////////////////
	// OpenGLIndexBuffer
	//////////////////////////////////////////////////////////////////////////

    bool OpenGLVertexBuffer::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHIVertexBufferDescriptor>( a_Params );

		OpenGL3::GenBuffers( 1, &m_Buffer );
		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, m_Buffer );
		OpenGL3::BufferData( GL_ARRAY_BUFFER, desc->InitialData.size(), desc->InitialData.data(), GL_STATIC_DRAW );

        #if RHI_DEBUG_ENABLED
		OpenGL4::ObjectLabel( GL_BUFFER, m_Buffer, desc->Name.size(), desc->Name.data() );
        #endif

		return m_Buffer != 0;
    }

    bool OpenGLVertexBuffer::Release()
    {
		if ( m_Buffer != 0 )
        {
            OpenGL3::DeleteBuffers( 1, &m_Buffer );
			m_Buffer = 0;
        }

		return true;
    }

    bool OpenGLVertexBuffer::Write( const Span<const Byte>& a_Data, size_t a_DstOffset )
    {
		GLint prevBinding = 0;
		OpenGL3::GetIntegerv( GL_ARRAY_BUFFER_BINDING, &prevBinding );
		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, m_Buffer );

        GLint bufferSize = 0;
        OpenGL3::GetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize );
        if ( a_DstOffset + a_Data.size_bytes() > bufferSize )
        {
            CHECK( a_DstOffset == 0 ); // THis is just some really crappy code to see tiles updating in the world basically.
            OpenGL3::BufferData( GL_ARRAY_BUFFER, a_Data.size_bytes(), a_Data.data(), GL_STATIC_DRAW );
        }
        else
        {
            OpenGL3::BufferSubData( GL_ARRAY_BUFFER, a_DstOffset, a_Data.size_bytes(), a_Data.data() );
        }

        OpenGL3::BindBuffer( GL_ARRAY_BUFFER, prevBinding );
		return true;
    }

    bool OpenGLVertexBuffer::IsWritable() const
    {
		return true;
    }

    bool OpenGLVertexBuffer::Map( size_t a_Offset, int64_t a_Length, ERHIMappingMode a_MappingMode )
    {
        //void* addr = nullptr;
        //GLint prevBinding = 0;
        //OpenGL3::GetIntegerv( GL_ARRAY_BUFFER_BINDING, &prevBinding );
        //OpenGL3::BindBuffer( GL_ARRAY_BUFFER, m_Buffer );

		return false;
    }

	bool OpenGLVertexBuffer::Unmap()
	{
		//GLint prevBinding = 0;
		//OpenGL3::GetIntegerv( GL_ARRAY_BUFFER_BINDING, &prevBinding );
		//OpenGL3::BindBuffer( GL_ARRAY_BUFFER, m_Buffer );

		return false;
	}

	bool OpenGLVertexBuffer::IsMappable() const
	{
		return false;
	}

    bool OpenGLVertexBuffer::IsValid() const
    {
		bool valid = m_Buffer != 0;

		TODO( "Is this necessary?" );
        if ( valid )
        {
            GLint prevBinding = 0;
            OpenGL3::GetIntegerv( GL_ARRAY_BUFFER_BINDING, &prevBinding );
            OpenGL3::BindBuffer( GL_ARRAY_BUFFER, m_Buffer );
            GLint size = 0;
            OpenGL3::GetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size );
            valid = size > 0;
            OpenGL3::BindBuffer( GL_ARRAY_BUFFER, prevBinding );
        }

		return valid;
    }

    size_t OpenGLVertexBuffer::GetSizeInBytes() const
    {
        GLint prevBinding = 0;
		OpenGL3::GetIntegerv( GL_ARRAY_BUFFER_BINDING, &prevBinding );
		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, m_Buffer );
		GLint size = 0;
		OpenGL3::GetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size );
		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, prevBinding );
		return size;
    }

}
