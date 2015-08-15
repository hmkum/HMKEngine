#version 420 core
//#extension GL_ARB_shading_language_420pack: enable

in vec3 Position;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 Tangent;
out vec4 FinalColor;

uniform vec3 uCameraPosition;

layout(binding = 0) uniform sampler2D AlbedoTexture;
layout(binding = 1) uniform sampler2D RoughnessTexture;
layout(binding = 2) uniform sampler2D MetalnessTexture;
layout(binding = 4) uniform samplerCube EnvMap;

#define PI 3.1415926f
#define EPSILON 10e-5f
#define saturate(value) clamp(value, 0.0f, 1.0f);

vec4 BRDF_DisneyDiffuse(vec4 baseColor)
{
	return baseColor / PI;
}

// Got these BRDF formulas Moving Frostbite to PBR slide by Sebastien Lagarde & Charles de Rousiers 
// http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr_v2.pdf
float BRDF_D_GGX(float NdotH, float Roughness)
{
	float alpha2 = pow(Roughness, 4);
	float d = NdotH * NdotH * (alpha2 - 1) + 1;
	return alpha2 / (PI * d * d);
}


float BRDF_F_FresnelSchlick(float LdotH, float F0)
{
	return F0 + (1.0f - F0) * pow((1.0f - LdotH), 5);
}

vec3 BRDF_F_FresnelSchlick(float LdotH, vec3 F0)
{
	return F0 + (vec3(1.0f) - F0) * pow((1.0f - LdotH), 5);
}

float BRDF_G_Unreal(float NdotL, float NdotV, float Roughness)
{
	float r = (Roughness + 1) / 2;
	float k = pow(r + 1, 2) / 8.0f;

	float gv = NdotV / (NdotV * (1 - k) + k);
	float gl = NdotL / (NdotL * (1 - k) + k);
	return gv * gl;
}

float BRDF_G_SmithGGXCorrelated(float NdotL, float NdotV, float Roughness)
{
	float Roughness2 = Roughness * Roughness;
	float GV = NdotL * sqrt((-NdotV * Roughness2 + NdotV) * NdotV + Roughness2);
	float GL = NdotV * sqrt((-NdotL * Roughness2 + NdotL) * NdotL + Roughness2);

	return 0.5f / (GV + GL + EPSILON);
}

float BRDF_Specular(float NdotV, float NdotL, float NdotH, float LdotH, float Roughness, float F0)
{
	float D = BRDF_D_GGX(NdotH, Roughness);
	float F = BRDF_F_FresnelSchlick(LdotH, F0);
	float G = BRDF_G_Unreal(NdotL, NdotV, Roughness); //BRDF_G_SmithGGXCorrelated(NdotL, NdotV, Roughness);
	return (D * F * G) / 4 * NdotL * NdotV;
}

void main()
{
	vec3 normal = normalize(Normal);

	vec3 LightDirection = normalize(vec3(0, 0, 4) - Position);
	vec3 ViewDirection = normalize(uCameraPosition - Position);
	vec3 HalfVector = normalize(ViewDirection + LightDirection);	

	float NdotL = saturate(dot(LightDirection, normal));
	float NdotV = saturate(dot(ViewDirection, normal));
	float LdotH = saturate(dot(LightDirection, HalfVector));
	float NdotH = saturate(dot(normal, HalfVector));
	float VdotH = saturate(dot(ViewDirection, HalfVector));
	
	float Roughness = texture(RoughnessTexture, TexCoord0).r;
	float Metalness = 0.0f;//texture(MetalnessTexture, TexCoord0).r;

	float F0 = 0.04f;
	if(Metalness != 0.0f)
	{
		float RefractiveIndex = 1.5f; // RI for Gold materials. I got this from http://refractiveindex.info/
		F0 = pow(((1.0f - RefractiveIndex) / (1.0f + RefractiveIndex)), 2);
	}

	vec3 Reflected = reflect(-ViewDirection, normal);
	vec4 ReflectedColor = texture(EnvMap, Reflected);

	vec4 BaseColor = pow(texture(AlbedoTexture, TexCoord0), vec4(2.2f));
	vec4 SpecularColor = vec4(1.0f);

	BaseColor = BRDF_DisneyDiffuse(BaseColor);
	vec4 AmbientColor = BaseColor * 0.1f;

	float DiffuseFactor = NdotL;
	float SpecularFactor = BRDF_Specular(NdotV, NdotL, NdotH, LdotH, Roughness, F0);
	FinalColor = AmbientColor + BaseColor * DiffuseFactor + ReflectedColor * SpecularFactor;
	//FinalColor = pow(FinalColor, vec4(2.2f));
}
