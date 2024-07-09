#type Vertex
#version 410

layout(location = 0) in vec3 aPosition;

uniform int aID;
uniform mat4 uPVM;		

out vec2 vID;

void main()
{	
	gl_Position = uPVM * vec4(aPosition, 1);
	vID = aID;
}

#type Fragment
#version 410 core

layout(location = 0) out int oID;

in vec2 vID;						

uniform sampler2D uTexture;

void main()
{
	oFragColor = vec4(texture(uTexture, vUV).rgb, 1);
	vID = aID;
}