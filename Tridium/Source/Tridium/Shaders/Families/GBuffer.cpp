#include "tripch.h"
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>

namespace Tridium {

    static constexpr StringView VS = R"(
    #include "Globals.hlsli"
    #include "GBuffer_ShaderInterop.h"

    INLINED_CONSTANTS( Constants, InlinedConstants_GBuffer );

    struct VS_INPUT
    {
		float3 Position : POSITION;
		float3 Normal : NORMAL;
		float4 Tangent : TANGENT;
		float2 TexCoord : TEXCOORD0;
    };

    struct PS_INPUT
    {
        float4 Position : SV_POSITION;
        float3 WorldPosition : WORLD_POSITION;
        float3x3 TBN : TANGENT_SPACE;
        float2 UV  : TEXCOORD0;
    };

    PS_INPUT VSMain(VS_INPUT a_Input)
    {
        float3 N = normalize( mul( Constants.Model, float4( a_Input.Normal, 0.0f ) ).xyz );
        float3 T = normalize( mul( Constants.Model, a_Input.Tangent ).xyz );
        float3 B = cross(N, T);

        PS_INPUT output;
        output.Position = mul( Constants.PVM, float4( a_Input.Position, 1.0f ) );
        output.WorldPosition = mul( Constants.Model, float4( a_Input.Position, 1.0f ) ).xyz;
        output.UV  = a_Input.TexCoord;
        output.TBN = transpose( float3x3( T, B, N ) );
        return output;
    }
	)";

    static constexpr StringView PS = R"(
    #include "Globals.hlsli"
    #include "Material.hlsli"

    struct PS_INPUT
    {
        float4 Position      : SV_POSITION;
        float3 WorldPosition : WORLD_POSITION;
        float3x3 TBN         : TANGENT_SPACE;
        float2 UV            : TEXCOORD0;
    };

    struct PS_OUTPUT
    {
        float4 Position     : SV_Target0;
        float4 Albedo       : SV_Target1;
        float4 Normal       : SV_Target2;
        float4 MetalRoughAO : SV_Target3;
        float4 Emission     : SV_Target4;
    };

    COMBINED_SAMPLER( AlbedoMap, Texture2D, 0 );
    COMBINED_SAMPLER( NormalMap, Texture2D, 1 );
    COMBINED_SAMPLER( MetallicMap, Texture2D, 2 );
    COMBINED_SAMPLER( RoughnessMap, Texture2D, 3 );
    COMBINED_SAMPLER( EmissiveMap, Texture2D, 4 );
    COMBINED_SAMPLER( AmbientOcclusionMap, Texture2D, 5 );

    CONSTANT_BUFFER( u_MaterialProps, MaterialProperties, 0 );

    PS_OUTPUT PSMain( PS_INPUT a_Input )
    {
        PS_OUTPUT output;

        // Position
        output.Position = float4( a_Input.WorldPosition, 1.0f );

        // Albedo
        float4 albedoColor = u_MaterialProps.AlbedoColor;
        albedoColor *= SampleTexture( AlbedoMap, a_Input.UV );
        output.Albedo = albedoColor;

        // Normal
        float3 normal = SampleTexture( NormalMap, a_Input.UV ).xyz * 2.0f - 1.0f; // Transform from [0,1] to [-1,1]
        normal = normalize( mul( a_Input.TBN, normal ) );
        output.Normal = float4( normal * 0.5f + 0.5f, 1.0f ); // Pack to [0,1] range

        // Metallic, Roughness, AO
        float metallic = u_MaterialProps.MetallicIntensity;
        metallic *= SampleTexture( MetallicMap, a_Input.UV ).r;
        float roughness = u_MaterialProps.RoughnessIntensity;
        roughness *= SampleTexture( RoughnessMap, a_Input.UV ).r;
        float ao = SampleTexture( AmbientOcclusionMap, a_Input.UV ).r;
        output.MetalRoughAO = float4( metallic, roughness, ao, 1.0f );

        // Emission
        float3 emission = u_MaterialProps.EmissiveIntensity * SampleTexture( EmissiveMap, a_Input.UV ).rgb;
        output.Emission = float4( emission, 1.0f );

        return output;
    }
	)";

    static const auto GBuffer = ShaderFamily{}
        .SetName( "GBuffer" )
        .SetShaderSource( ERHIShaderType::Vertex, VS )
        .SetShaderSource( ERHIShaderType::Pixel, PS );

    REGISTER_SHADER_FAMILY( GBuffer );

} // namespace Tridium