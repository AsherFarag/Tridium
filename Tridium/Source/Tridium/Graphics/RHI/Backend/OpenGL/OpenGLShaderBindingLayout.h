#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	class OpenGLShaderBindingLayout : public RHIShaderBindingLayout
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );
		bool Commit( const void* a_Params ) override { ParamsToDescriptor<RHIShaderBindingLayoutDescriptor>( a_Params ); return true; }
		bool Release() override { return true; }
		bool IsValid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }
	};

} // namespace Tridium