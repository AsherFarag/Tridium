#pragma once
#include "OpenGL4.h"
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
		bool IsValid() const override;
		const void* NativePtr() const override;
	};

} // namespace Tridium