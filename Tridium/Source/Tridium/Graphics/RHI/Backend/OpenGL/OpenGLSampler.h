#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHISampler_OpenGLImpl, RHISampler )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHISampler_OpenGLImpl, ERHInterfaceType::OpenGL );

		RHISampler_OpenGLImpl( const DescriptorType& a_Desc )
			: RHISampler( a_Desc )
		{
			OpenGL3::GenSamplers( 1, &m_SamplerID );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_MIN_FILTER, ToOpenGL::GetFilter( a_Desc.Filter ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_MAG_FILTER, ToOpenGL::GetFilter( a_Desc.Filter ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_S, ToOpenGL::GetAddressMode( a_Desc.AddressU ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_T, ToOpenGL::GetAddressMode( a_Desc.AddressV ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_R, ToOpenGL::GetAddressMode( a_Desc.AddressW ) );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_LOD_BIAS, a_Desc.MipLODBias );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_MAX_ANISOTROPY, a_Desc.MaxAnisotropy );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_COMPARE_FUNC, ToOpenGL::GetComparisonFunc( a_Desc.ComparisonFunc ) );
			OpenGL3::SamplerParameterfv( m_SamplerID, GL_TEXTURE_BORDER_COLOR, &a_Desc.BorderColor.r );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_MIN_LOD, a_Desc.MinLOD );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_MAX_LOD, a_Desc.MaxLOD );
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