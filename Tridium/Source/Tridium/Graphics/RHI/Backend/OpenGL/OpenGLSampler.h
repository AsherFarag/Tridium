#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	class OpenGLSampler final : public RHISampler
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );

		bool Commit( const void* a_Params ) override
		{
			const auto* desc = ParamsToDescriptor<RHISamplerDescriptor>( a_Params );
			if ( desc == nullptr )
			{
				return false;
			}
			OpenGL3::GenSamplers( 1, &m_SamplerID );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_MIN_FILTER, ToOpenGL::GetFilter( desc->Filter ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_MAG_FILTER, ToOpenGL::GetFilter( desc->Filter ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_S, ToOpenGL::GetAddressMode( desc->AddressU ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_T, ToOpenGL::GetAddressMode( desc->AddressV ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_R, ToOpenGL::GetAddressMode( desc->AddressW ) );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_LOD_BIAS, desc->MipLODBias );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_MAX_ANISOTROPY, desc->MaxAnisotropy );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_COMPARE_FUNC, ToOpenGL::GetComparisonFunc( desc->ComparisonFunc ) );
			OpenGL3::SamplerParameterfv( m_SamplerID, GL_TEXTURE_BORDER_COLOR, &desc->BorderColor.r );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_MIN_LOD, desc->MinLOD );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_MAX_LOD, desc->MaxLOD );
			return true;
		}

		bool Release() override
		{
			glDeleteSamplers( 1, &m_SamplerID );
			return true;
		}

		bool IsValid() const override { return m_SamplerID != 0; }
		const void* NativePtr() const override { return &m_SamplerID; }

		GLuint GetGLHandle() const { return m_SamplerID; }

	private:
		GLuint m_SamplerID = 0;
	};

} // namespace Tridium