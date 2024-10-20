#type Vertex
#version 420 core
layout(location = 0) in vec3 a_Position;

out vec3 v_UV;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	v_UV = a_Position;
    vec4 pos = u_Projection * u_View * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}

#type Fragment
#version 420 core
layout(location = 0) out vec4 o_Color;

in vec3 v_UV;

uniform samplerCube u_EnvironmentMap;

void main()
{
	//o_Color = vec4(0.0,1.0,0.5,1.0);
	o_Color = texture(u_EnvironmentMap, v_UV);	
}
