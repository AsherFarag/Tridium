#include "tripch.h"
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>

namespace Tridium {

	static constexpr StringView VS = R"(
    #include "Globals.hlsli"

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
    #include "Globals.hlsli"
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

    float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
    {
        const float shinyness = 1.0 - roughness;
        return F0 + (max(float3(shinyness,shinyness,shinyness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
    }

    float2 DisneyBRDF(float NdotV, float NdotL, float LdotH, float VdotH, float roughness, float metallic)
    {
        float alpha = roughness * roughness;
        float alpha2 = alpha * alpha;
        // Distribution GGX
        float denom = (LdotH * LdotH) * (alpha2 - 1.0) + 1.0;
        float D = alpha2 / max(PI * denom * denom, 1e-4);
        // Geometry Smith
        float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
        float Gv = NdotV / (NdotV * (1.0 - k) + k);
        float Gl = NdotL / (NdotL * (1.0 - k) + k);
        float G = Gv * Gl;
        // Fresnel Schlick
        float3 F0 = lerp(float3(0.04, 0.04, 0.04), float3(1.0, 1.0, 1.0), metallic);
        float3 F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
        return float2(D * G, F.r); // Return specular term and Fresnel term
    }

    float2 ApproxBRDF(float NdotV, float roughness)
    {
        // Approximation of the integrated BRDF used by UE4 (no LUT)
        const float4 c0 = float4(-1.0, -0.0275, -0.572, 0.022);
        const float4 c1 = float4( 1.0,  0.0425,  1.04, -0.04);
        float4 r = roughness * c0 + c1;
        float a004 = min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
        return float2(-1.04, 1.04) * a004 + r.zw;
    }

    float4 PSMain(PS_INPUT input) : SV_TARGET0
    {
        float3 position = SampleTexture(PositionMap, input.UV).xyz;
        float3 albedo   = SampleTexture(AlbedoMap, input.UV).xyz;
        float3 normal   = normalize(SampleTexture(NormalMap, input.UV).xyz * 2.0f - 1.0f);
        float3 mra      = SampleTexture(MetalRoughAOMap, input.UV).xyz;
        float3 emission = SampleTexture(EmissionMap, input.UV).xyz;
    
        float metallic  = mra.r;
        float roughness = saturate(mra.g);
        float ao        = mra.b;
    
        float3 N = normal;
        float3 V = normalize(Constants.CameraPosition - position);
    
        float3 lighting = 0;

        // Apply directional light
        {
            float3 L = normalize(-Constants.DirectionalLight.Direction);
            float3 H = normalize(L + V);
    
            float NdotL = max(dot(N, L), 0.0);
            float NdotV = max(dot(N, V), 0.0);
            float NdotH = max(dot(N, H), 0.0);
            float VdotH = max(dot(V, H), 0.0);
    
            // Microfacet BRDF
            float alpha = roughness * roughness;
            float alpha2 = alpha * alpha;
    
            float denom = (NdotH * NdotH) * (alpha2 - 1.0) + 1.0;
            float D = alpha2 / max(PI * denom * denom, 1e-4);
    
            float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
            float Gv = NdotV / (NdotV * (1.0 - k) + k);
            float Gl = NdotL / (NdotL * (1.0 - k) + k);
            float G = Gv * Gl;
    
            float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
            float3 F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
    
            float3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, 1e-4);
            float3 diffuse  = (1.0 - F) * albedo / PI;
    
            float3 radiance = Constants.DirectionalLight.Color * Constants.DirectionalLight.Intensity;
    
            lighting += (diffuse + specular) * radiance * NdotL;
        }
    
        [loop]
        for (uint i = 0; i < Constants.NumPointLights; ++i)
        {
            PointLight light = PointLights[i];
            float3 L = normalize(light.Position - position);
            float3 H = normalize(L + V);
    
            float distance = length(light.Position - position);
            float attenuation = saturate(1.0f - (distance / light.Radius));
    
            float NdotL = max(dot(N, L), 0.0);
            float NdotV = max(dot(N, V), 0.0);
            float NdotH = max(dot(N, H), 0.0);
            float VdotH = max(dot(V, H), 0.0);
    
            // Microfacet BRDF
            float alpha = roughness * roughness;
            float alpha2 = alpha * alpha;
    
            float denom = (NdotH * NdotH) * (alpha2 - 1.0) + 1.0;
            float D = alpha2 / max(PI * denom * denom, 1e-4);
    
            float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
            float Gv = NdotV / (NdotV * (1.0 - k) + k);
            float Gl = NdotL / (NdotL * (1.0 - k) + k);
            float G = Gv * Gl;
    
            float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
            float3 F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
    
            float3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, 1e-4);
            float3 diffuse  = (1.0 - F) * albedo / PI;
    
            float3 radiance = light.Color * light.Intensity * attenuation;
    
            lighting += (diffuse + specular) * radiance * NdotL;
        }

        float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), lerp(float3(0.04,0.04,0.04), albedo, metallic), roughness);

        float3 kS = F;
        float3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;
        
        float3 irradiance = SampleTexture(IrradianceMap, N).rgb;
        float3 diffuse    = irradiance * albedo;

        float3 R = reflect(-V, N);
        float lod = roughness * roughness * 9;
        float3 prefilteredColor = SampleTextureLod(RadianceMap, R, lod).rgb;
        
        float NdotV = saturate(dot(N, V));
        float2 brdf = ApproxBRDF(NdotV, roughness);

        float3 specular = prefilteredColor * (F * brdf.x + brdf.y);

        lighting += kD * diffuse + specular;

        // Apply AO + emission
        float3 color = lighting * ao + emission;

        // Exposure tone mapping
        color = float3(1.0, 1.0, 1.0) - exp(-color);
        // Gamma correction
        const float3 gamma = float3(1.0/2.2, 1.0/2.2, 1.0/2.2); 
        color = pow(color, gamma); // Gamma correction
    
        return float4(color, 1.0);
    }

	)";

	static const auto LitDefaultFamily = ShaderFamily{}
		.SetName( "LitDefault" )
		.SetShaderSource( ERHIShaderType::Vertex, VS )
		.SetShaderSource( ERHIShaderType::Pixel, PS );

	REGISTER_SHADER_FAMILY( LitDefaultFamily );

} // namespace Tridium