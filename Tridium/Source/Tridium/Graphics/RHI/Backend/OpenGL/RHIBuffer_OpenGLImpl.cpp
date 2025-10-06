#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	RHIBuffer_OpenGLImpl::RHIBuffer_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc, Span<const uint8_t> a_Data )
		: IRHIBuffer( a_Device, a_Desc )
	{
		BufferObj.Create();
		if ( !BufferObj.Valid() )
		{
			ASSERT( false, "Failed to create OpenGL buffer" );
			return;
		}

		// Vertex buffer
		if ( EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::VertexBuffer ) )
		{
			OpenGL1::BindBuffer( GL_ARRAY_BUFFER, BufferObj );
			OpenGL1::BufferData( GL_ARRAY_BUFFER,
								 a_Desc.Size,
								 a_Data.size() ? a_Data.data() : nullptr,
								 Translate( a_Desc.HeapType ) );
			OpenGL1::BindBuffer( GL_ARRAY_BUFFER, 0 );
		}
		// Index buffer
		else if ( EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::IndexBuffer ) )
		{
			OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, BufferObj );
			OpenGL1::BufferData( GL_ELEMENT_ARRAY_BUFFER,
								 a_Desc.Size,
								 a_Data.size() ? a_Data.data() : nullptr,
								 Translate( a_Desc.HeapType ) );
			OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		// Constant buffer
		else if ( EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::ConstantBuffer ) )
		{
			OpenGL3::BindBuffer( GL_UNIFORM_BUFFER, BufferObj );
			OpenGL3::BufferData( GL_UNIFORM_BUFFER,
								 a_Desc.Size,
								 a_Data.size() ? a_Data.data() : nullptr,
								 Translate( a_Desc.HeapType ) );
			OpenGL3::BindBuffer( GL_UNIFORM_BUFFER, 0 );
		}
		// Structured/Raw/Other buffer
		else if ( EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::ShaderResource )
			   || EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::UnorderedAccess )
			   || EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::IndirectArgument )
			   || a_Desc.Type == ERHIBufferType::Structured
			   || a_Desc.Type == ERHIBufferType::Raw
			   || a_Desc.Type == ERHIBufferType::Formatted )
		{
			OpenGL1::BindBuffer( GL_SHADER_STORAGE_BUFFER, BufferObj );
			OpenGL1::BufferData( GL_SHADER_STORAGE_BUFFER,
								 a_Desc.Size,
								 a_Data.size() ? a_Data.data() : nullptr,
								 Translate( a_Desc.HeapType ) );
			OpenGL1::BindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );
		}
		else
		{
			ASSERT( false, "Invalid buffer type!" );
		}

		BufferObj.SetName( a_Desc.Name );
	}

	const void* RHIBuffer_OpenGLImpl::Map()
	{
		RHI_DEV_CHECK( m_Desc.HeapType == ERHIHeapType::Staging, "Buffer '{}' must be created with staging heap to be mapped", m_Desc.Name );
		RHI_DEV_CHECK( Valid(), "Buffer '{}' is not valid", m_Desc.Name );

		void* mappedPtr = nullptr;
		OpenGL1::BindBuffer( GL_ARRAY_BUFFER, BufferObj );
		mappedPtr = OpenGL1::MapBuffer( GL_ARRAY_BUFFER, GL_READ_ONLY );
		OpenGL1::BindBuffer( GL_ARRAY_BUFFER, 0 );

		if ( !ASSERT( mappedPtr != nullptr, "Failed to map buffer '{}'", m_Desc.Name ) )
		{
			return nullptr;
		}

		return mappedPtr;
	}

	void RHIBuffer_OpenGLImpl::Unmap()
	{
		RHI_DEV_CHECK( m_Desc.HeapType == ERHIHeapType::Staging, "Buffer '{}' must be created with staging heap to be unmapped", m_Desc.Name );
		RHI_DEV_CHECK( Valid(), "Buffer '{}' is not valid", m_Desc.Name );

		OpenGL1::BindBuffer( GL_ARRAY_BUFFER, BufferObj );

		if ( !ASSERT( OpenGL1::UnmapBuffer( GL_ARRAY_BUFFER ) == GL_TRUE, "Failed to unmap buffer '{}'", m_Desc.Name ) )
		{
			return;
		}

		OpenGL1::BindBuffer( GL_ARRAY_BUFFER, 0 );
	}

}