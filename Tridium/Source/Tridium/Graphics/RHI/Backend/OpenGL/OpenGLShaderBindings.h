#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBindingLayout_OpenGLImpl, RHIBindingLayout )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBindingLayout_OpenGLImpl, ERHInterfaceType::OpenGL );

		RHIBindingLayout_OpenGLImpl( const DescriptorType & a_Desc )
			: RHIBindingLayout( a_Desc )
		{}

		bool Release() override { return true; }
		bool IsValid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }
	};

} // namespace Tridium