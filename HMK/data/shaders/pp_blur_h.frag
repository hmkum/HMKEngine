#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;

const float gBlurOffsets[9] = {-4.0f, -3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
const float gBlurWeights[9] = {0.05f, 0.09f, 0.12f, 0.15f, 0.16f, 0.15f, 0.12f, 0.09f, 0.05f};

void main()
{
	vec3 color = vec3(0.0f);
	float blurSizeH = 1.0f / 600.0f;
	for(int i = 0; i < 9; ++i)
	{
		color += texture(PostProcessBuffer, vec2(TexCoords.x, TexCoords.y + gBlurOffsets[i] * blurSizeH)) * gBlurWeights[i];
	}

	FinalColor = vec4(color, 1.0f);
}