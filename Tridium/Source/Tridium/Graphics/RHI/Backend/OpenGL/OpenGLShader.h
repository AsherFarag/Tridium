#pragma once
#include "OpenGLCommon.h"
#include <Tridium/Containers/Map.h>

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIShaderModule_OpenGLImpl, RHIShaderModule )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIShaderModule_OpenGLImpl, ERHInterfaceType::OpenGL );

		RHIShaderModule_OpenGLImpl( const DescriptorType& a_Desc );
		bool Release() override;
		bool IsValid() const override { return m_ShaderID != 0; }
		const void* NativePtr() const override { return &m_ShaderID; }

		GLuint GetGLHandle() const { return m_ShaderID; }

	private:
		GLuint m_ShaderID = 0;
	};

} // namespace Tridium