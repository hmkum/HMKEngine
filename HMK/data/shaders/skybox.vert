#version 330 core

layout(location = 0) in vec3 inPosition;

out vec3 TexCoords;

uniform mat4 uModel;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

void main()
{
	TexCoords = (uModel * vec4(inPosition, 1.0f)).xyz;
	gl_Position = uProjMatrix * uViewMatrix * uModel * vec4(inPosition, 1.0f);
}