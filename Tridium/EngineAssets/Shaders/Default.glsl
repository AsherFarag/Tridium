#type Vertex
#version 420

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV[8];
layout(location = 3) in vec3 aTangent;

uniform mat4 uPVM;

out vec4 vPosition;
out vec3 vNormal;
out vec2 vUV[8];
out vec3 vTangent;
out float vDepth;		

void main()
{	
	gl_Position = uPVM * vec4(aPosition, 1);
	vPosition = vec4(aPosition, 1);
	vNormal = aNormal;
	for (int i = 0; i < 8; ++i)
	{
		vUV[i] = aUV[i];
	}
	vTangent = aTangent;
	vDepth = gl_Position.z;
}

#type Fragment
#version 420 core

layout(location = 0) out vec4 oFragColor;

in vec4 vPosition;
in vec3 vNormal;
in vec2 vUV[8];
in vec3 vTangent;
in float vDepth;					

uniform vec4 uColor;

layout(binding = 0) uniform sampler2D uBaseColorTexture;
layout(binding = 1) uniform sampler2D uNormalMapTexture;
layout(binding = 2) uniform sampler2D uMetallicTexture;
layout(binding = 3) uniform sampler2D uRoughnessTexture;
layout(binding = 4) uniform sampler2D uEmissiveTexture;

void main()
{
	// Sample base color and emissive textures
	vec3 baseColor = texture(uBaseColorTexture, vUV[0]).rgb;
	vec3 emissiveColor = 2.0f * texture(uEmissiveTexture, vUV[0]).rgb;
	
	// Calculate the final fragment color
	vec3 ambientColor = baseColor;
	vec3 finalColor = ambientColor + emissiveColor;
	
	// Output the final color
	oFragColor = uColor * vec4(finalColor, 1.0);
}