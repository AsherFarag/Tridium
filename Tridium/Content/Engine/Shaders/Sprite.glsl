#type Vertex
#version 410

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

uniform mat4 uPVM;

out vec2 vUV;			

void main()
{	
	gl_Position = uPVM * vec4(aPosition, 1);
	vUV = aUV;
}

#type Fragment
#version 410 core

out vec4 oFragColor;

in vec2 vUV;						

uniform sampler2D uTexture;

void main()
{
	oFragColor = texture(uTexture, vUV).rgba;
}