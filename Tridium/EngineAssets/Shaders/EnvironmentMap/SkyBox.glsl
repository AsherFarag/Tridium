#type Vertex
#version 420 core
layout(location = 0) in vec3 a_Position;

out vec3 v_UV;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	v_UV = a_Position;
    vec4 pos = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}

#type Fragment
#version 420 core
layout(location = 0) out vec4 o_Color;

in vec3 v_UV;

uniform samplerCube u_EnvironmentMap;
uniform float u_Exposure;
uniform float u_Gamma;
uniform float u_Roughness;

vec3 ExposureToneMapping(vec3 color, float exposure) 
{
    return vec3(1.0) - exp(-color * exposure);
}

vec3 GammaCorrection(vec3 color, float gamma) 
{
    return pow(color, vec3(1.0 / gamma));
}

void main()
{
    vec3 hdrColor = textureLod(u_EnvironmentMap, v_UV, u_Roughness).rgb;
    vec3 mapped = ExposureToneMapping(hdrColor, u_Exposure);
    mapped = GammaCorrection(mapped, u_Gamma);

    o_Color = vec4( mapped, 1.0 );
}
