#pragma once
#include "OpenGLCommon.h"
#include <Tridium/Graphics/RHI/RHIBuffer.h>

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBuffer_OpenGLImpl, RHIBuffer )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBuffer_OpenGLImpl, ERHInterfaceType::OpenGL );

		RHIBuffer_OpenGLImpl( const RHIBufferDescriptor & a_Desc, Span<const uint8_t> a_Data = {} )
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

		virtual ~RHIBuffer_OpenGLImpl() override = default;
		virtual bool Commit( const RHIBufferDescriptor& a_Desc ) override
		{
			TODO( "Implement RHIBuffer_OpenGLImpl::Commit" );
			m_Desc = a_Desc;
			return true;
		}
		virtual bool Release() override { BufferObj.Release(); return true; }
		virtual bool IsValid() const override { return BufferObj.Valid(); }
		virtual size_t GetSizeInBytes() const override
		{
			GLint size = 0;
			OpenGL1::GetBufferParameteriv( BufferObj, GL_BUFFER_SIZE, &size );
			return static_cast<size_t>( size );
		}
		virtual const void* NativePtr() const override { return BufferObj.NativePtr(); }

		OpenGL::GLBufferWrapper BufferObj{};
	};

}
