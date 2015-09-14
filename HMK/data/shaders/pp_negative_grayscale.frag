#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;
uniform bool uIsGrayScale;

void main()
{
	vec3 color;
	if(uIsGrayScale)
	{
		color = texture(PostProcessBuffer, TexCoords).rgb;
		float avg = dot(color, vec3(0.2126f, 0.7152f, 0.0722));
		color = vec3(avg);
	}
	else
	{
		color = 1.0f - texture(PostProcessBuffer, TexCoords).rgb;
	}
	FinalColor = vec4(color, 1.0f);
}