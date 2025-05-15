#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	RHIBuffer_OpenGLImpl::RHIBuffer_OpenGLImpl( const DescriptorType& a_Desc, Span<const uint8_t> a_Data )
		: RHIBuffer( a_Desc )
	{
		BufferObj.Create();
		if ( !BufferObj.Valid() )
		{
			ASSERT( false, "Failed to create OpenGL buffer" );
			return;
		}

		if ( a_Data.size() == 0 )
		{
			return;
		}

		// Vertex buffer
		if ( EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::VertexBuffer ) )
		{
			GLState::BindVertexBuffer( BufferObj );
			OpenGL1::BufferData( GL_ARRAY_BUFFER, a_Desc.Size, a_Data.data(), GL_STATIC_DRAW );
			GLState::BindVertexBuffer( 0 );
		}
		// Index buffer
		else if ( EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::IndexBuffer ) )
		{
			GLState::BindIndexBuffer( BufferObj );
			OpenGL1::BufferData( GL_ELEMENT_ARRAY_BUFFER, a_Desc.Size, a_Data.data(), GL_STATIC_DRAW );
			GLState::BindIndexBuffer( 0 );
		}
		else
		{
			NOT_IMPLEMENTED;
		}

		BufferObj.SetName( a_Desc.Name );
	}

	size_t RHIBuffer_OpenGLImpl::GetSizeInBytes() const
	{
		GLint size = 0;
		OpenGL1::GetBufferParameteriv( BufferObj, GL_BUFFER_SIZE, &size );
		return Cast<size_t>(size);
	}

}