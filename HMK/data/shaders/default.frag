#version 420 core
//#extension GL_ARB_shading_language_420pack: enable

in vec3 Position;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 Tangent;
out vec4 FinalColor;

uniform vec3 uCameraPosition;

layout(binding = 0) uniform sampler2D AlbedoTexture;
layout(binding = 1) uniform sampler2D NormalTexture;
layout(binding = 2) uniform sampler2D RoughnessTexture;
layout(binding = 3) uniform sampler2D MetalnessTexture;
layout(binding = 4) uniform samplerCube EnvMap;

#define PI 3.1415926f
#define EPSILON 10e-5f
#define saturate(value) clamp(value, 0.0f, 1.0f);

// Disney diffuse model
vec4 Diffuse(vec4 albedoColor)
{
	return albedoColor / PI;
}

float D_GGX(float a, float NdotH)
{
	float a2 = a * a;
	float NdotH2 = NdotH * NdotH;

	float d = NdotH2 * (a2 - 1.0f) + 1.0f;
	return a2 / (d * d * PI);
}

float G_SmithSchlickGGX(float a, float NdotV, float NdotL)
{
	float k = a * 0.5f;
	float v = NdotV / (NdotV * (1 - k) + k);
	float l = NdotL / (NdotL * (1 - k) + k);

	return v * l;
}

vec3 F_Schlick(vec3 specularColor, float VdotH)
{
	return(specularColor + (vec3(1.0f) - specularColor) * pow(1.0f - VdotH, 5.0f));
}

vec3 Specular_F_Roughness(vec3 specularColor, float a, float VdotH)
{
    // Sclick using roughness to attenuate fresnel.
    return (specularColor + (max(vec3(1.0f - a), specularColor) - specularColor) * pow((1.0f - VdotH), 5.0f));
}

vec3 Specular(vec3 specularColor, float NdotL, float NdotV, float NdotH, float VdotH, float a)
{
	float D = D_GGX(a, NdotH);
	float G = G_SmithSchlickGGX(a, NdotV, NdotL);
	vec3 F  = F_Schlick(specularColor, VdotH);

	return (D * G * F) / (4.0f * NdotL * NdotV + EPSILON);
}

void main()
{
	vec3 normal = normalize(Normal);
	/*
	vec3 normal = texture(NormalTexture, TexCoord0).rgb;
	normal = 2.0f * normal - 1.0f;
	normal = normalize(normal);
	*/

	vec3 LightDirection = normalize((vec3(0, 0, 4) - Position));
	vec3 ViewDirection = normalize((uCameraPosition - Position));
	vec3 HalfVector = normalize(ViewDirection + LightDirection);	

	float LdotN = saturate(dot(LightDirection, normal));
	float VdotN = saturate(dot(ViewDirection, normal));
	float NdotH = saturate(dot(normal, HalfVector));
	float VdotH = saturate(dot(ViewDirection, HalfVector));
	
	float Roughness = texture(RoughnessTexture, TexCoord0).r;
	float Metalness = texture(MetalnessTexture, TexCoord0).r;
	float Alpha = max(0.001f, Roughness * Roughness);

	vec3 Reflected = reflect(-ViewDirection, normal);
	vec4 ReflectedColor = pow(texture(EnvMap, Reflected), vec4(2.2f));

	vec4 LightColor = vec4(1.0f, 0.9762f, 0.86f, 1.0f);

	// Gamma corrected albedo color
	vec4 AlbedoColor = pow(texture(AlbedoTexture, TexCoord0), vec4(2.2f));
	vec4 BaseColor = Diffuse(AlbedoColor);
	BaseColor = BaseColor - BaseColor * Metalness;
	vec3 SpecularColor = mix(vec3(0.04f), AlbedoColor.rgb, Metalness);

	vec3 Spec = Specular(SpecularColor, LdotN, VdotN, NdotH, VdotH, Alpha);
	vec3 envFresnel = Specular_F_Roughness(SpecularColor, Alpha, VdotH);


	vec4 Result = vec4(0.0f);
	Result += LightColor * LdotN * (BaseColor * (1.0f - vec4(Spec, 1.0f)) + vec4(Spec, 1.0f));
	Result += ReflectedColor * vec4(envFresnel, 1.0f) * 0.5f;

	FinalColor = Result;
	//FinalColor = pow(Result, vec4(1.0f / 2.2f));
}


/*
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
*/