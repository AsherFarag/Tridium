#type Vertex
#version 420 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_UV;

uniform mat4 u_PVM;

void main()
{
	gl_Position = u_PVM * vec4(a_Position, 1.0);
}

#type Fragment
#version 420 core
layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = vec4( 1.0, 0.85, 0.0, 1.0);
}