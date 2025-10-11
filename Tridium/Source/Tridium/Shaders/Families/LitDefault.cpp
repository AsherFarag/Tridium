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
    STRUCTURED_BUFFER( PointLights, PointLight, 7 );
    STRUCTURED_BUFFER( SpotLights, SpotLight, 8 );

    float2 ApproxBRDF(float NdotV, float roughness)
    {
        // Approximation of the integrated BRDF used by UE4 (no LUT)
        const float4 c0 = float4(-1.0, -0.0275, -0.572, 0.022);
        const float4 c1 = float4( 1.0,  0.0425,  1.04, -0.04);
        float4 r = roughness * c0 + c1;
        float a004 = min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
        return float2(-1.04, 1.04) * a004 + r.zw;
    }

    float4 PSMain(PS_INPUT a_Input) : SV_TARGET0
    {
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
        float3 V = normalize(Constants.CameraPosition - position);
    
        float3 lighting = 0;
    
        // --- Directional light ---
        {
            DirectionalLight light = Constants.DirectionalLight;
            float3 L = normalize(-light.Direction);
            float NoL = max(dot(N, L), 0.001);
            float VoL   = dot(V, L);
    
            float3 spec = CalcDirLightBRDF(albedo, roughness, metallic, N, V, L, NoL, VoL);
            float3 radiance = light.Color * light.Intensity;
    
            float3 diffuse = albedo / PI;
    
            lighting += (diffuse * 0.0001 + spec) * radiance * NoL;
        }
    
        // --- Point lights ---
        [loop]
        for (uint i = 0; i < Constants.NumPointLights; ++i)
        {
            const PointLight light = PointLights[i];
            const float distance = length(light.Position - position);
            const float attenuation = AttenuateCusp(distance, light.Radius, light.Intensity, light.Falloff);

            float3 L = normalize(light.Position - position);
            float3 V = normalize(Constants.CameraPosition - position);
            float NoL = max(dot(N, L), 0.001);
            float VoL   = dot(V, L);
            
            float3 brdf = CalcPointLightBRDF(albedo, roughness, metallic, N, V, L, NoL, VoL);
            float3 radiance = light.Color * light.Intensity * attenuation;
            
            lighting += brdf * radiance * NoL;
        }
    
        // --- Image-based lighting (IBL) ---
        float3 F = FresnelSchlickRoughness(saturate(dot(N, V)), lerp(float3(0.04,0.04,0.04), albedo, metallic), roughness);
    
        float3 kS = F;
        float3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;
    
        // Diffuse IBL
        float3 irradiance = SampleTexture(IrradianceMap, N).rgb;
        float3 diffuseIBL = irradiance * albedo;
    
        // Specular IBL
        float3 R = reflect(-V, N);
        float lod = roughness * roughness * 9.0;
        float3 prefilteredColor = SampleTextureLod(RadianceMap, R, lod).rgb;
        float2 brdf = ApproxBRDF(saturate(dot(N, V)), roughness);
        float3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);
    
        lighting += kD * diffuseIBL + specularIBL;
    
        // --- AO + emission ---
        float3 color = lighting * ao + emission;
    
        // --- Tone mapping ---
        color = 1.0 - exp(-color);
    
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