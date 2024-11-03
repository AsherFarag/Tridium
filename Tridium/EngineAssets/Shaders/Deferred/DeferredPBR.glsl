#pragma type Vertex
#version 420 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;

out vec2 v_UV;

void main()
{
	v_UV = a_UV;
	gl_Position = vec4(a_Position, 1.0);
}

#pragma type Fragment
#version 420 core
layout(location = 0) out vec4 o_Color;
in vec2 v_UV;

const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 4.0;
const vec3 GRAY_SCALE = vec3(0.299, 0.587, 0.114);
const float EPSILON = 0.0001;

uniform vec3 u_CameraPosition;

// GBuffer Textures
uniform sampler2D g_Position;
uniform sampler2D g_Normal;
uniform sampler2D g_Albedo;
uniform sampler2D g_AORM;
uniform sampler2D g_Emission;

struct Environment
{
	samplerCube PrefilterMap;
	sampler2D BrdfLUT;
	float Roughness;
	float Exposure;
	float Gamma;
    float Intensity;
};

uniform Environment u_Environment;

// Lights
struct PointLight
{
	vec3 Position;
	vec3 Color;
	float Intensity;
	float AttenuationRadius;
	float FalloffExponent;
};
const int MAX_NUM_POINT_LIGHTS = 32;
uniform PointLight u_PointLights[MAX_NUM_POINT_LIGHTS];

struct SpotLight
{
	vec3 Position;
	vec3 Direction;
	vec3 Color;
	float Intensity;
	float FalloffExponent;
	float AttenuationRadius;
	float InnerConeAngle;
	float OuterConeAngle;
};
const int MAX_NUM_SPOT_LIGHTS = 32;
uniform SpotLight u_SpotLights[MAX_NUM_SPOT_LIGHTS];

struct DirectionalLight
{
	vec3 Direction;
	vec3 Color;
	float Intensity;
};
const int MAX_NUM_DIRECTIONAL_LIGHTS = 1;
uniform DirectionalLight u_DirectionalLights[MAX_NUM_DIRECTIONAL_LIGHTS];

// Shadows
uniform sampler2D u_PointShadowMaps[MAX_NUM_POINT_LIGHTS];
uniform sampler2D u_SpotShadowMaps[MAX_NUM_SPOT_LIGHTS];
uniform sampler2D u_DirectionalShadowMaps[MAX_NUM_DIRECTIONAL_LIGHTS];

// ====================================================

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
// F0 is the surface reflection at zero incidence
vec3 FresnelSchlick(float cosTheta, vec3 F0);
// F0 is the surface reflection at zero incidence
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float Unreal_LightAttentuation(float distance, float falloffExponent, float attenuationRadius);

vec3 CalcDirectionalLightRadiance(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo, float roughness, float metallic, vec3 F0);
vec3 CalcPointLightRadiance(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float roughness, float metallic, vec3 F0);
vec3 CalcSpotLightRadiance(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float roughness, float metallic, vec3 F0);

vec3 ExposureToneMapping(vec3 color, float exposure) 
{
    return vec3(1.0) - exp(-color * exposure);
}

vec3 GammaCorrection(vec3 color, float gamma) 
{
    return pow(color, vec3(1.0 / gamma));
}

// TEMP ?
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, sampler2D shadowMap);

uniform mat4 u_LightSpaceMatrix;

void main()
{
	vec3 worldPos = texture(g_Position, v_UV).rgb;
	vec3 albedo = texture(g_Albedo, v_UV).rgb;
	// NOTE: AO, Roughness, and Metallic could all use the same texture as we only need one channel for each. 
	// r = ambient occlusion
	// g = roughness
	// b = metallic
	float ao = texture(g_AORM, v_UV).r;
	float roughness = texture(g_AORM, v_UV).g;
	float metallic = texture(g_AORM, v_UV).b;
	vec3 emissive = texture(g_Emission, v_UV).rgb;
	vec3 normal = texture(g_Normal, v_UV).rgb;

	vec3 V = normalize(u_CameraPosition - worldPos);
	vec3 R = reflect(-V, normal);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)  
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// Temp?
	vec4 fragPosLightSpace = u_LightSpaceMatrix * vec4( worldPos, 1.0 );

	// Calculate Light Output
	vec3 Lo = vec3(0.0);
	// Directional Lights
	for (int i = 0; i < MAX_NUM_DIRECTIONAL_LIGHTS; ++i)
	{
	    DirectionalLight dirLight = u_DirectionalLights[i];
	    vec3 light = CalcDirectionalLightRadiance(dirLight, normal, V, albedo, roughness, metallic, F0);
        Lo += light * ( 1.0 - ShadowCalculation(fragPosLightSpace, normal, u_DirectionalShadowMaps[i]) );
	}
	// Point Lights
	for (int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i)
	{
		PointLight pointLight = u_PointLights[i];
		Lo += CalcPointLightRadiance(pointLight, normal, worldPos, V, albedo, roughness, metallic, F0);
	}
	// Spot Lights
	for (int i = 0; i < MAX_NUM_SPOT_LIGHTS; ++i)
	{
	    SpotLight spotLight = u_SpotLights[i];
	    Lo += CalcSpotLightRadiance(spotLight, normal, worldPos, V, albedo, roughness, metallic, F0);
	}

	// =======================================================================

	vec3 F = FresnelSchlickRoughness(max(dot(normal, V), 0.0), F0, roughness);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 irradiance = textureLod(u_Environment.PrefilterMap, normal, MAX_REFLECTION_LOD).rgb;
    irradiance *= u_Environment.Intensity;
	vec3 diffuse = irradiance * albedo;

	float prefilteredRoughness = min( ( ( u_Environment.Roughness + roughness ) * MAX_REFLECTION_LOD ), MAX_REFLECTION_LOD );
	vec3 prefilteredColor = textureLod(u_Environment.PrefilterMap, R, prefilteredRoughness).rgb;
    prefilteredColor *= u_Environment.Intensity;

	vec2 envBRDF = texture(u_Environment.BrdfLUT, vec2(max(dot(normal, -V), 0.0)), u_Environment.Roughness + roughness).rg;
	vec3 specular = prefilteredColor;
	// Calculate Fresnel using Schlick's approximation
	float fresnel = pow(clamp(1.0 - max(dot(normal, V), 0.0), 0.0, 1.0), 5.0); // Use ^5 for more accurate Fresnel

	// Apply fresnel factor to the specular term, ensuring roughness impact
	//specular *= max( (1.0 - roughness), EPSILON );  // Reduce specular based on roughness
	specular *= 0.65 + fresnel * 0.35;  // Add a slight minimum base reflectivity (0.1) and clamp at 1.0
	//specular *= mix(vec3(1.0), albedo / max( dot(GRAY_SCALE, albedo), metallic), 0.01 );

	vec3 ambient = (kD * diffuse + specular ) * max(ao, 0.05); // Minimum AO is 0.05
	vec3 color = ambient + Lo;

	color += emissive * 5.0;

	color = ExposureToneMapping(color, u_Environment.Exposure);
	color = GammaCorrection(color, u_Environment.Gamma);

	o_Color = vec4( color, 1.0);
}

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
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// F0 is the surface reflection at zero incidence
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

float Unreal_LightAttentuation(float distance, float falloffExponent, float attenuationRadius)
{
	float attenuation = 1.0 / (distance * distance + 1e-6f);

	float falloff = max(0.0, (attenuationRadius - distance) / attenuationRadius);
	attenuation *= pow(falloff, falloffExponent);
	attenuation = clamp(attenuation, 0.0, 1.0);
	return attenuation;
}

vec3 CalcDirectionalLightRadiance(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo, float roughness, float metallic, vec3 F0)
{
    vec3 L = normalize(-light.Direction);  // light direction (normalized)
    vec3 H = normalize(viewDir + L);       // half-vector between L and V

    // Compute per-light radiance
    vec3 radiance = light.Color * light.Intensity;

    // Cook-Torrance BRDF terms
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 fresnel = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    // Specular and diffuse contributions
    vec3 kS = fresnel;
    vec3 kD = vec3(1.0) - kS;  // Only non-metallic surfaces have diffuse reflection
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * fresnel;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + EPSILON;
    vec3 specular = numerator / denominator;

    float NdotL = max(dot(normal, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalcPointLightRadiance(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float roughness, float metallic, vec3 F0)
{
	// Calculate per-light radiance
	vec3 L = normalize(light.Position - fragPos);
	vec3 H = normalize(viewDir + L);
	float distance = length(light.Position - fragPos);
	float attenuation = Unreal_LightAttentuation(distance, light.FalloffExponent, light.AttenuationRadius);
	vec3 radiance = light.Color * attenuation * light.Intensity;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, H, roughness);
	float G = GeometrySmith(normal, viewDir, L, roughness);
	vec3 fresnel = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

	vec3 kS = fresnel;
	// for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
	kD *= 1.0 - metallic;

	vec3 numerator = NDF * G * fresnel;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + EPSILON;
	vec3 specular = numerator / denominator;

	float NdotL = max(dot(normal, L), 0.0);
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalcSpotLightRadiance(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float roughness, float metallic, vec3 F0)
{
    vec3 L = normalize(light.Position - fragPos);  // Direction to light
    vec3 H = normalize(viewDir + L);               // Half-vector

    // Spotlight angle attenuation based on cone angles
    float theta = dot(-L, normalize(-light.Direction)); // Cosine of the angle between light direction and fragment direction
	float epsilon = light.InnerConeAngle - light.OuterConeAngle;
	float intensityFactor = clamp((theta - light.OuterConeAngle) / epsilon, 0.0, 1.0);
    // Ensure that the smoothstep arguments are in the correct order for a smooth transition
	//float intensityFactor = 1.0 - smoothstep(cos(light.OuterConeAngle), cos(light.InnerConeAngle), theta);

    // Distance attenuation (similar to point light attenuation)
    float distance = length(light.Position - fragPos);
    float attenuation = Unreal_LightAttentuation(distance, light.FalloffExponent, light.AttenuationRadius);

    // Combine spotlight intensity with distance attenuation
    vec3 radiance = light.Color * light.Intensity * attenuation * intensityFactor;

    // Cook-Torrance BRDF terms
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 fresnel = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    // Specular and diffuse contributions
    vec3 kS = fresnel;
    vec3 kD = vec3(1.0) - kS;  // Only non-metallic surfaces have diffuse reflection
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * fresnel;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + EPSILON;
    vec3 specular = numerator / denominator;

    float NdotL = max(dot(normal, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

// TEMP ?
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, sampler2D shadowMap)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	if(projCoords.z > 1.0)
        return 0.0;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    float shadow = 0.0;

    const int sampleRadius = 2;
    const vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
    for (int y = -sampleRadius; y <= sampleRadius; y++)
    {
        for (int x = -sampleRadius; x <= sampleRadius; x++)
        {
            // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            float closestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * pixelSize).r;
            if (currentDepth > closestDepth)
                shadow += 1.0;
        }
    }

    shadow /= pow((sampleRadius * 2 + 1), 2);

	return shadow;
}