#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	RHIBindingLayout_OpenGLImpl::RHIBindingLayout_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHIBindingLayout( a_Device, a_Desc )
	{
	}

	RHIBindingSet_OpenGLImpl::RHIBindingSet_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHIBindingSet( a_Device, a_Desc )
	{

	}

}
