#version 420 core

out vec4 FinalColor;
in vec2 TexCoords;

uniform sampler2D PostProcessBuffer;
uniform float uExposure;

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

   // Simple Reinhard tonemapping
   //texColor *= 2.0f; // Hardcoded exposure
   //texColor = texColor / (1.0f + texColor);

   texColor *= uExposure;
   float ExposureBias = 2.0f;
   vec3 curr = Uncharted2Tonemap(ExposureBias * texColor);

   vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(W));
   vec3 color = curr*whiteScale;

   // Gamma correction
   texColor = pow(texColor, vec3(1.0f / 2.2f));
   FinalColor = vec4(texColor, 1.0f);
}