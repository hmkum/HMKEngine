#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;

const vec2 offset = vec2(1.0f / 400.0f, 1.0f / 300.0f);
const vec2 offsets[9] = vec2[](
								vec2(-offset.x, offset.y),  // top-left
								vec2(0.0f,    offset.y),  // top-center
								vec2(offset.x,  offset.y),  // top-right
								vec2(-offset.x, 0.0f),    // center-left
								vec2(0.0f,    0.0f),    // center-center
								vec2(offset.x,  0.0f),    // center-right
								vec2(-offset.x, -offset.y), // bottom-left
								vec2(0.0f,    -offset.y), // bottom-center
								vec2(offset.x,  -offset.y)  // bottom-right    
								);

const float kernel[9] = float[](
								1.0 / 16, 2.0 / 16, 1.0 / 16,
								2.0 / 16, 4.0 / 16, 2.0 / 16,
								1.0 / 16, 2.0 / 16, 1.0 / 16  
								);

void main()
{
	vec3 color = vec3(0.0f);

	for(int i = 0; i < 9; ++i)
	{
		vec3 sampleTex = texture(PostProcessBuffer, TexCoords.st + offsets[i]).rgb;
		color += sampleTex * kernel[i];
	}

	FinalColor = vec4(color, 1.0f);
}