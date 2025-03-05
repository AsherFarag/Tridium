#pragma once
#include "OpenGL4.h"
#include <Tridium/Graphics/RHI/RHIPipelineState.h>

namespace Tridium {

	class OpenGLGraphicsPipelineState final : public RHIGraphicsPipelineState
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );
		bool Commit( const void* a_Params ) override { ParamsToDescriptor<RHIGraphicsPipelineStateDescriptor>( a_Params ); return true; }
		bool Release() override { return true; }
		bool IsValid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }
	};

} // namespace Tridium