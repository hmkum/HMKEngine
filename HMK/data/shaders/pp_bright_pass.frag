#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;

const float gMiddleGray = 0.5f;
const float gWhiteCutoff = 0.8f;

float calcLuminance(vec3 color)
{
	return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
}

void main()
{
	vec3 color = texture(PostProcessBuffer, TexCoords).rgb;
	float Luminance = log(calcLuminance(color));

	color *= gMiddleGray / (Luminance + 0.001f);
	color *= (1.0f + (color / (gWhiteCutoff * gWhiteCutoff)));
	color -= 0.5f;

	color = max(color, 0.0f);

	color /= (1.0f + color);
	FinalColor = vec4(color, 1.0f);
}