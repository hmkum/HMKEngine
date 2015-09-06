#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;

void main()
{
   vec3 texColor = texture(PostProcessBuffer, TexCoords).rgb;
   // Just gamma correction
   texColor = pow(texColor, vec3(1.0f / 2.2f));
   FinalColor = vec4(texColor, 1.0f);
}