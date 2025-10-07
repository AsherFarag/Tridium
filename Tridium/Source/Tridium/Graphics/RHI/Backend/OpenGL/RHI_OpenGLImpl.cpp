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

		const auto samplerProps = GetOpenGLRHI()->GetGPUInfo().DeviceFeatures.Sampler;

		GLSamplerWrapper& sampler = a_IsDepth ? DepthSamplers[a_Sampler] : Samplers[a_Sampler];
		sampler.Create();

		// Convert RHISampler to GLSamplerDesc and apply it to the sampler object
		GLSamplerDesc desc = GLSamplerDesc::From( a_Sampler, GetOpenGLRHI() );
		desc.ApplyToSampler( sampler );

		sampler.SetName( "Cached Sampler" );

		LOG( LogCategory::RHI, Info, "Created sampler" );

		return sampler;
	}

	GLSamplerDesc GLSamplerDesc::From( const RHISampler& a_Sampler, DynamicRHI_OpenGLImpl* a_RHI )
	{
		const auto samplerProps = a_RHI->GetGPUInfo().DeviceFeatures.Sampler;

		const auto ConvertAddressMode = [&]( ERHISamplerAddressMode a_Mode ) -> GLenum
		{
			GLenum wrapMode = Translate( a_Mode );
			if ( samplerProps.BorderSamplingModeSupported == false && wrapMode == GL_CLAMP_TO_BORDER )
			{
				LOG( LogCategory::RHI, Error, "Sampler address mode GL_CLAMP_TO_BORDER is not supported by the GPU. Using GL_CLAMP_TO_EDGE instead." );
				wrapMode = GL_CLAMP_TO_EDGE; // Fallback to GL_CLAMP_TO_EDGE if border sampling is not supported
			}

			return wrapMode;
		};

		GLSamplerDesc result{};
		result.AddressU = ConvertAddressMode( a_Sampler.AddressU );
		result.AddressV = ConvertAddressMode( a_Sampler.AddressV );
		result.AddressW = ConvertAddressMode( a_Sampler.AddressW );

		GLenum minFilter, magFilter, mipFilter;
		bool useComparison = false;
		bool isAnisotropic = false;
		Translate( a_Sampler.Filter, minFilter, magFilter, mipFilter, isAnisotropic, useComparison );

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

		result.MinFilter = minMipFilter;
		result.MagFilter = magFilter;
		result.ComparisonFunc = useComparison ? Translate( a_Sampler.ComparisonFunc ) : GL_NONE;
		result.IsAnisotropic = isAnisotropic;

		if ( samplerProps.LODBiasSupported )
			result.MipLodBias = a_Sampler.MipLODBias;
		else if ( a_Sampler.MipLODBias )
			LOG( LogCategory::RHI, Warn, "LOD Bias is not supported" );

		if ( result.IsAnisotropic && a_Sampler.MaxAnisotropy > 1 )
			result.MaxAnisotropy = Math::Min<uint8_t>( a_Sampler.MaxAnisotropy, samplerProps.MaxAnisotropy );
		else
			result.MaxAnisotropy = 1.0f;

		if ( samplerProps.BorderSamplingModeSupported )
		{
			// Convert border color from float [0, 1] to 16-bit unsigned integers [0, 65535]
			result.BorderColor[0] = a_Sampler.BorderColor[0];
			result.BorderColor[1] = a_Sampler.BorderColor[1];
			result.BorderColor[2] = a_Sampler.BorderColor[2];
			result.BorderColor[3] = a_Sampler.BorderColor[3];
		}
		else if ( a_Sampler.BorderColor != RHISampler{}.BorderColor )
		{
			LOG( LogCategory::RHI, Warn, "Border color sampling is not supported" );
		}

		result.MinLod = a_Sampler.MinLOD;
		result.MaxLod = a_Sampler.MaxLOD;
		result.MipLodBias = a_Sampler.MipLODBias;

		return result;
	}

	void GLSamplerDesc::ApplyToTexture( GLenum a_Target, GLuint a_Obj )
	{
		OpenGL1::TexParameteri( a_Target, GL_TEXTURE_MIN_FILTER, MinFilter );
		OpenGL1::TexParameteri( a_Target, GL_TEXTURE_MAG_FILTER, MagFilter );
		OpenGL1::TexParameteri( a_Target, GL_TEXTURE_WRAP_S, AddressU );
		OpenGL1::TexParameteri( a_Target, GL_TEXTURE_WRAP_T, AddressV );
		OpenGL1::TexParameteri( a_Target, GL_TEXTURE_WRAP_R, AddressW );
		OpenGL1::TexParameterf( a_Target, GL_TEXTURE_LOD_BIAS, MipLodBias );
		OpenGL1::TexParameterf( a_Target, GL_TEXTURE_MIN_LOD, MinLod );
		OpenGL1::TexParameterf( a_Target, GL_TEXTURE_MAX_LOD, MaxLod );

		if ( IsAnisotropic )
		{
			OpenGL1::TexParameteri( a_Target, GL_TEXTURE_MAX_ANISOTROPY, MaxAnisotropy );
		}

		if ( AddressU == GL_CLAMP_TO_BORDER || AddressV == GL_CLAMP_TO_BORDER || AddressW == GL_CLAMP_TO_BORDER )
		{
			OpenGL1::TexParameterfv( a_Target, GL_TEXTURE_BORDER_COLOR, &BorderColor[0] );
		}

		if ( ComparisonFunc != GL_NONE )
		{
			OpenGL1::TexParameteri( a_Target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
			OpenGL1::TexParameteri( a_Target, GL_TEXTURE_COMPARE_FUNC, ComparisonFunc );
		}
		else
		{
			OpenGL1::TexParameteri( a_Target, GL_TEXTURE_COMPARE_MODE, GL_NONE );
		}
	}

	void GLSamplerDesc::ApplyToSampler( GLuint a_Obj )
	{
		OpenGL3::SamplerParameteri( a_Obj, GL_TEXTURE_MIN_FILTER, MinFilter );
		OpenGL3::SamplerParameteri( a_Obj, GL_TEXTURE_MAG_FILTER, MagFilter );
		OpenGL3::SamplerParameteri( a_Obj, GL_TEXTURE_WRAP_S, AddressU );
		OpenGL3::SamplerParameteri( a_Obj, GL_TEXTURE_WRAP_T, AddressV );
		OpenGL3::SamplerParameteri( a_Obj, GL_TEXTURE_WRAP_R, AddressW );
		OpenGL3::SamplerParameterf( a_Obj, GL_TEXTURE_LOD_BIAS, MipLodBias );
		OpenGL3::SamplerParameterf( a_Obj, GL_TEXTURE_MIN_LOD, MinLod );
		OpenGL3::SamplerParameterf( a_Obj, GL_TEXTURE_MAX_LOD, MaxLod );

		if ( IsAnisotropic )
		{
			OpenGL3::SamplerParameteri( a_Obj, GL_TEXTURE_MAX_ANISOTROPY, MaxAnisotropy );
		}

		if ( AddressU == GL_CLAMP_TO_BORDER || AddressV == GL_CLAMP_TO_BORDER || AddressW == GL_CLAMP_TO_BORDER )
		{
			OpenGL3::SamplerParameterfv( a_Obj, GL_TEXTURE_BORDER_COLOR, &BorderColor[0] );
		}

		if ( ComparisonFunc != GL_NONE )
		{
			OpenGL3::SamplerParameteri( a_Obj, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
			OpenGL3::SamplerParameteri( a_Obj, GL_TEXTURE_COMPARE_FUNC, ComparisonFunc );
		}
		else
		{
			OpenGL3::SamplerParameteri( a_Obj, GL_TEXTURE_COMPARE_MODE, GL_NONE );
		}
	}

}
