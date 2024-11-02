#type Vertex
#version 420 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_UV;

uniform mat4 u_PVM;
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

out vec3 v_WorldPos;
out vec3 v_Normal; 
out vec2 v_UV;
out vec4 v_FragPosLightSpace;

void main()
{
    v_Normal = transpose(inverse(mat3(u_Model))) * a_Normal;
	v_WorldPos = vec3( u_Model * vec4(a_Position, 1.0) );
	v_UV = a_UV;

	v_FragPosLightSpace = u_LightSpaceMatrix * vec4( v_WorldPos, 1.0 );

	gl_Position = u_PVM * vec4(a_Position, 1.0);
}

#type Fragment
#version 420 core
layout(location = 0) out vec4 o_Color;

in vec3 v_WorldPos;
in vec3 v_Normal; 
in vec2 v_UV;
in vec4 v_FragPosLightSpace;

uniform sampler2D u_ShadowMap;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal);

void main()
{
	vec3 color = vec3(0.25,1.0,0.75);

	float shadow = ShadowCalculation(v_FragPosLightSpace, v_Normal);
	color *= 1.0 - shadow;

	o_Color = vec4( color, 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	if(projCoords.z > 1.0)
        return 0.0;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    vec3 lightDir = normalize(u_LightPos - v_WorldPos);
	float bias = max(0.025 * (1.0 - dot(normal, lightDir)), 0.0005);  
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth + bias ? 1.0 : 0.0;
	return shadow;
}