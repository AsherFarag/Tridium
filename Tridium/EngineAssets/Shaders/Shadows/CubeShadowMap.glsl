#pragma type Vertex
#version 420 core
layout(location = 0) in vec3 a_Position;

uniform mat4 u_Model;

void main()
{
    gl_Position = u_Model * vec4(a_Position, 1.0);
}

#pragma type Geometry
#version 420
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 u_LightSpaceMatrices[6];

out vec4 o_FragPos; // FragPos from Geometry Shader (output per emitvertex)

void main()
{
    for ( int face = 0; face < 6; ++face )
    {
        gl_Layer = face;
        for ( int i = 0; i < 3; ++i )
        {
            o_FragPos = gl_in[i].gl_Position;
            gl_Position = u_LightSpaceMatrices[face] * o_FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

#pragma type Fragment
#version 420 core
in vec4 o_FragPos;

uniform vec3 u_LightPosition;
uniform float u_FarPlane;

void main()
{
    // Get distance between fragment and light source
    float lightDistance = length(o_FragPos.xyz - u_LightPosition);
    
    // Map to [0;1] range by dividing by u_FarPlane
    lightDistance = lightDistance / u_FarPlane;
    
    // Write this as modified depth
    gl_FragDepth = lightDistance;
}
