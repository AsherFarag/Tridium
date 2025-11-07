#include "tripch.h"
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>

namespace Tridium {

	static constexpr StringView VS = R"(
	#include "Core.hlsli"

	struct InlinedConstants_Depth
	{
		float4x4 PVM;
	};

	INLINED_CONSTANTS( InlinedConstants, InlinedConstants_Depth );

	struct VS_INPUT
	{
		float3 Position : POSITION;
	};

	struct PS_INPUT
	{
		float4 Position : SV_POSITION;
	};

	PS_INPUT VSMain(VS_INPUT a_Input)
	{
		PS_INPUT output;
		output.Position = mul(InlinedConstants.PVM, float4(a_Input.Position, 1.0f));
		return output;
	}
	)";

	static const auto DepthFamily = ShaderFamily{}
		.SetName( "Depth" )
		.SetShaderSource( ERHIShaderType::Vertex, VS );

	REGISTER_SHADER_FAMILY( DepthFamily );

} // namespace Tridium