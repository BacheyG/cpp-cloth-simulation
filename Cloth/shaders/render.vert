#version 430

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 colorBuffer;

uniform mat4 viewproj;
out vec3 normalVector;
out vec3 fragPos;
void main()
{
	normalVector = colorBuffer.xyz;
	fragPos = (viewproj * vPosition).xyz;	
	gl_Position = viewproj * vPosition;	
}