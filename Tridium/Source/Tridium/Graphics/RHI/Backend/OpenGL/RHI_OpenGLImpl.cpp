#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	GLuint GLResourceCache::GetOrCreateSampler( const RHISampler& a_Sampler, bool a_IsDepth )
	{
		if ( a_IsDepth )
		{
			auto it = DepthSamplers.find( a_Sampler );
			if ( it != DepthSamplers.end() )
				return it->second;
		}
		else
		{
			auto it = Samplers.find( a_Sampler );
			if ( it != Samplers.end() )
				return it->second;
		}

		const auto samplerProps = GetOpenGLRHI()->GetGPUInfo().Properties.Sampler;

		GLSamplerWrapper& sampler = a_IsDepth ? DepthSamplers[a_Sampler] : Samplers[a_Sampler];
		sampler.Create();
		bool anisotripicFiltering;
		bool comparisonFiltering;
		GLenum minFilter, magFilter, mipFilter;
		Translate( a_Sampler.Filter, minFilter, magFilter, mipFilter, anisotripicFiltering, comparisonFiltering );

		OpenGL3::SamplerParameteri( sampler, GL_TEXTURE_MAG_FILTER, magFilter );

		GLenum minMipFilter = 0;
		if ( minFilter == GL_NEAREST && mipFilter == GL_NEAREST )
			minMipFilter = GL_NEAREST_MIPMAP_NEAREST;
		else if ( minFilter == GL_LINEAR && mipFilter == GL_NEAREST )
			minMipFilter = GL_LINEAR_MIPMAP_NEAREST;
		else if ( minFilter == GL_NEAREST && mipFilter == GL_LINEAR )
			minMipFilter = GL_NEAREST_MIPMAP_LINEAR;
		else if ( minFilter == GL_LINEAR && mipFilter == GL_LINEAR )
			minMipFilter = GL_LINEAR_MIPMAP_LINEAR;
		else
			ASSERT( false, "Unsupported sampler filter combination" );
		OpenGL3::SamplerParameteri( sampler, GL_TEXTURE_MIN_FILTER, minMipFilter );

		GLenum wrapModes[3] = {
			Translate( a_Sampler.AddressU ),
			Translate( a_Sampler.AddressV ),
			Translate( a_Sampler.AddressW )
		};

		for ( GLenum& wrapMode : wrapModes)
		{
			if ( samplerProps.BorderSamplingModeSupported == false && wrapMode == GL_CLAMP_TO_BORDER )
			{
				LOG( LogCategory::RHI, Error, "Sampler address mode GL_CLAMP_TO_BORDER is not supported by the GPU. Using GL_CLAMP_TO_EDGE instead." );
				wrapMode = GL_CLAMP_TO_EDGE; // Fallback to GL_CLAMP_TO_EDGE if border sampling is not supported
			}
		}

		OpenGL3::SamplerParameteri( sampler, GL_TEXTURE_WRAP_S, wrapModes[0] );
		OpenGL3::SamplerParameteri( sampler, GL_TEXTURE_WRAP_T, wrapModes[1] );
		OpenGL3::SamplerParameteri( sampler, GL_TEXTURE_WRAP_R, wrapModes[2] );

		if ( samplerProps.LODBiasSupported )
			OpenGL3::SamplerParameterf( sampler, GL_TEXTURE_LOD_BIAS, a_Sampler.MipLODBias );
		else if ( a_Sampler.MipLODBias )
			LOG( LogCategory::RHI, Warn, "LOD Bias is not supported" );

		if ( anisotripicFiltering && a_Sampler.MaxAnisotropy > 1 )
			OpenGL3::SamplerParameteri( sampler, GL_TEXTURE_MAX_ANISOTROPY, Math::Min<uint8_t>( a_Sampler.MaxAnisotropy, samplerProps.MaxAnisotropy ) );

		OpenGL3::SamplerParameteri( sampler, GL_TEXTURE_COMPARE_MODE, comparisonFiltering && a_IsDepth ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE );

		if ( samplerProps.BorderSamplingModeSupported )
		{
			// Convert border color from float [0, 1] to 16-bit unsigned integers [0, 65535]
			GLfloat borderColor[4] = {
				a_Sampler.BorderColor[0],
				a_Sampler.BorderColor[1],
				a_Sampler.BorderColor[2],
				a_Sampler.BorderColor[3]
			};
			OpenGL3::SamplerParameterfv( sampler, GL_TEXTURE_BORDER_COLOR, borderColor );
		}
		else if ( a_Sampler.BorderColor != RHISampler{}.BorderColor )
		{
			LOG( LogCategory::RHI, Warn, "Border color sampling is not supported" );
		}

		OpenGL3::SamplerParameteri( sampler, GL_TEXTURE_COMPARE_FUNC, Translate( a_Sampler.ComparisonFunc ) );

		OpenGL3::SamplerParameterf( sampler, GL_TEXTURE_MIN_LOD, a_Sampler.MinLOD );
		OpenGL3::SamplerParameterf( sampler, GL_TEXTURE_MAX_LOD, a_Sampler.MaxLOD );

		sampler.SetName( "Cached Sampler" );

		LOG( LogCategory::RHI, Info, "Created sampler" );

		return sampler;
	}

}
