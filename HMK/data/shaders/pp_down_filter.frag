#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;
uniform int uFilterLevel;

void main()
{
	vec2 offset = vec2(1.0f / 800.0f, 1.0f / 600.0f);
	vec3 color = vec3(0.0f);
	color += texture(PostProcessBuffer, TexCoords + offset).rgb;
	color += texture(PostProcessBuffer, TexCoords + vec2(offset.x, -offset.y)).rgb;
	color += texture(PostProcessBuffer, TexCoords + vec2(-offset.x, offset.y)).rgb;
	color += texture(PostProcessBuffer, TexCoords - offset).rgb;

	color /= 4.0f;

	FinalColor = vec4(color, 1.0f);
}