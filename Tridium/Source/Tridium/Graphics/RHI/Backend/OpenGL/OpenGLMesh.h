#pragma once
#include "OpenGLCommon.h"
#include <Tridium/Graphics/RHI/RHIMesh.h>

namespace Tridium {

	class OpenGLIndexBuffer final : public RHIIndexBuffer
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;
	};

	class OpenGLVertexBuffer final : public RHIVertexBuffer
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool Write( const Span<const Byte>& a_Data, size_t a_DstOffset = 0 ) override;
		bool IsWritable() const override;
		bool Map( size_t a_Offset, int64_t a_Length, ERHIMappingMode a_MappingMode ) override;
		bool Unmap() override;
		bool IsMappable() const override;
		bool IsValid() const override;
		size_t GetSizeInBytes() const override;
		const void* NativePtr() const override { return &m_Buffer; }

		//=====================================================================
		GLuint GetGLHandle() const { return m_Buffer; }

	private:
		GLuint m_Buffer = 0;
	};

} // namespace Tridium