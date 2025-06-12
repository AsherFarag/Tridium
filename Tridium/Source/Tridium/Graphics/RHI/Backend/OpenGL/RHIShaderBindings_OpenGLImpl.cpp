#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	RHIBindingLayout_OpenGLImpl::RHIBindingLayout_OpenGLImpl( const DescriptorType& a_Desc )
		: IRHIBindingLayout( a_Desc )
	{
	}

	RHIBindingSet_OpenGLImpl::RHIBindingSet_OpenGLImpl( const DescriptorType& a_Desc )
		: IRHIBindingSet( a_Desc )
	{

	}

}
