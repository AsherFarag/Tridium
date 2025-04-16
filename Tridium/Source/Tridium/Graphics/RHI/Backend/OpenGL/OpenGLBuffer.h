#pragma once
#include "OpenGLCommon.h"
#include <Tridium/Graphics/RHI/RHIBuffer.h>

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( OpenGLBuffer, RHIBuffer )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( OpenGLBuffer, ERHInterfaceType::OpenGL );

		OpenGLBuffer( const RHIBufferDescriptor & a_Desc )
			: RHIBuffer( a_Desc )
		{
		}

		virtual ~OpenGLBuffer() override = default;
		virtual bool Commit( const RHIBufferDescriptor& a_Desc ) override
		{
			TODO( "Implement OpenGLBuffer::Commit" );
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
