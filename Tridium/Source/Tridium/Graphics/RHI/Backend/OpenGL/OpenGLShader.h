#pragma once
#include "OpenGLCommon.h"
#include <Tridium/Containers/Map.h>

namespace Tridium {


	class OpenGLShaderModule : public RHIShaderModule
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override { return m_ShaderID != 0; }
		const void* NativePtr() const override { return &m_ShaderID; }

		GLuint GetGLHandle() const { return m_ShaderID; }

	private:
		GLuint m_ShaderID = 0;
	};

} // namespace Tridium