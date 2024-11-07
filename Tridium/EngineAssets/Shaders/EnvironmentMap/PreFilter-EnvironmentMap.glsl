#pragma type Vertex
#version 420 core
layout (location = 0) in vec3 a_Pos;

out vec3 v_WorldPos;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    v_WorldPos = a_Pos;  
    gl_Position =  u_Projection * u_View * vec4(v_WorldPos, 1.0);
}

#pragma type Fragment
#version 420 core
out vec4 o_Color;
in vec3 v_WorldPos;

uniform samplerCube u_EnvironmentMap;
uniform float u_SourceResolution;
uniform float u_Roughness;
const float u_Exposure = 1.0;  // Exposure control
const float u_Gamma = 2.2;     // Optional gamma correction

const float PI = 3.14159265359;
const uint NUM_SAMPLES = 2048u * 8u;  // Number of samples for Monte Carlo integration
const float EPSILON = 0.0001;

float DistributionGGX(vec3 normal, vec3 halfway, float roughness);
float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float CalculateMipLevel(vec3 normal, vec3 view, vec3 halfway, float roughness);

void main()
{
    vec3 normal = normalize(v_WorldPos);    
    vec3 reflection = normal;
    vec3 view = reflection;

    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);     
    for(uint i = 0u; i < NUM_SAMPLES; ++i)
    {
        // A uniformly distributed point in the sample domain
        vec2 Xi = Hammersley(i, NUM_SAMPLES);
        // The halfway vector ( between V and L ), that is biased towards the direction of the surface normal
        vec3 H  = ImportanceSampleGGX(Xi, normal, u_Roughness);
        // The light direction
        vec3 L  = normalize(2.0 * dot(view, H) * H - view);

        float NdotL = max(dot(normal, L), 0.0);
        if(NdotL > 0.0)
        {
            const float mipLevel = CalculateMipLevel(normal, view, H, u_Roughness);
            vec3 radiance = clamp( textureLod(u_EnvironmentMap, L, mipLevel).rgb, vec3(0.0), vec3(100.0) * (1.1 - u_Roughness) );
            radiance = clamp( textureLod(u_EnvironmentMap, L, mipLevel).rgb, vec3(0.0), vec3(100.0) * (1.1 - u_Roughness) );
            prefilteredColor +=  radiance * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    o_Color = vec4(prefilteredColor, 1.0);
}

float DistributionGGX(vec3 normal, vec3 halfway, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(normal, halfway), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float CalculateMipLevel(vec3 normal, vec3 view, vec3 halfway, float roughness)
{
    if (roughness == 0.0)
        return 0.0;

    float D = DistributionGGX(normal, halfway, roughness);
    float NdotH = max(dot(normal, halfway), 0.0);
    float HdotV = max(dot(halfway, view), 0.0);
    float pdf = D * NdotH / (4.0 * HdotV) + EPSILON;

    float saTexel = 4.0 * PI / (6.0 * u_SourceResolution * u_SourceResolution);
    float saSample = 1.0 / (float(NUM_SAMPLES) * pdf + EPSILON);
    return 0.5 * log2(saSample / saTexel);
}