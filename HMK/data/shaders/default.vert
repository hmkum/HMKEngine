#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord0;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

out vec3 Position;
out vec2 TexCoord0;
out vec3 Normal;
out vec3 Tangent;
out vec4 FragPosLightSpace;

uniform mat4 uModel;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uLightSpaceMatrix;
uniform vec3 uLightPosition;

void main()
{
	Position = (uModel * vec4(inPosition, 1.0f)).xyz;
	TexCoord0 = inTexCoord0;
	Normal = mat3(transpose(inverse(uModel))) * inNormal;
	Tangent = (uModel * vec4(inTangent, 1.0f)).xyz;
	// Saying "Goodbye" to Shadow Acne by Daniel Holbert, High Moon Studios
	// Normal offset yöntemi
	float bias = dot(Normal, normalize(uLightPosition));
	bias = clamp(1.0f - bias, 0.0f, 1.0f);
	FragPosLightSpace = uLightSpaceMatrix * vec4(Position + Normal * bias, 1.0f);
	gl_Position = uProjMatrix * uViewMatrix * uModel * vec4(inPosition, 1.0f);
}