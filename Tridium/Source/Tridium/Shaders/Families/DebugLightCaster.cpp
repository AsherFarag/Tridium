#include "tripch.h"
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>

namespace Tridium {

    static constexpr StringView VS = R"(
    #include "Core.hlsli"
    #include "DebugLightCaster_ShaderInterop.h"

    INLINED_CONSTANTS( Constants, InlinedConstants_DebugLightCaster );

    struct PS_INPUT
    {
        float4 Position : SV_POSITION;
    };

    PS_INPUT VSMain(float3 a_Position : POSITION)
    {
        PS_INPUT output;
        output.Position = mul(Constants.PVM, float4(a_Position, 1.0f));
        return output;
    }
	)";

    static constexpr StringView PS = R"(
    #include "Core.hlsli"
    #include "DebugLightCaster_ShaderInterop.h"

    INLINED_CONSTANTS( Constants, InlinedConstants_DebugLightCaster );

    struct PS_INPUT
    {
        float4 Position : SV_POSITION;
    };

    float4 PSMain(PS_INPUT input) : SV_TARGET0
    {
        return float4( Constants.Color.rgb, 1.0f );
    }
	)";

    static const auto DebugLightCaster = ShaderFamily{}
        .SetName( "DebugLightCaster" )
        .SetShaderSource( ERHIShaderType::Vertex, VS )
        .SetShaderSource( ERHIShaderType::Pixel, PS );

    REGISTER_SHADER_FAMILY( DebugLightCaster );

} // namespace Tridium