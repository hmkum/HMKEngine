#version 420 core
layout (location = 0) in vec3 inPosition;

uniform mat4 uLightSpaceMatrix;
uniform mat4 uModel;

void main()
{
    gl_Position = uLightSpaceMatrix * uModel * vec4(inPosition, 1.0f);
}  