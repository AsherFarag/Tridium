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

    struct PS_INPUT
    {
        float4 Position : SV_POSITION;
        float2 UV  : TEXCOORD0;
    };

    COMBINED_SAMPLER( PositionMap, Texture2D, 0 );
    COMBINED_SAMPLER( AlbedoMap, Texture2D, 1 );
    COMBINED_SAMPLER( NormalMap, Texture2D, 2 );
    COMBINED_SAMPLER( MetalRoughAOMap, Texture2D, 3 );
    COMBINED_SAMPLER( EmissionMap, Texture2D, 4 );

    float4 PSMain(PS_INPUT input) : SV_TARGET0
    {
        // Sample the G-Buffer textures
        float3 position = SampleTexture( PositionMap, input.UV ).xyz;
        float3 albedo = SampleTexture( AlbedoMap, input.UV ).xyz;
        float3 normal = SampleTexture( NormalMap, input.UV ).xyz * 2.0f - 1.0f; // Decode normal
        float3 metallicRoughnessAO = SampleTexture( MetalRoughAOMap, input.UV ).xyz;
        float3 emission = SampleTexture( EmissionMap, input.UV ).xyz;

        // Unpack the metallic, roughness, and ambient occlusion values
        float metallic = metallicRoughnessAO.r;
        float roughness = metallicRoughnessAO.g;
        float ambientOcclusion = metallicRoughnessAO.b;

        // Simple lighting calculation (for demonstration purposes)
        float3 lightDir = normalize(float3(0.5f, 1.0f, -0.5f));
        float3 viewDir = normalize(float3(0.0f, 0.0f, -1.0f) - position);
        float3 reflectDir = reflect(-lightDir, normal);
        float3 ambient = 0.1f * albedo;
        float3 diffuse = max(dot(normal, lightDir), 0.0f) * albedo;
        float3 specular = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f) * (metallic * 0.5f + 0.5f);

        float3 color = ambient + diffuse + specular + emission;
        color *= 1 - ambientOcclusion; // Apply ambient occlusion
        return float4(color, 1.0f);
    }
	)";

	static const auto LitDefaultFamily = ShaderFamily{}
		.SetName( "LitDefault" )
		.SetShaderSource( ERHIShaderType::Vertex, VS )
		.SetShaderSource( ERHIShaderType::Pixel, PS );

	REGISTER_SHADER_FAMILY( LitDefaultFamily );

} // namespace Tridium