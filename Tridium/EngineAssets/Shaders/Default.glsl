#type Vertex
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

#type Fragment
#version 420 core
layout(location = 0) out vec4 o_Color;

const float PI = 3.14159265359;

uniform vec3 u_CameraPosition;

// Vertex Attributes
in vec3 v_WorldPos;
in mat3 v_TBN;
in vec2 v_UV;

// PBR Textures
uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_MetallicTexture;
uniform sampler2D u_RoughnessTexture;
uniform sampler2D u_SpecularTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_OpacityTexture;
uniform sampler2D u_EmissiveTexture;
uniform sampler2D u_AOTexture;

// Lights
struct PointLight
{
	vec3 Position;
	vec3 Color;
	float Intensity;
};
const int MAX_NUM_POINT_LIGHTS = 4;
uniform PointLight u_PointLights[MAX_NUM_POINT_LIGHTS];

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

// F0 is the surface reflection at zero incidence
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FakeSSS( in vec3 SubColour, in PointLight light )
{
	vec3 PDiff = v_WorldPos - u_CameraPosition;
	vec3 LDiff = light.Position - u_CameraPosition;
	float PDiffLen = length( PDiff );
	float LDiffLen = length( LDiff );
	vec3 P = PDiff / PDiffLen;
	vec3 L = LDiff / LDiffLen;
	vec3 Colour = light.Color * light.Intensity * SubColour;

	vec3 Normal = normalize( v_TBN[2] );
	float Fresnel = pow( 1.0 - dot( -P, Normal ), 0.5 );

			float distance = length(light.Position - v_WorldPos);
		float attenuation = 1.0 / ( distance * distance);

	float Behind = ( LDiffLen > PDiffLen ? 1.0 : 0.0 ) * pow( max( dot( P, L ), 0.0 ), 80.0 ) * Fresnel;
	float Nearby = attenuation * 0.1;

	return max( Behind * 0.3, Nearby ) * Colour;
}

void main()
{
	vec3 albedo = texture(u_AlbedoTexture, v_UV).rgb;
	vec3 normal = texture(u_NormalTexture, v_UV).rgb;
	// NOTE: AO, Roughness, and Metallic could all use the same texture as we only need one channel for each. 
	// r = ambient occlusion
	// g = roughness
	// b = metallic
	float ao = texture(u_AOTexture, v_UV).r;
	float roughness = texture(u_RoughnessTexture, v_UV).g;
	float metallic = texture(u_MetallicTexture, v_UV).b;
	vec3 emissive = texture(u_EmissiveTexture, v_UV).rgb;

	normal = normal * 2.0 - 1.0;
	normal = normalize(v_TBN * normal);
	vec3 V = normalize(u_CameraPosition - v_WorldPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// Calculate Light Output
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 4; ++i)
	{
		PointLight pointLight = u_PointLights[i];
		// Calculate per-light radiance
		vec3 L = normalize(pointLight.Position - v_WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(pointLight.Position - v_WorldPos);
		float attenuation = 1.0 / ( distance * distance);
		vec3 radiance = pointLight.Color * attenuation * pointLight.Intensity;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(normal, H, roughness);
		float G = GeometrySmith(normal, V, L, roughness);
		vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		// add to Lo
		float NdotL = max(dot(normal, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;

		//Lo += albedo * FakeSSS( vec3( 0.2, 0.8, 0.4 ), pointLight );
	}

	vec3 ambient = vec3(0.1) * albedo * ao;
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));
	color += emissive;

	o_Color = vec4(color, 1.0);
}
