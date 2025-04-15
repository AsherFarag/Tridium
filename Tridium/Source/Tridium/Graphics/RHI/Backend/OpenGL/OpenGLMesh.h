#pragma once
#include "OpenGLCommon.h"
#include <Tridium/Graphics/RHI/RHIMesh.h>

namespace Tridium {

#if 0

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( OpenGLIndexBuffer, RHIIndexBuffer )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( OpenGLIndexBuffer, ERHInterfaceType::OpenGL );

		virtual bool Commit( const void* a_Params ) override;
		virtual bool Release() override;
		virtual bool IsValid() const override { return m_Buffer != 0; }
		virtual size_t GetSizeInBytes() const override;
		virtual const void* NativePtr() const override { return &m_Buffer; }
		virtual bool Read( Array<Byte>&o_Data, size_t a_SrcOffset = 0 ) override;
		virtual bool IsReadable() const override;
		virtual bool Write( const Span<const Byte>&a_Data, size_t a_DstOffset = 0 ) override;
		virtual bool IsWritable() const override;
		virtual bool IsReady() const override;
		virtual void Wait() override;

		//=====================================================================
		GLuint GetGLHandle() const { return m_Buffer; }
	private:
		GLuint m_Buffer = 0;
	};

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( OpenGLVertexBuffer, RHIVertexBuffer )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( OpenGLVertexBuffer, ERHInterfaceType::OpenGL );

		virtual bool Commit( const void* a_Params ) override;
		virtual bool Release() override;
		virtual bool IsValid() const override;
		virtual size_t GetSizeInBytes() const override;
		virtual const void* NativePtr() const override { return &m_Buffer; }
		virtual bool Read( Array<Byte>& o_Data, size_t a_SrcOffset = 0 ) override;
		virtual bool IsReadable() const override;
		virtual bool Write( const Span<const Byte>& a_Data, size_t a_DstOffset = 0 ) override;
		virtual bool IsWritable() const override;
		virtual bool IsReady() const override;
		virtual void Wait() override;

		//=====================================================================
		GLuint GetGLHandle() const { return m_Buffer; }

	private:
		GLuint m_Buffer = 0;
	};

#endif

} // namespace Tridium