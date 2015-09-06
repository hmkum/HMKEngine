#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;

void main()
{
    vec3 color = texture(PostProcessBuffer, TexCoords).rgb;
    float mono = dot(vec3(0.2125f, 0.7154f, 0.0721f), color);
	FinalColor = vec4(mono, mono, mono, 1.0f);
}