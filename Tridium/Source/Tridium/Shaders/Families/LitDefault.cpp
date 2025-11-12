#include "tripch.h"
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>

namespace Tridium {

	static constexpr StringView VS = R"(
    #include "Core.hlsli"

    struct VS_INPUT
    {
        float3 Position : POSITION;
        float2 UV : TEXCOORD0;
    };

    struct PS_INPUT
    {
        float4 Position : SV_POSITION;
        float2 UV  : TEXCOORD0;
    };

    PS_INPUT VSMain(VS_INPUT a_Input)
    {
        PS_INPUT output;
        output.Position = float4(a_Input.Position, 1.0f);
        output.UV  = a_Input.UV;
        return output;
    }
	)";

	static constexpr StringView PS = R"(

    #ifdef HIGH_QUALITY
        #define BRDF_HIGH_QUALITY
    #endif

    #include "Core.hlsli"
    #include "Lighting/BRDF.hlsli"
    #include "Lighting/Lighting.hlsli"
    #include "Lighting/ImageBasedLighting.hlsli"
    #include "Lighting/Tonemapping.hlsli"
    #include "LitDefault_ShaderInterop.h"

    struct PS_INPUT
    {
        float4 Position : SV_POSITION;
        float2 UV  : TEXCOORD0;
    };

    INLINED_CONSTANTS( Constants, InlinedConstants_LitDefault );

    COMBINED_SAMPLER( PositionMap, Texture2D, 0 );
    COMBINED_SAMPLER( AlbedoMap, Texture2D, 1 );
    COMBINED_SAMPLER( NormalMap, Texture2D, 2 );
    COMBINED_SAMPLER( MetalRoughAOMap, Texture2D, 3 );
    COMBINED_SAMPLER( EmissionMap, Texture2D, 4 );
    COMBINED_SAMPLER( IrradianceMap, TextureCube, 5 );
    COMBINED_SAMPLER( RadianceMap, TextureCube, 6 );
    COMBINED_COMP_SAMPLER( DirectionalShadowMap, Texture2D, 7 );
    STRUCTURED_BUFFER( PointLights, PointLight, 8 );
    STRUCTURED_BUFFER( SpotLights, SpotLight, 9 );

    float4 PSMain(PS_INPUT a_Input) : SV_TARGET0
    {
        const float3 camPosition = u_RenderView.ViewPosition.xyz;

        // Sample G-buffer textures
        float3 position = SampleTexture(PositionMap, a_Input.UV).xyz;
        float3 albedo   = SampleTexture(AlbedoMap, a_Input.UV).xyz;
        float3 normal   = normalize(SampleTexture(NormalMap, a_Input.UV).xyz * 2.0f - 1.0f);
        float3 mra      = SampleTexture(MetalRoughAOMap, a_Input.UV).xyz;
        float3 emission = SampleTexture(EmissionMap, a_Input.UV).xyz;
    
        float metallic  = mra.r;
        float roughness = saturate(mra.g);
        float ao        = mra.b;
    
        float3 N = normal;
        float3 V = normalize(camPosition - position);
    
        // Direct lighting from light sources such as point, direct and spot lights.
        float3 directLighting = 0;
    
        // --- Directional light ---
        {
            DirectionalLight light = Constants.DirectionalLight;
            float3 L = normalize(-light.Direction);
            float NoL = max(dot(N, L), 0.001);
            float VoL   = dot(V, L);
    
            float3 radiance = light.Color * light.Intensity;
            float3 diffuse = albedo / PI;

            float3 brdf = CalculateBRDF(albedo, roughness, metallic,
                                        N, V, L, NoL, VoL,
                                        BRDF_DIR_LIGHT_RADIUS_TAN, light.SpecularScale);
    
            // Shadowing
            float4 lightSpacePos = mul(light.LightSpaceMatrix, float4(position, 1.0f));
            float shadowFactor = SampleDirectionalShadowMap(
                PassCombinedSampler(DirectionalShadowMap),
                lightSpacePos, N, light.Direction);

            radiance *= shadowFactor;
            directLighting += brdf * radiance * NoL;
        }
    
        // --- Point lights ---
        [loop]
        for (uint i = 0; i < Constants.NumPointLights; ++i)
        {
            const PointLight light = PointLights[i];
            const float distance = length(light.Position - position);
            const float attenuation = AttenuateCusp(distance, light.Radius, light.Intensity, light.Falloff);

            float3 L = normalize(light.Position - position);
            float3 V = normalize(camPosition - position);
            float NoL = max(dot(N, L), 0.001);
            float VoL   = dot(V, L);
            
            float3 brdf = CalculateBRDF(albedo, roughness, metallic,
                                        N, V, L, NoL, VoL,
                                        light.SourceSize, light.SpecularScale);

            float3 radiance = light.Color * light.Intensity * attenuation;
            
            directLighting += brdf * radiance * NoL;
        }

        // --- Spot lights --- 
        [loop]
        for (uint i = 0; i < Constants.NumSpotLights; ++i)
        {
            const SpotLight light = SpotLights[i];
            const float3 lightToPixel = position - light.Position;
            const float distance = length(lightToPixel);
            const float3 L = normalize(-lightToPixel);
            const float NoL = max(dot(N, L), 0.001);
            const float VoL   = dot(V, L);
            const float attenuation = AttenuateCusp(distance, light.Range, light.Intensity, light.Falloff);
            const float spotFactor = SpotLightFactor(L, light.Direction, light.InnerConeCos, light.OuterConeCos);
            float3 brdf = CalculateBRDF(albedo, roughness, metallic,
                                        N, V, L, NoL, VoL,
                                        light.SourceSize, light.SpecularScale);
            float3 radiance = light.Color * light.Intensity * attenuation * spotFactor;
            directLighting += brdf * radiance * NoL;
        }
    
        // --- Image-based lighting (IBL) ---
        float3 environmentLighting = EvaluateIBL( albedo, roughness, metallic, N, V,
                                             PassCombinedSampler( IrradianceMap ), 
                                             PassCombinedSampler( RadianceMap ) );

        // --- Combine lighting ---
        float3 lighting = directLighting + environmentLighting;

        // --- AO + emission ---
        float3 color = lighting * ao + emission;
    
        // --- Tone mapping ---
        //color = Tonemap_AGX(color);
        color = Tonemap_ACESFilm(color);
    
        // --- Gamma correction ---
        const float3 gamma = float3(1.0/2.2, 1.0/2.2, 1.0/2.2);
        color = pow(color, gamma);
    
        return float4(color, 1.0);
    }
	)";

	static const auto LitDefaultFamily = ShaderFamily{}
		.SetName( "LitDefault" )
		.SetShaderSource( ERHIShaderType::Vertex, VS )
		.SetShaderSource( ERHIShaderType::Pixel, PS );

	REGISTER_SHADER_FAMILY( LitDefaultFamily );

} // namespace Tridium