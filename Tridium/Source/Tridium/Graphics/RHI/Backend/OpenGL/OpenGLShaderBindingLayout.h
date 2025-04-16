#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( OpenGLShaderBindingLayout, RHIShaderBindingLayout )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( OpenGLShaderBindingLayout, ERHInterfaceType::OpenGL );
		bool Commit( const RHIShaderBindingLayoutDescriptor & a_Desc ) override
		{
			m_Desc = a_Desc;
			return true;
		}
		bool Release() override { return true; }
		bool IsValid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }
	};

} // namespace Tridium