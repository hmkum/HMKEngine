#version 420 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord0;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(inPosition, 1.0f);
    TexCoords = inTexCoord0;
}