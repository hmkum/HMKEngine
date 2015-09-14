#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;

void main()
{
    vec3 color = texture(PostProcessBuffer, TexCoords).rgb;
	FinalColor = vec4(1.0f - color, 1.0f);
}