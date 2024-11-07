#pragma type Vertex
#version 420 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_UV;
uniform mat4 u_PVM;
uniform mat4 u_Model;

out vec3 v_WorldPos;
out mat3 v_TBN; 
out vec2 v_UV;

void main()
{
	vec3 T = normalize(vec3(u_Model * vec4(a_Tangent,   0.0)));
    vec3 N = normalize(vec3(u_Model * vec4(a_Normal,    0.0)));
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = cross(N, T);
    v_TBN = mat3(T, B, N);

	v_WorldPos = ( u_Model * vec4(a_Position, 1.0) ).xyz;
	v_UV = a_UV;

	gl_Position = u_PVM * vec4(a_Position, 1.0);
}

#pragma type Fragment
#version 420 core
layout (location = 0) out vec3 g_Position;
layout (location = 1) out vec3 g_Normal;
layout (location = 2) out vec3 g_Albedo;
layout (location = 3) out vec3 g_AORM;
layout (location = 4) out vec3 g_Emission;

in vec3 v_WorldPos;
in mat3 v_TBN; 
in vec2 v_UV;

// PBR Textures
uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_MetallicTexture;
uniform sampler2D u_RoughnessTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_OpacityTexture;
uniform sampler2D u_EmissiveTexture;
uniform sampler2D u_AOTexture;

void main()
{
	// Temp Opacity calculation
	if ( texture(u_OpacityTexture, v_UV).a < 0.2 )
		discard; 

	g_Position = v_WorldPos;
	g_Albedo = texture(u_AlbedoTexture, v_UV).rgb;

	// Calculate Normal using the normal map and TBN matrix
	g_Normal = texture(u_NormalTexture, v_UV).rgb;
	g_Normal = g_Normal * 2.0 - 1.0;
	g_Normal = normalize(v_TBN * g_Normal);

	// NOTE: AO, Roughness, and Metallic could all use the same texture as we only need one channel for each. 
	// r = ambient occlusion
	// g = roughness
	// b = metallic
	g_AORM.r = texture(u_AOTexture, v_UV).r;
	g_AORM.g = texture(u_RoughnessTexture, v_UV).g;
	g_AORM.b = texture(u_MetallicTexture, v_UV).b;
	g_Emission = texture(u_EmissiveTexture, v_UV).rgb;
}