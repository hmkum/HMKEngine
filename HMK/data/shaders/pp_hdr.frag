#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

layout(binding = 0) uniform sampler2D PostProcessBuffer;
layout(binding = 1) uniform sampler2D BloomBuffer;

uniform float uExposure;
uniform bool uIsBloomActive;
uniform float uBloomIntensity;

const float A = 0.22;
const float B = 0.30;
const float C = 0.10;
const float D = 0.20;
const float E = 0.01;
const float F = 0.30;
const float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{
   vec3 texColor = texture(PostProcessBuffer, TexCoords).rgb;

   texColor *= uExposure;
   float ExposureBias = 2.0f;
   vec3 curr = Uncharted2Tonemap(ExposureBias * texColor);

   vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(W));
   vec3 color = curr*whiteScale;

   if(uIsBloomActive)
   {
		vec3 bloomColor = texture(BloomBuffer, TexCoords).rgb;
		color = color + uBloomIntensity * bloomColor;
   }

   // Gamma correction
   color = pow(color, vec3(1.0f / 2.2f));
   FinalColor = vec4(color, 1.0f);
}