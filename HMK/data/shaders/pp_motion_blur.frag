#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform mat4 uInvViewProj;
uniform mat4 uPrevViewProj;

layout(binding = 0) uniform sampler2D PostProcessBuffer;
layout(binding = 1) uniform sampler2D DepthBuffer;

void main()
{
    vec3 color = texture(PostProcessBuffer, TexCoords).rgb;
    float zOverW = texture(DepthBuffer, TexCoords).r;
	vec4 H = vec4(TexCoords.x * 2.0f - 1.0f, TexCoords.y * 2.0f - 1.0f, zOverW, 1.0f);
	vec4 D = uInvViewProj * H;
	vec4 worldPos = D / D.w;

	vec4 currentPos = H;
	vec4 prevPos = uPrevViewProj * worldPos;
	prevPos /= prevPos.w;
	
	
	vec2 velocity = ((currentPos - prevPos) / 30.0f).xy;
	vec2 texCoord = TexCoords + velocity;
	
	for(int i = 1; i < 13; ++i, texCoord += velocity)
	{
		vec4 currColor = texture(PostProcessBuffer, texCoord);
		color += currColor.rgb;
	}

	FinalColor = vec4(color / 13.0f, 1.0f);
}