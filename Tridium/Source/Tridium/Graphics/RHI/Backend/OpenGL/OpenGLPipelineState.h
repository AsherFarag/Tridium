#pragma once
#include "OpenGL4.h"
#include <Tridium/Graphics/RHI/RHIPipelineState.h>

namespace Tridium {

	class OpenGLPipelineState final : public RHIPipelineState
	{
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;
	private:
		GLuint m_Handle = 0;
	};

} // namespace Tridium