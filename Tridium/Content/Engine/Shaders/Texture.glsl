#type Vertex
#version 410

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTextureCoords;

uniform mat4 uPVM;

out vec2 vTextureCoords;			

void main()
{	
	gl_Position = uPVM * vec4(aPosition, 1);
	vPosition =  vec4(aPosition, 1);
	vTextureCoords = aTextureCoords;
}

#type Fragment
#version 410 core

layout (location = 0) out vec4 aFragColour;

in vec2 vTextureCoords;						

uniform sampler2D uTexture;

void main()
{
	aFragColour = vec4(texture(uTexture, vTextureCoords).rgb, 1);
}