#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord0;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

out vec3 Position;
out vec2 TexCoord0;
out vec3 Normal;
out vec3 Tangent;

uniform mat4 uModel;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

void main()
{
	Position = (uModel * vec4(inPosition, 1.0f)).xyz;
	TexCoord0 = inTexCoord0;
	Normal = mat3(transpose(inverse(uModel))) * inNormal;
	Tangent = (uModel * vec4(inTangent, 1.0f)).xyz;
	gl_Position = uProjMatrix * uViewMatrix * uModel * vec4(inPosition, 1.0f);
}