#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	class OpenGLSwapChain : public RHISwapChain
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override { return Window != nullptr; }
		const void* NativePtr() const override { return Window; }
		bool Present() override;
		RHITextureRef GetBackBuffer() override;

		GLFWwindow* Window = nullptr;
	};

} // namespace Tridium