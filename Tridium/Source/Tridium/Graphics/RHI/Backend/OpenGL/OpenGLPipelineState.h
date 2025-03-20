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
		GLint TryGetUniformLocation( hash_t a_NameHash ) const
		{
			auto it = m_UnifromLocations.find( a_NameHash );
			if ( it != m_UnifromLocations.end() )
			{
				return it->second;
			}
			return -1;
		}


	private:
		GLuint m_ShaderProgramID = 0;
		GLuint m_VAO = 0;
		UnorderedMap<hash_t, GLint> m_UnifromLocations;
		UnorderedMap<StringView, Array<StringView>> m_CombinedSamplers; // Map of texture name to an array of sampler name
	};

} // namespace Tridium