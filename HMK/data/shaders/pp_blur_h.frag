#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;

const float Weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216,
                                0.0540540541, 0.0162162162);

void main()
{
	ivec2 pix = ivec2(gl_FragCoord.xy);
    vec4 color = texelFetch(PostProcessBuffer, pix, 0) * Weight[0];
    color += texelFetchOffset(PostProcessBuffer, pix, 0, ivec2( 1, 0)) * Weight[1];
    color += texelFetchOffset(PostProcessBuffer, pix, 0, ivec2(-1, 0)) * Weight[1];
    color += texelFetchOffset(PostProcessBuffer, pix, 0, ivec2( 2, 0)) * Weight[2];
    color += texelFetchOffset(PostProcessBuffer, pix, 0, ivec2(-2, 0)) * Weight[2];
    color += texelFetchOffset(PostProcessBuffer, pix, 0, ivec2( 3, 0)) * Weight[3];
    color += texelFetchOffset(PostProcessBuffer, pix, 0, ivec2(-3, 0)) * Weight[3];
    color += texelFetchOffset(PostProcessBuffer, pix, 0, ivec2( 4, 0)) * Weight[4];
    color += texelFetchOffset(PostProcessBuffer, pix, 0, ivec2(-4, 0)) * Weight[4];
	FinalColor = color;
}