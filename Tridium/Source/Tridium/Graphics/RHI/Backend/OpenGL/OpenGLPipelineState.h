#pragma once
#include "OpenGLCommon.h"
#include <Tridium/Graphics/RHI/RHIPipelineState.h>

namespace Tridium {

	class OpenGLGraphicsPipelineState final : public RHIGraphicsPipelineState
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override { return nullptr; }

		GLuint GetShaderProgramID() const { return m_ShaderProgramID; }
		GLuint GetVAO() const { return m_VAO; }
		bool ApplyVertexLayoutToVAO( GLuint a_VAO );

	private:
		GLuint m_ShaderProgramID = 0;
		GLuint m_VAO = 0;
	};

} // namespace Tridium