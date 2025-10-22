#include "tripch.h"

#if USE_ASSET_IMPORTERS

#include "EnvironmentMapImporter.h"
#include <Tridium/Asset/Importers/TextureImporter.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>
#include <Tridium/Graphics/Renderer/PipelineStateCache.h>

namespace Tridium {

	static const Matrix4 s_ProjMatrix = Math::Perspective( Math::Radians( 90.0f ), 1.0f, 0.1f, 10.0f );
	static const Matrix4 s_ViewMatrices[6] = {
			// +X
			Math::LookAt( Vector3::Zero(), Vector3( 1.0f,  0.0f,  0.0f ), Vector3( 0.0f, -1.0f,  0.0f ) ),
			// -X
			Math::LookAt( Vector3::Zero(), Vector3( -1.0f, 0.0f,  0.0f ), Vector3( 0.0f, -1.0f,  0.0f ) ),
			// +Y
			Math::LookAt( Vector3::Zero(), Vector3( 0.0f, -1.0f,  0.0f ), Vector3( 0.0f,  0.0f, -1.0f ) ),
			// -Y
			Math::LookAt( Vector3::Zero(), Vector3( 0.0f,  1.0f,  0.0f ), Vector3( 0.0f,  0.0f,  1.0f ) ),
			// +Z
			Math::LookAt( Vector3::Zero(), Vector3( 0.0f,  0.0f,  1.0f ), Vector3( 0.0f, -1.0f,  0.0f ) ),
			// -Z
			Math::LookAt( Vector3::Zero(), Vector3( 0.0f,  0.0f, -1.0f ), Vector3( 0.0f, -1.0f,  0.0f ) ),
	};


	struct IrradianceVertex
	{
		Vector3 Position;
	};

	struct InlinedConstants_Irradiance
	{
		float4x4 View;
		float4x4 Projection;
	};

	// Irradiance Convolution Shader
	static constexpr StringView IrradianceVS = R"(
    #include "Core.hlsli"

	struct InlinedConstants_Irradiance
	{
		float4x4 View;
		float4x4 Projection;
	};

	struct VS_INPUT
	{
		float3 Position : POSITION;
	};

	struct PS_INPUT
	{
		float4 Position : SV_POSITION;
		float3 WorldPosition : WORLD_POSITION;
	};

	INLINED_CONSTANTS( Constants, InlinedConstants_Irradiance );

	PS_INPUT VSMain(float3 a_Position : POSITION)
	{
		PS_INPUT output;
		output.Position = mul(Constants.Projection, mul(Constants.View, float4(a_Position, 1.0f)));
		output.WorldPosition = a_Position;
		return output;
	}
	)";

	static constexpr StringView IrradiancePS = R"(
    #include "Core.hlsli"

	struct PS_INPUT
	{
		float4 Position : SV_POSITION;
		float3 WorldPosition : WORLD_POSITION;
	};

	COMBINED_SAMPLER( environmentMap, TextureCube, 0 );

	float4 PSMain(PS_INPUT input) : SV_TARGET0
	{
		// The world position is the direction from the origin to sample the environment map
		float3 normal = normalize( input.WorldPosition * float3( 1, -1, 1 ) );
	
		// tangent space calculation from origin point
		float3 up    = float3(0.0, 1.0, 0.0);
		float3 right = normalize(cross(up, normal));
		up           = normalize(cross(normal, right));
	
		float sampleDelta = 0.025;
		float nrSamples = 0.0;
		float3 irradiance = float3(0.0, 0.0, 0.0);
		for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
		{
		    for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		    {
		        // spherical to cartesian (in tangent space)
		        float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
		        // tangent space to world
		        float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

		        irradiance += clamp( SampleTexture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta), 0.0, 1.0);
		        nrSamples++;
		    }
		}
		irradiance = PI * irradiance * (1.0 / float(nrSamples));
	
		return float4(irradiance, 1.0);
	}
	)";

	static const auto IrradianceConvolutionFamily = ShaderFamily{}
		.SetName( "IrradianceConvolution" )
		.SetShaderSource( ERHIShaderType::Vertex, IrradianceVS )
		.SetShaderSource( ERHIShaderType::Pixel, IrradiancePS );

	REGISTER_SHADER_FAMILY( IrradianceConvolutionFamily );

	struct InlinedConstants_Radiance
	{
		float4x4 View;
		float4x4 Projection;
		float Roughness;
		uint Resolution;
	};

	// Radiance Convolution Shader
	static constexpr StringView RadianceVS = R"(
    #include "Core.hlsli"

	struct InlinedConstants_Radiance
	{
		float4x4 View;
		float4x4 Projection;
		float Roughness;
		uint Resolution;
	};

	struct VS_INPUT
	{
		float3 Position : POSITION;
	};

	struct PS_INPUT
	{
		float4 Position : SV_POSITION;
		float3 WorldPosition : WORLD_POSITION;
	};

	INLINED_CONSTANTS( Constants, InlinedConstants_Radiance );

	PS_INPUT VSMain(float3 a_Position : POSITION)
	{
		PS_INPUT output;
		output.Position = mul(Constants.Projection, mul(Constants.View, float4(a_Position, 1.0f)));
		output.WorldPosition = a_Position;
		return output;
	}
	)";

	static constexpr StringView RadiancePS = R"(
    #include "Core.hlsli"

	struct InlinedConstants_Radiance
	{
		float4x4 View;
		float4x4 Projection;
		float Roughness;
		uint Resolution;
	};

	struct PS_INPUT
	{
		float4 Position : SV_POSITION;
		float3 WorldPosition : WORLD_POSITION;
	};

	COMBINED_SAMPLER( EnvMap, TextureCube, 0 );
	INLINED_CONSTANTS( Constants, InlinedConstants_Radiance );

	float RadicalInverse_VdC(uint bits) 
	{
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return float(bits) * 2.3283064365386963e-10; // / 0x100000000
	}

	float2 Hammersley(uint i, uint N)
	{
		return float2(float(i)/float(N), RadicalInverse_VdC(i));
	}

	float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
	{
		// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
		float a = roughness * roughness;

		// from spherical coordinates to cartesian coordinates (in tangent space)
		float phi = 2.0 * PI * Xi.x;
		float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
		float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

		// from spherical coordinates to cartesian coordinates
		float3 H;
		H.x = cos(phi) * sinTheta;
		H.y = sin(phi) * sinTheta;
		H.z = cosTheta;

		// from tangent-space H vector to world-space sample vector
		float3 up        = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
		float3 tangent   = normalize(cross(up, N));
		float3 bitangent = cross(N, tangent);

		float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
		return normalize(sampleVec);
	}

	float DistributionGGX(float3 N, float3 H, float roughness)
	{
		float a      = roughness*roughness;
		float a2     = a*a;
		float NdotH  = max(dot(N, H), 0.0);
		float NdotH2 = NdotH*NdotH;

		float denom = (NdotH2 * (a2 - 1.0) + 1.0);
		denom = PI * denom * denom;

		return a2 / denom;
	}

	float CalculateMipLevel(float3 N, float3 V, float3 H, float roughness)
	{
		if (roughness == 0.0)
			return 0.0;

		float D = DistributionGGX(N, H, roughness);
		float NdotH = max(dot(N, H), 0.0);
		float HdotV = max(dot(H, V), 0.0);
		float pdf = (D * NdotH) / (4.0 * HdotV) + 0.0001;

		float resolution = float(Constants.Resolution);
		float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
		float saSample = 1.0 / (float(1024u * 16u) * pdf + 0.0001);
		float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

		return mipLevel;
	}

	float4 PSMain(PS_INPUT input) : SV_TARGET0
	{
		float3 N = normalize( input.WorldPosition * float3( 1, -1, 1 ) );
		float3 R = N;
		float3 V = R;

		const uint SAMPLE_COUNT = 1024u * 8u; // Increase for better quality
		float3 prefilteredColor = float3(0.0, 0.0, 0.0);
		float totalWeight = 0.0;

		for(uint i = 0u; i < SAMPLE_COUNT; ++i)
		{
			float2 Xi = Hammersley(i, SAMPLE_COUNT);
			float3 H  = ImportanceSampleGGX(Xi, N, Constants.Roughness);
			float3 L  = normalize(2.0 * dot(V, H) * H - V);
			float NdotL = max(dot(N, L), 0.0);

			if(NdotL > 0.0)
			{
				const float mipLevel = CalculateMipLevel( N, V, H, Constants.Roughness );
				float3 radiance = clamp( SampleTextureLod( EnvMap, L, mipLevel ).rgb, float3(0,0,0), float3(100,100,100) * (1.1 - Constants.Roughness) );
				prefilteredColor += radiance * NdotL;
				totalWeight      += NdotL;
			}
		}

		prefilteredColor = prefilteredColor / totalWeight;
		return float4(prefilteredColor, 1.0);
	}
	)";

	static const auto RadianceConvolutionFamily = ShaderFamily{}
		.SetName( "RadianceConvolution" )
		.SetShaderSource( ERHIShaderType::Vertex, RadianceVS )
		.SetShaderSource( ERHIShaderType::Pixel, RadiancePS );

	REGISTER_SHADER_FAMILY( RadianceConvolutionFamily );


	REGISTER_ASSET_IMPORTER( EnvironmentMapImporter );

	using CubeMap = EnvironmentMap::CubeMap;

	template<typename T>
	CubeMap CalculateCubemap( const T* a_ImageData, uint32_t a_Width, uint32_t a_Height, uint32_t a_Channels, uint32_t a_CubemapResolution, bool a_FilterLinear );

	TODO( "These functions should probably be moved to a shader interop file" );

	static RHITextureRef ComputeIrradianceConvolution( const RHICommandListRef& a_CmdList, ERHIFormat a_Format, const RHITextureRef& a_EnvMap, uint32_t a_OutSize )
	{
		const RHIFormatInfo formatInfo = GetRHIFormatInfo( a_Format );
		const auto& shaderFamily = ShaderLibrary::GetOrCreateVariant( "IrradianceConvolution"_H );
		ENSURE( shaderFamily );

		RHIGraphicsPipelineStateRef graphicsPSO;
		{
			auto psoDesc = RHIGraphicsPipelineStateDesc{}
				.SetName( "IrradianceConvolution" )
				.SetTopology( ERHITopology::Triangle )
				.SetVertexLayout( RHIVertexLayout::From<IrradianceVertex>() )
				.SetDepthState( RHIDepthState{}
					.SetDepthTestEnabled( false )
					.SetDepthWriteEnabled( false )
					.SetComparison( ERHIComparison::Less ) )
				.SetRasterizerState( RHIRasterizerState{}
					.SetCullMode( ERHICullMode::None )
					.SetFillMode( ERHIFillMode::Solid ) )
				.SetFramebufferInfo( RHIFramebufferInfo{}
					.SetColorFormats( { a_Format } )
				);

			shaderFamily->Apply( psoDesc );

			graphicsPSO = PipelineStateCache::GetOrCreatePSO( psoDesc );
		}

		RHITextureRef irradianceTex;
		{
			const auto irradianceDesc = RHITextureDesc{}
				.SetName( "IrradianceConvolution_IrradianceTex" )
				.SetDimension( ERHITextureDimension::TextureCube )
				.SetWidth( a_OutSize )
				.SetHeight( a_OutSize )
				.SetDepth( 6 )
				.SetMips( 1 )
				.SetFormat( a_Format )
				.SetUseClearValue( true )
				.SetClearValue( RHIClearValue{} )
				.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource )
				.SetHeapType( ERHIHeapType::Default );
			irradianceTex = RHI::CreateTexture( irradianceDesc );
		}

		RHITextureRef stagingTex;
		{
			const auto stagingDesc = RHITextureDesc{}
				.SetDimension( ERHITextureDimension::TextureCube )
				.SetWidth( a_OutSize )
				.SetHeight( a_OutSize )
				.SetDepth( 6 )
				.SetMips( 1 )
				.SetFormat( irradianceTex->Desc().Format )
				.SetHeapType( ERHIHeapType::Staging );
			stagingTex = RHI::CreateTexture( stagingDesc );
		}

		RHIBufferRef vertexBuffer;
		{
			// Cubemap capture uses a unit cube
			constexpr IrradianceVertex vertices[] = {
				{ { -1.0f, -1.0f,  1.0f } }, // 0
				{ {  1.0f, -1.0f,  1.0f } }, // 1
				{ {  1.0f,  1.0f,  1.0f } }, // 2
				{ { -1.0f,  1.0f,  1.0f } }, // 3
				{ { -1.0f, -1.0f, -1.0f } }, // 4
				{ {  1.0f, -1.0f, -1.0f } }, // 5
				{ {  1.0f,  1.0f, -1.0f } }, // 6
				{ { -1.0f,  1.0f, -1.0f } }, // 7
			};

			vertexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
				.SetName( "IrradianceConvolution_VertexBuffer" )
				.SetSize( sizeof( vertices ) )
				.SetHeapType( ERHIHeapType::Immutable )
				.SetBindFlags( ERHIBindFlags::VertexBuffer ),
				Span<const byte_t>( (const byte_t*)vertices, sizeof( vertices ) )
			);
		}

		RHIBufferRef indexBuffer;
		{
			constexpr uint16_t indices[] = {
				0, 1, 2, 2, 3, 0, // Front face
				1, 5, 6, 6, 2, 1, // Right face
				5, 4, 7, 7, 6, 5, // Back face
				4, 0, 3, 3, 7, 4, // Left face
				3, 2, 6, 6, 7, 3, // Top face
				4, 5, 1, 1, 0, 4  // Bottom face
			};

			indexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
				.SetName( "IrradianceConvolution_IndexBuffer" )
				.SetSize( sizeof( indices ) )
				.SetHeapType( ERHIHeapType::Immutable )
				.SetBindFlags( ERHIBindFlags::IndexBuffer )
				.SetType( ERHIBufferType::Formatted )
				.SetFormat( ERHIFormat::R16_UINT ),
				Span<const byte_t>( (const byte_t*)indices, sizeof( indices ) )
			);
		}

		RHIBindingSetRef bindingSet;
		{
			const auto bindingSetDesc = RHIBindingSetDesc{ shaderFamily->BindingLayout }
				.SetName( "IrradianceConvolution_BindingSet" )
				.AddTexture( "environmentMap"_H, a_EnvMap.get() );
			bindingSet = RHI::CreateBindingSet( bindingSetDesc );
		}

		RHIGraphicsState graphicsState;
		graphicsState.SetPipelineState( graphicsPSO.get() );
		graphicsState.SetVertexBuffer( vertexBuffer.get() );
		graphicsState.SetIndexBuffer( indexBuffer.get() );
		graphicsState.AddBindingSet( bindingSet.get() );

		// For each face of the cubemap
		// We render a quad with the appropriate view matrix
		for ( uint32_t face = 0; face < 6; face++ )
		{
			InlinedConstants_Irradiance inlinedConstants;
			inlinedConstants.View = s_ViewMatrices[face];
			inlinedConstants.Projection = s_ProjMatrix;

			graphicsState.SetFramebuffer( RHIFramebuffer{}
				.AddColorAttachment( irradianceTex, false, RHITextureSlice{}
					.SetMipLevel( 0 )
					.SetArraySlice( face )
					.SetSize( a_OutSize, a_OutSize ) )
			);

			a_CmdList->SetGraphicsState( graphicsState );
			a_CmdList->SetViewportState( RHIViewportState{}
				.AddViewportAndScissor( RHIViewport{ 0.0f, 0.0f, (float)a_OutSize, (float)a_OutSize, 0.0f, 1.0f } )
			);

			a_CmdList->ClearRenderTargets( ERHIClearFlags::All, RHIClearValue{} );

			a_CmdList->SetInlinedConstants( inlinedConstants, 0 );

			a_CmdList->Draw( RHIDrawArgs{}
						   .SetVertexCount( vertexBuffer->Desc().Size / sizeof( IrradianceVertex ) )
						   .SetIndexCount( indexBuffer->Desc().Size / sizeof( uint16_t ) )
			);
		}

		// Copy the results to the staging texture for readback
		for ( uint32_t face = 0; face < 6; face++ )
		{
			const auto slice = RHITextureSlice{}
				.SetMipLevel( 0 )
				.SetArraySlice( face )
				.SetSize( a_OutSize, a_OutSize );

			a_CmdList->CopyTexture( *stagingTex, slice, *irradianceTex, slice );
		}

		return stagingTex;
	}

	static CubeMap ExtractIrradianceCubeMap( const RHITextureRef& a_StagingTex )
	{
		const RHIFormatInfo formatInfo = GetRHIFormatInfo( a_StagingTex->Desc().Format );
		const uint32_t outSize = a_StagingTex->Desc().Width;

		// Read back the staging texture
		CubeMap result;
		result.FaceWidth = outSize;
		result.RowStride = outSize * formatInfo.Bytes();
		result.FaceStride = result.RowStride * outSize;
		result.PixelData.Resize( 6u * result.FaceStride );
		for ( uint32_t face = 0; face < 6; face++ )
		{
			const auto slice = RHITextureSlice{}
				.SetMipLevel( 0 )
				.SetArraySlice( face )
				.SetSize( outSize, outSize );

			RHITextureSubresourceData stagingMapped = a_StagingTex->MapSubresource( slice );
			ENSURE( stagingMapped.Valid() );

			byte_t* destFace = result.PixelData.Data() + face * result.FaceStride;
			for ( uint32_t y = 0; y < outSize; y++ )
			{
				std::memcpy( destFace + y * result.RowStride,
							 (const byte_t*)stagingMapped.Data + y * stagingMapped.RowStride,
							 result.RowStride );
			}

			a_StagingTex->UnmapSubresource( slice );
		}

		return result;
	}

	static RHITextureRef PrefilterSpecular( const RHICommandListRef& a_CmdList, ERHIFormat a_Format, const RHITextureRef& a_EnvMap, uint32_t a_OutSize, uint32_t a_MipLevels, float a_RoughnessBase )
	{
		const RHIFormatInfo formatInfo = GetRHIFormatInfo( a_Format );
		const auto& shaderFamily = ShaderLibrary::GetOrCreateVariant( "RadianceConvolution"_H );
		ENSURE( shaderFamily );
		RHIGraphicsPipelineStateRef graphicsPSO;
		{
			auto psoDesc = RHIGraphicsPipelineStateDesc{}
				.SetName( "RadianceConvolution" )
				.SetTopology( ERHITopology::Triangle )
				.SetVertexLayout( RHIVertexLayout::From<IrradianceVertex>() )
				.SetDepthState( RHIDepthState{}
					.SetDepthTestEnabled( false )
					.SetDepthWriteEnabled( false )
					.SetComparison( ERHIComparison::Less ) )
				.SetRasterizerState( RHIRasterizerState{}
					.SetCullMode( ERHICullMode::None )
					.SetFillMode( ERHIFillMode::Solid ) )
				.SetFramebufferInfo( RHIFramebufferInfo{}
					.SetColorFormats( { a_Format } )
				);
			shaderFamily->Apply( psoDesc );
			graphicsPSO = PipelineStateCache::GetOrCreatePSO( psoDesc );
		}

		RHITextureRef radianceTex;
		{
			const auto radianceDesc = RHITextureDesc{}
				.SetName( "RadianceConvolution_RadianceTex" )
				.SetDimension( ERHITextureDimension::TextureCube )
				.SetWidth( a_OutSize )
				.SetHeight( a_OutSize )
				.SetDepth( 6 )
				.SetMips( a_MipLevels )
				.SetFormat( a_Format )
				.SetUseClearValue( true )
				.SetClearValue( RHIClearValue{} )
				.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource )
				.SetHeapType( ERHIHeapType::Default );
			radianceTex = RHI::CreateTexture( radianceDesc );
		}

		RHITextureRef stagingTex;
		{
			const auto stagingDesc = RHITextureDesc{}
				.SetName( "RadianceConvolution_StagingTex" )
				.SetDimension( ERHITextureDimension::TextureCube )
				.SetWidth( a_OutSize )
				.SetHeight( a_OutSize )
				.SetDepth( 6 )
				.SetMips( a_MipLevels )
				.SetFormat( radianceTex->Desc().Format )
				.SetHeapType( ERHIHeapType::Staging );
			stagingTex = RHI::CreateTexture( stagingDesc );
		}

		RHIBufferRef vertexBuffer;
		{
			// Cubemap capture uses a unit cube
			constexpr IrradianceVertex vertices[] = {
				{ { -1.0f, -1.0f,  1.0f } }, // 0
				{ {  1.0f, -1.0f,  1.0f } }, // 1
				{ {  1.0f,  1.0f,  1.0f } }, // 2
				{ { -1.0f,  1.0f,  1.0f } }, // 3
				{ { -1.0f, -1.0f, -1.0f } }, // 4
				{ {  1.0f, -1.0f, -1.0f } }, // 5
				{ {  1.0f,  1.0f, -1.0f } }, // 6
				{ { -1.0f,  1.0f, -1.0f } }, // 7
			};

			vertexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
				.SetName( "IrradianceConvolution_VertexBuffer" )
				.SetSize( sizeof( vertices ) )
				.SetHeapType( ERHIHeapType::Immutable )
				.SetBindFlags( ERHIBindFlags::VertexBuffer ),
				Span<const byte_t>( (const byte_t*)vertices, sizeof( vertices ) )
			);
		}

		RHIBufferRef indexBuffer;
		{
			constexpr uint16_t indices[] = {
				0, 1, 2, 2, 3, 0, // Front face
				1, 5, 6, 6, 2, 1, // Right face
				5, 4, 7, 7, 6, 5, // Back face
				4, 0, 3, 3, 7, 4, // Left face
				3, 2, 6, 6, 7, 3, // Top face
				4, 5, 1, 1, 0, 4  // Bottom face
			};

			indexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
				.SetName( "IrradianceConvolution_IndexBuffer" )
				.SetSize( sizeof( indices ) )
				.SetHeapType( ERHIHeapType::Immutable )
				.SetBindFlags( ERHIBindFlags::IndexBuffer )
				.SetType( ERHIBufferType::Formatted )
				.SetFormat( ERHIFormat::R16_UINT ),
				Span<const byte_t>( (const byte_t*)indices, sizeof( indices ) )
			);
		}

		RHIBindingSetRef bindingSet;
		{
			const auto bindingSetDesc = RHIBindingSetDesc{ shaderFamily->BindingLayout }
				.SetName( "RadianceConvolution_BindingSet" )
				.AddTexture( "EnvMap"_H, a_EnvMap.get() );
			bindingSet = RHI::CreateBindingSet( bindingSetDesc );
		}

		RHIGraphicsState graphicsState;
		graphicsState.SetPipelineState( graphicsPSO.get() );
		graphicsState.SetVertexBuffer( vertexBuffer.get() );
		graphicsState.SetIndexBuffer( indexBuffer.get() );
		graphicsState.AddBindingSet( bindingSet.get() );

		// For each face of the cubemap
		for ( uint32_t mip = 0; mip < a_MipLevels; mip++ )
		{
			const uint32_t mipSize = a_OutSize >> mip;
			const float roughness = (float)mip / (float)( a_MipLevels - 1 );
			// We render a quad with the appropriate view matrix
			for ( uint32_t face = 0; face < 6; face++ )
			{
				InlinedConstants_Radiance inlinedConstants;
				inlinedConstants.View = s_ViewMatrices[face];
				inlinedConstants.Projection = s_ProjMatrix;
				inlinedConstants.Roughness = roughness;
				inlinedConstants.Resolution = a_OutSize;

				graphicsState.SetFramebuffer( RHIFramebuffer{}
					.AddColorAttachment( radianceTex, false, RHITextureSlice{}
						.SetMipLevel( mip )
						.SetArraySlice( face )
						.SetSize( mipSize, mipSize ) )
				);
				a_CmdList->SetGraphicsState( graphicsState );
				a_CmdList->SetViewportState( RHIViewportState{}
					.AddViewportAndScissor( RHIViewport{ 0.0f, 0.0f, (float)mipSize, (float)mipSize, 0.0f, 1.0f } )
				);
				a_CmdList->ClearRenderTargets( ERHIClearFlags::All, RHIClearValue{} );
				a_CmdList->SetInlinedConstants( inlinedConstants, 0 );
				a_CmdList->Draw( RHIDrawArgs{}
							   .SetVertexCount( vertexBuffer->Desc().Size / sizeof( IrradianceVertex ) )
							   .SetIndexCount( indexBuffer->Desc().Size / sizeof( uint16_t ) )
				);
			}
		}

		// Copy the results to the staging texture for readback
		for ( uint32_t mip = 0; mip < a_MipLevels; mip++ )
		{
			const uint32_t mipSize = a_OutSize >> mip;
			for ( uint32_t face = 0; face < 6; face++ )
			{
				const auto slice = RHITextureSlice{}
					.SetMipLevel( mip )
					.SetArraySlice( face )
					.SetSize( mipSize, mipSize );
				a_CmdList->CopyTexture( *stagingTex, slice, *radianceTex, slice );
			}
		}

		return stagingTex;
	}

	static Array<CubeMap> ExtractRadianceCubeMapMips( const RHITextureRef& a_StagingTex )
	{
		const RHIFormatInfo formatInfo = GetRHIFormatInfo( a_StagingTex->Desc().Format );
		const uint32_t baseSize = a_StagingTex->Desc().Width;
		const uint32_t mipLevels = a_StagingTex->Desc().Mips;

		Array<CubeMap> result;
		result.Resize( mipLevels );

		// Read back the staging texture
		for ( uint32_t mip = 0; mip < mipLevels; mip++ )
		{
			const uint32_t mipSize = baseSize >> mip;
			CubeMap& cubeMap = result[mip];
			cubeMap.FaceWidth = mipSize;
			cubeMap.RowStride = mipSize * formatInfo.Bytes();
			cubeMap.FaceStride = cubeMap.RowStride * mipSize;
			cubeMap.PixelData.Resize( 6u * cubeMap.FaceStride );
			for ( uint32_t face = 0; face < 6; face++ )
			{
				const auto slice = RHITextureSlice{}
					.SetMipLevel( mip )
					.SetArraySlice( face )
					.SetSize( mipSize, mipSize );

				RHITextureSubresourceData stagingMapped = a_StagingTex->MapSubresource( slice );
				ENSURE( stagingMapped.Valid() );

				byte_t* destFace = cubeMap.PixelData.Data() + face * cubeMap.FaceStride;
				for ( uint32_t y = 0; y < mipSize; y++ )
				{
					std::memcpy( destFace + y * cubeMap.RowStride,
								 (const byte_t*)stagingMapped.Data + y * stagingMapped.RowStride,
								 cubeMap.RowStride );
				}

				a_StagingTex->UnmapSubresource( slice );
			}
		}
		return result;
	}

	bool EnvironmentMapImporter::OnImport( AssetImportContext& a_Context )
	{
		if ( !a_Context.FileData().empty() )
		{
			TODO( "We currently don't support importing from memory for environment maps" );
			NOT_IMPLEMENTED;
			return false;
		}

		Array<byte_t> texData;
		uint32_t textureWidth = 0;
		uint32_t textureHeight = 0;
		ERHIFormat textureFormat = ERHIFormat::Unknown;
		bool isFloat = true;
		bool isEmbedded = false;

		const String filePath = a_Context.AssetPath().ToString();

		auto output = TextureImporter{}.LoadFromFile(
			filePath.c_str(),
			texData,
			textureWidth,
			textureHeight,
			textureFormat,
			isFloat,
			false,
			4
		);

		if ( output.IsError() )
		{
			// Failed to load texture, log a warning and return null
			a_Context.FailImport( std::format( "Failed to load texture '{}': {}", filePath, output.Error() ) );
			return false;
		}

		const RHIFormatInfo formatInfo = GetRHIFormatInfo( textureFormat );
		const size_t outputFaceSize = 1024; TODO( "Make this configurable?" );
		CubeMap envMap = isFloat 
			? CalculateCubemap<float>( (const float*)texData.Data(), textureWidth, textureHeight, formatInfo.NumChannels(), outputFaceSize, false)
			: CalculateCubemap<byte_t>(              texData.Data(), textureWidth, textureHeight, formatInfo.NumChannels(), outputFaceSize, false);

		// Create the environment map asset
		const FilePath assetFileName = FilePath::UniquePath( std::format( "{}.{}",
																		  a_Context.AssetPath().GetFilenameWithoutExtension(),
																		  AssetExtensionName ) );

		const FilePath assetPath = a_Context.DestinationPath() / assetFileName;

		AssetRef<EnvironmentMap> envMapRef = a_Context.CreateAsset<EnvironmentMap>( a_Context.AssetPath().GetFilenameWithoutExtension(), assetPath.ToString() );
		if ( !envMapRef )
		{
			a_Context.FailImport( "Failed to create EnvironmentMap asset." );
			return false;
		}

		// Upload the environment map to the GPU and compute the irradiance and radiance maps on the GPU
		CubeMap irradianceMap;
		Array<CubeMap> radianceMaps;
		{
			RHITextureRef envRHITex;
			{
				const auto envDesc = RHITextureDesc{}
					.SetName( "IrradianceConvolution_EnvironmentTex" )
					.SetDimension( ERHITextureDimension::TextureCube )
					.SetWidth( envMap.FaceWidth )
					.SetHeight( envMap.FaceWidth )
					.SetDepth( 6 )
					.SetMips( 1 )
					.SetFormat( textureFormat )
					.SetBindFlags( ERHIBindFlags::ShaderResource )
					.SetInitialState( ERHIResourceStates::ShaderResource )
					.SetHeapType( ERHIHeapType::Immutable );

				Array<RHITextureSubresourceData> subresources;
				subresources.Resize( 6 );
				for ( uint32_t face = 0; face < 6; face++ )
				{
					subresources[face] = RHITextureSubresourceData{}
						.SetData( envMap.PixelData.Data() + face * envMap.FaceStride )
						.SetRowStride( envMap.FaceWidth * formatInfo.Bytes() );
				}

				envRHITex = RHI::CreateTexture( envDesc, subresources );
			}

			RHICommandListRef cmdList = RHI::CreateCommandList( RHICommandListDesc{}
				.SetName( "EnvironmentMapImporter" )
				.SetQueueType( ERHICommandQueueType::Graphics )
				.SetEnableImmediateExecution( true )
			);

			cmdList->Open();
			RHITextureRef irradianceTex = ComputeIrradianceConvolution( cmdList, textureFormat, envRHITex, envMap.FaceWidth / 4 );
			RHITextureRef radianceTex = PrefilterSpecular( cmdList, textureFormat, envRHITex, envMap.FaceWidth, 10, 1.0f );
			cmdList->Close();

			auto* cmdListPtr = cmdList.get();
			const RHIFenceValue fence = RHI::ExecuteCommandLists( &cmdListPtr, 1, ERHICommandQueueType::Graphics );
			RHI::WaitForFence( ERHICommandQueueType::Graphics, fence );

			irradianceMap = ExtractIrradianceCubeMap( irradianceTex );
			radianceMaps = ExtractRadianceCubeMapMips( radianceTex );
		}

		struct EnvironmentMapAccessor : EnvironmentMap { friend EnvironmentMapImporter; };
		auto envMapAsset = Cast<EnvironmentMapAccessor*>( envMapRef.get() );
		envMapAsset->m_Format = textureFormat;
		envMapAsset->m_Irradiance = std::move( irradianceMap );
		envMapAsset->m_RadianceMips = std::move( radianceMaps );

		if ( envMapAsset->NumRadianceMips() == 0 )
		{
			a_Context.FailImport( "Failed to prefilter radiance environment map." );
			return false;
		}

		return true;
	}

	template<typename T>
	CubeMap CalculateCubemap( const T* a_ImageData, uint32_t a_Width, uint32_t a_Height, uint32_t a_Channels, uint32_t a_CubemapResolution, bool a_FilterLinear )
	{
		CubeMap result;
		result.FaceWidth = a_CubemapResolution;
		result.FaceStride = a_CubemapResolution * a_CubemapResolution * a_Channels * sizeof( T );
		result.PixelData.Resize( 6u * result.FaceStride );

		T* faces[6];
		for ( int i = 0; i < 6; i++ )
		{
			faces[i] = ReinterpretCast<T*>( result.PixelData.Data() + i * result.FaceStride );
		}

		struct Vec3 { float x, y, z; };
		std::array<std::array<Vec3, 3>, 6> startRightUp = { { // for each face, contains the 3d starting point (corresponding to left bottom pixel), right direction, and up direction in 3d space, correponding to pixel x,y coordinates of each face		{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}},   // right
			{{{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}},  // left
			{{{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}},  // down
			{{{-1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}},   // up
			{{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}},  // front
			{{{1.0f, -1.0f, 1.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}}   // back 
		} };

		for ( int i = 0; i < 6; i++ )
		{
			Vec3& start = startRightUp[i][0];
			Vec3& right = startRightUp[i][1];
			Vec3& up = startRightUp[i][2];

			T* face = faces[i];
			Vec3 pixelDirection3d; // 3d direction corresponding to a pixel in the cubemap face
			//#pragma omp parallel for (private pixelDirection?)
			for ( int row = 0; row < a_CubemapResolution; row++ )
			{
				for ( int col = 0; col < a_CubemapResolution; col++ )
				{
					pixelDirection3d.x = start.x + ( (float)col * 2.0f + 0.5f ) / (float)a_CubemapResolution * right.x + ( (float)row * 2.0f + 0.5f ) / (float)a_CubemapResolution * up.x;
					pixelDirection3d.y = start.y + ( (float)col * 2.0f + 0.5f ) / (float)a_CubemapResolution * right.y + ( (float)row * 2.0f + 0.5f ) / (float)a_CubemapResolution * up.y;
					pixelDirection3d.z = start.z + ( (float)col * 2.0f + 0.5f ) / (float)a_CubemapResolution * right.z + ( (float)row * 2.0f + 0.5f ) / (float)a_CubemapResolution * up.z;

					float azimuth = atan2f( pixelDirection3d.x, -pixelDirection3d.z ) + Math::PI<float>(); // add pi to move range to 0-360 deg
					float elevation = atanf( pixelDirection3d.y / sqrtf( pixelDirection3d.x * pixelDirection3d.x + pixelDirection3d.z * pixelDirection3d.z ) ) + Math::PI<float>() / 2.0f;

					float colHdri = ( azimuth / Math::PI<float>() / 2.0f ) * a_Width; // add pi to azimuth to move range to 0-360 deg
					float rowHdri = ( elevation / Math::PI<float>() ) * a_Height;

					if ( !a_FilterLinear )
					{
						uint32_t colNearest = std::clamp( (uint32_t)colHdri, 0u, a_Width - 1u );
						uint32_t rowNearest = std::clamp( (uint32_t)rowHdri, 0u, a_Height - 1u );

						face[col * a_Channels + a_CubemapResolution * row * a_Channels] = a_ImageData[colNearest * a_Channels + a_Width * rowNearest * a_Channels]; // red
						face[col * a_Channels + a_CubemapResolution * row * a_Channels + 1] = a_ImageData[colNearest * a_Channels + a_Width * rowNearest * a_Channels + 1]; //green
						face[col * a_Channels + a_CubemapResolution * row * a_Channels + 2] = a_ImageData[colNearest * a_Channels + a_Width * rowNearest * a_Channels + 2]; //blue
						if ( a_Channels > 3 )
							face[col * a_Channels + a_CubemapResolution * row * a_Channels + 3] = a_ImageData[colNearest * a_Channels + a_Width * rowNearest * a_Channels + 3]; //alpha
					}
					else // perform bilinear interpolation
					{
						float intCol, intRow;
						float factorCol = modf( colHdri - 0.5f, &intCol );        // factor gives the contribution of the next column, while the contribution of intCol is 1 - factor
						float factorRow = modf( rowHdri - 0.5f, &intRow );

						int low_idx_row = static_cast<int>( intRow );
						int low_idx_column = static_cast<int>( intCol );
						int high_idx_column;
						if ( factorCol < 0.0f )                           //modf can only give a negative value if the azimuth falls in the first pixel, left of the center, so we have to mix with the pixel on the opposite side of the panoramic image
							high_idx_column = a_Width - 1;
						else if ( low_idx_column == a_Width - 1 )          //if we are in the right-most pixel, and fall right of the center, mix with the left-most pixel
							high_idx_column = 0;
						else
							high_idx_column = low_idx_column + 1;

						int high_idx_row;
						if ( factorRow < 0.0f )
							high_idx_row = a_Height - 1;
						else if ( low_idx_row == a_Height - 1 )
							high_idx_row = 0;
						else
							high_idx_row = low_idx_row + 1;

						factorCol = abs( factorCol );
						factorRow = abs( factorRow );
						float f1 = ( 1 - factorRow ) * ( 1 - factorCol );
						float f2 = factorRow * ( 1 - factorCol );
						float f3 = ( 1 - factorRow ) * factorCol;
						float f4 = factorRow * factorCol;

						for ( int j = 0; j < a_Channels; j++ )
						{
							unsigned char interpolatedValue = Cast<unsigned char>(
								a_ImageData[low_idx_column * a_Channels + a_Width * low_idx_row * a_Channels + j] * f1 +
								a_ImageData[low_idx_column * a_Channels + a_Width * high_idx_row * a_Channels + j] * f2 +
								a_ImageData[high_idx_column * a_Channels + a_Width * low_idx_row * a_Channels + j] * f3 +
								a_ImageData[high_idx_column * a_Channels + a_Width * high_idx_row * a_Channels + j] * f4
							);

							face[col * a_Channels + a_CubemapResolution * row * a_Channels + j] = std::clamp( interpolatedValue, (uint8_t)0, (uint8_t)255 );
						}
					}
				}
			}
		}

		return result;
	}

	Color4 SampleEnvironmentCubemap( ERHIFormat a_Format, const CubeMap& a_Env, const Vector3& a_Dir )
	{
		// Determine the major axis direction
		const Vector3 absDir = Vector3( fabsf( a_Dir.X ), fabsf( a_Dir.Y ), fabsf( a_Dir.Z ) );
		uint32_t faceIndex;
		Vector2 uv;

		if ( absDir.X >= absDir.Y && absDir.X >= absDir.Z )
		{
			// Major axis is X
			if ( a_Dir.X > 0 )
			{
				faceIndex = 0; // +X
				uv = Vector2( -a_Dir.Z, -a_Dir.Y ) / absDir.X;
			}
			else
			{
				faceIndex = 1; // -X
				uv = Vector2( a_Dir.Z, -a_Dir.Y ) / absDir.X;
			}
		}
		else if ( absDir.Y >= absDir.X && absDir.Y >= absDir.Z )
		{
			// Major axis is Y
			if ( a_Dir.Y > 0 )
			{
				faceIndex = 2; // +Y
				uv = Vector2( a_Dir.X, a_Dir.Z ) / absDir.Y;
			}
			else
			{
				faceIndex = 3; // -Y
				uv = Vector2( a_Dir.X, -a_Dir.Z ) / absDir.Y;
			}
		}
		else
		{
			// Major axis is Z
			if ( a_Dir.Z > 0 )
			{
				faceIndex = 4; // +Z
				uv = Vector2( a_Dir.X, -a_Dir.Y ) / absDir.Z;
			}
			else
			{
				faceIndex = 5; // -Z
				uv = Vector2( -a_Dir.X, -a_Dir.Y ) / absDir.Z;
			}
		}

		// Convert range from [-1,1] to [0,1]
		uv = ( uv + Vector2( 1.0f ) ) * 0.5f;
		const uint32_t x = Math::Clamp( Cast<uint32_t>( uv.X * a_Env.FaceWidth ), 0u, a_Env.FaceWidth - 1 );
		const uint32_t y = Math::Clamp( Cast<uint32_t>( uv.Y * a_Env.FaceWidth ), 0u, a_Env.FaceWidth - 1 );

		return {};//GetPixelFromCubemap( a_Env, faceIndex, x, y, a_Format );
	}

	Vector3 CubemapTexelDirection( uint32_t a_Face, float a_U, float a_V, uint32_t a_FaceSize )
	{
		// Convert from [0, FaceWidth-1] to [-1, 1]
		float u = ( 2.0f * ( a_U + 0.5f ) / a_FaceSize ) - 1.0f;
		float v = ( 2.0f * ( a_V + 0.5f ) / a_FaceSize ) - 1.0f;

		switch ( a_Face )
		{
			case 0: return Vector3( 1.0f, -v, -u );      // +X
			case 1: return Vector3( -1.0f, -v, u );      // -X
			case 2: return Vector3( u, 1.0f, v );        // +Y
			case 3: return Vector3( u, -1.0f, -v );      // -Y
			case 4: return Vector3( u, -v, 1.0f );       // +Z
			case 5: return Vector3( -u, -v, -1.0f );     // -Z
			default: return Vector3( 0.0f, 0.0f, 0.0f );
		}
	}

	Vector3 ImportanceSampleCosineHemisphere( const Vector2& a_Xi, const Vector3& a_N )
	{
		// Cosine-weighted hemisphere sampling
		float phi = 2.0f * Math::PI() * a_Xi.X;
		float cosTheta = Math::Sqrt( 1.0f - a_Xi.Y );
		float sinTheta = Math::Sqrt( a_Xi.Y );

		Vector3 H;
		H.X = Math::Cos( phi ) * sinTheta;
		H.Y = sinTheta * Math::Sin( phi );
		H.Z = cosTheta;

		// Transform from tangent space to world space
		Vector3 up = Math::Abs( a_N.Z ) < 0.999f ? Vector3( 0, 0, 1 ) : Vector3( 1, 0, 0 );
		Vector3 tangent = Math::Cross( up, a_N ).Normalized();
		Vector3 bitangent = Math::Cross( a_N, tangent );

		return ( tangent * H.X + bitangent * H.Y + a_N * H.Z ).Normalized();
	}

	Vector3 ImportanceSampleGGX( const Vector2& a_Xi, float a_Roughness, const Vector3& a_N )
	{
		const float a = a_Roughness * a_Roughness;
		const float phi = 2.0f * Math::PI() * a_Xi.X;
		const float cosTheta = Math::Sqrt( ( 1.0f - a_Xi.Y ) / ( 1.0f + ( a * a - 1.0f ) * a_Xi.Y ) );
		const float sinTheta = Math::Sqrt( 1.0f - cosTheta * cosTheta );

		Vector3 H;
		H.X = Math::Cos( phi ) * sinTheta;
		H.Y = sinTheta * Math::Sin( phi );
		H.Z = cosTheta;

		// Transform from tangent space to world space
		const Vector3 up = Math::Abs( a_N.Z ) < 0.999f ? Vector3( 0, 0, 1 ) : Vector3( 1, 0, 0 );
		const Vector3 tangent = Math::Cross( up, a_N ).Normalized();
		const Vector3 bitangent = Math::Cross( a_N, tangent );

		return ( tangent * H.X + bitangent * H.Y + a_N * H.Z ).Normalized();
	}

	Vector3 Hammersley( uint32_t a_i, uint32_t a_N )
	{
		float rdi = 0.0f;
		float f = 0.5f;
		uint32_t i = a_i;

		while ( i )
		{
			rdi += f * ( i & 1 );
			i >>= 1;
			f *= 0.5f;
		}

		return Vector3( (float)a_i / (float)a_N, rdi, 0.0f );
	}

	Color4 GetPixelFromCubemap( const CubeMap& a_CubeMap, uint32_t a_Face, uint32_t a_X, uint32_t a_Y, ERHIFormat a_Format )
	{
		if ( a_Face >= 6 || a_X >= a_CubeMap.FaceWidth || a_Y >= a_CubeMap.FaceWidth )
		{
			return Color4( 0.0f );
		}

		const RHIFormatInfo formatInfo = GetRHIFormatInfo( a_Format );
		const uint32_t pixelIndex = ( a_Y * a_CubeMap.FaceWidth + a_X ) * formatInfo.Bytes();
		const uint32_t faceOffset = a_Face * a_CubeMap.FaceStride;
		const uint32_t byteOffset = faceOffset + pixelIndex;
		const Span<const byte_t> pixelData = { &a_CubeMap.PixelData[byteOffset], formatInfo.Bytes() };

		return formatInfo.ConvertToColor( pixelData );
	}

	void SetPixelOnCubemap( CubeMap& a_CubeMap, uint32_t a_Face, uint32_t a_X, uint32_t a_Y, const Color4& a_Color, ERHIFormat a_Format )
	{
		if ( a_Face >= 6 || a_X >= a_CubeMap.FaceWidth || a_Y >= a_CubeMap.FaceWidth )
		{
			return;
		}

		const RHIFormatInfo formatInfo = GetRHIFormatInfo( a_Format );

		const uint32_t pixelIndex = ( a_Y * a_CubeMap.FaceWidth + a_X ) * formatInfo.Bytes();
		const uint32_t faceOffset = a_Face * a_CubeMap.FaceStride;
		const uint32_t byteOffset = faceOffset + pixelIndex;

		const Span<byte_t> pixelData = { &a_CubeMap.PixelData[byteOffset], formatInfo.Bytes() };
		formatInfo.ConvertFromColor( pixelData, a_Color );
	}

	CubeMap ConvolveDiffuse( ERHIFormat a_Format, const CubeMap& a_Env, uint32_t a_OutSize, uint32_t a_SampleCount )
	{
		CubeMap irradiance{};
		irradiance.FaceWidth = a_OutSize;
		irradiance.FaceStride = a_OutSize * a_OutSize * GetRHIFormatInfo( a_Format ).Bytes();
		irradiance.PixelData.Resize( irradiance.FaceStride * 6 );

		for ( uint32_t face = 0; face < 6; ++face )
		{
			for ( uint32_t y = 0; y < a_OutSize; y++ )
			for ( uint32_t x = 0; x < a_OutSize; x++ )
			{
				const Vector3 N = CubemapTexelDirection( face, x, y, a_OutSize ).Normalized();

				Vector3 sum{ 0.0f };
				float totalWeight = 0.0f;

				for ( uint32_t i = 0; i < a_SampleCount; ++i )
				{
					const Vector2 Xi = Hammersley( i, a_SampleCount );
					const Vector3 L = ImportanceSampleCosineHemisphere( Xi, N );
					const float NdotL = Math::Dot( N, L );

					if ( NdotL > 0.0f )
					{
						const Color4 envColor = SampleEnvironmentCubemap( a_Format, a_Env, L );
						sum += Vector3( envColor.r, envColor.g, envColor.b ) * NdotL;
						totalWeight += NdotL;
					}
				}


				sum = sum / Math::Max( totalWeight, 1e-4f );
				SetPixelOnCubemap( irradiance, face, x, y, Color4( sum.X, sum.Y, sum.Z, 1.0f ), a_Format );
			}
		}

		return irradiance;
	}

	Array<CubeMap> PrefilterSpecular(
		ERHIFormat a_Format,
		const CubeMap& a_Env,
		uint32_t a_CubeMapSize,
		uint32_t a_MipLevels,
		uint32_t a_SampleCount )
	{
		a_MipLevels = a_MipLevels == 0 ? (uint32_t)( Math::Floor( Math::Log2( (float)a_CubeMapSize ) ) + 1 ) : a_MipLevels;

		Array<CubeMap> mips;
		mips.Resize( a_MipLevels );

		const RHIFormatInfo formatInfo = GetRHIFormatInfo( a_Format );

		for ( uint32_t mip = 0; mip < a_MipLevels; ++mip )
		{
			const uint32_t size = Math::Max( 1u, a_CubeMapSize >> mip );
			const float roughness = (float)mip / (float)( a_MipLevels - 1 );

			CubeMap cube{};
			cube.FaceWidth = size;
			cube.FaceStride = size * size * formatInfo.Bytes();
			cube.PixelData.Resize( cube.FaceStride * 6 );

			for ( uint32_t face = 0; face < 6; ++face )
			{
				for ( uint32_t y = 0; y < size; ++y )
				for ( uint32_t x = 0; x < size; ++x )
				{
					const Vector3 R = CubemapTexelDirection( face, x, y, size ).Normalized();
					const Vector3 N = R;
					const Vector3 V = R;

					Vector3 prefilteredColor( 0.0f );
					float totalWeight = 0.0f;

					// Reduce sample count for higher mip levels exponentially
					uint32_t mipSampleCount = a_SampleCount / ( 1 << mip );

					for ( uint32_t i = 0; i < mipSampleCount; ++i )
					{
						const Vector2 Xi = Hammersley( i, mipSampleCount );
						const Vector3 H = ImportanceSampleGGX( Xi, roughness, N );
						const Vector3 L = ( H * 2.0f * Math::Dot( V, H ) - V ).Normalized();

						const float NdotL = Math::Dot( N, L );
						if ( NdotL > 0.0f )
						{
							const Color4 envColor = SampleEnvironmentCubemap( a_Format, a_Env, L );
							prefilteredColor += Vector3( envColor.r, envColor.g, envColor.b ) * NdotL;
							totalWeight += NdotL;
						}
					}

					prefilteredColor = prefilteredColor / Math::Max( totalWeight, 1e-4f );

					SetPixelOnCubemap( cube, face, x, y, Color4(
						prefilteredColor.X,
						prefilteredColor.Y,
						prefilteredColor.Z,
						1.0f ), a_Format );
				}
			}

			mips[mip] = std::move( cube );
		}

		return mips;
	}

}

#endif // USE_ASSET_IMPORTERS
