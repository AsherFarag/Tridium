#include "tripch.h"
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>

namespace Tridium {

    static constexpr StringView VS = R"(
    #include "Core.hlsli"
    #include "SkyBox_ShaderInterop.h"

    INLINED_CONSTANTS( Constants, InlinedConstants_SkyBox );

    struct PS_INPUT
    {
        float4 Position : SV_POSITION;
        float3 LocalPosition : LOCAL_POSITION;
    };

    PS_INPUT VSMain(float3 a_Position : POSITION)
    {
        // Remove translation from view
        float4x4 rotView = Constants.View;
        rotView[0][3] = 0.0f;
        rotView[1][3] = 0.0f;
        rotView[2][3] = 0.0f;
    
        PS_INPUT output;
        output.Position = mul(Constants.Projection, mul(rotView, float4(a_Position, 1.0f)));
        output.Position = output.Position.xyww; // Set depth to far plane
    
        output.LocalPosition = a_Position;
        return output;
    }
	)";

    static constexpr StringView PS = R"(
    #include "Core.hlsli"
    #include "Lighting/Tonemapping.hlsli"

    struct PS_INPUT
    {
        float4 Position : SV_POSITION;
        float3 LocalPosition : LOCAL_POSITION;
    };

    COMBINED_SAMPLER( SkyboxMap, TextureCube, 0 );

    float4 PSMain(PS_INPUT input) : SV_TARGET0
    {
        float3 direction = normalize(input.LocalPosition);
        float3 color = SampleTexture(SkyboxMap, direction).rgb;

        //color = Tonemap_ACESFilm(color); // Tone mapping
        color = Tonemap_AGX(color); // Tone mapping
        color = pow(color, 1.0f / 2.2f); // Gamma correction

        return float4(color, 1.0f);
    }

	)";

    static const auto SkyBox = ShaderFamily{}
        .SetName( "SkyBox" )
        .SetShaderSource( ERHIShaderType::Vertex, VS )
        .SetShaderSource( ERHIShaderType::Pixel, PS );

    REGISTER_SHADER_FAMILY( SkyBox );

} // namespace Tridium