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
const float u_Exposure = 1.0;  // Exposure control
const float u_Gamma = 2.2;     // Optional gamma correction

const float PI = 3.14159265359;
const int NUM_SAMPLES = 2048 * 2;  // Number of samples for Monte Carlo integration

// Function to compute cosine-weighted direction sampling
vec3 ImportanceSampleCosineHemisphere(float Xi1, float Xi2, vec3 N);

void main()
{
    vec3 N = -normalize(v_WorldPos);  // Surface normal
    vec3 irradiance = vec3(0.0);

    int nrSamples = 0;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        // Generate two random numbers for sampling
        vec2 Xi = vec2(float(i) / NUM_SAMPLES, fract(sin(float(i)) * 43758.5453123));
        
        // Importance sample a direction from the hemisphere (cosine-weighted)
        vec3 sampleVec = ImportanceSampleCosineHemisphere(Xi.x, Xi.y, N);
        
        // Sample the environment map with the generated sample direction
        vec3 radiance = texture(u_EnvironmentMap, sampleVec).rgb;
        radiance = clamp(radiance, vec3(0.0), vec3(1.0) );

        // Cosine term (the dot product between normal and sample direction)
        float NdotL = max(dot(N, sampleVec), 0.0);

        // Accumulate irradiance, divided by the PDF (which is cos(theta) / PI for cosine-weighted)
        irradiance += radiance * NdotL;

        nrSamples++;
    }

    irradiance = (PI * irradiance) / float(nrSamples);  // Average and scale by PI

    // Apply exposure control
    vec3 color = vec3(1.0) - exp(-irradiance * u_Exposure);  // Exposure tone mapping

    // Optional gamma correction
    color = pow(color, vec3(1.0 / u_Gamma));

    o_Color = vec4(irradiance, 1.0);  // Output the final color
}


vec3 ImportanceSampleCosineHemisphere(float Xi1, float Xi2, vec3 N)
{
    // Cosine-weighted spherical sampling
    float theta = acos(sqrt(1.0 - Xi1));  // Sample theta with cosine weighting
    float phi = 2.0 * PI * Xi2;           // Uniform sampling for phi

    // Convert spherical coordinates to cartesian coordinates (tangent space)
    vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

    // Convert tangent space to world space
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    return tangentSample.x * tangent + tangentSample.y * bitangent + tangentSample.z * N;
}