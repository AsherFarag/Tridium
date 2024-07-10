#type Vertex
#version 420

layout(location = 0) in vec3 aPosition;

uniform int uID;
uniform mat4 uPVM;		

flat out int vID;

void main()
{	
	gl_Position = uPVM * vec4(aPosition, 1);
	vID = uID;
}

#type Fragment
#version 420 core

layout(location = 0) out int oID;

flat in int vID;						

void main()
{
	oID = vID;
}