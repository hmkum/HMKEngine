#version 420 core
//#extension GL_ARB_shading_language_420pack: enable

in vec3 Position;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 Tangent;
in vec4 FragPosLightSpace;
out vec4 FinalColor;

struct Material
{
	vec4 BaseColor;
	float Roughness;
	float Metallic;
};

uniform vec3 uCameraPosition;
uniform vec3 uLightPosition;
uniform ivec4 uHasTextures;
uniform Material uMaterial;

layout(binding = 0) uniform sampler2D AlbedoTexture;
layout(binding = 1) uniform sampler2D NormalTexture;
layout(binding = 2) uniform sampler2D RoughnessTexture;
layout(binding = 3) uniform sampler2D MetalnessTexture;
layout(binding = 4) uniform samplerCube EnvMap;
layout(binding = 5) uniform sampler2D DepthTexture;

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

float CalculateShadow(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	projCoords = projCoords * 0.5f + 0.5f;

	float closestDepth = texture(DepthTexture, projCoords.xy).r;
	float currentDepth = projCoords.z;

	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(DepthTexture, 0);
	for(float x = -1.5f; x <= 1.5f; ++x)
	{
		for(float y = -1.5f; y <= 1.5f; ++y)
		{
			float pcfDepth = texture(DepthTexture, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += pcfDepth < currentDepth ? 0.0f : 1.0f;
		}
	}

	shadow /= 16.0f;

	if(projCoords.z > 1.0f)
		shadow = 1.0f;

	return shadow;
}

void main()
{
	const float Gamma = 2.2f;
	const float InvGamma = 1.0f / Gamma;

	vec3 normal = normalize(Normal);
	if(uHasTextures.y > 0)
	{
		normal = texture(NormalTexture, TexCoord0).rgb;
		normal = 2.0f * normal - 1.0f;
		normal = normalize(normal);
		vec3 T = normalize(Tangent - dot(Tangent, normal) * normal);
		vec3 B = cross(T, normal);
		mat3 TBN = mat3(T, B, normal);
		normal = normalize(TBN * normal);
	}
	
	vec4 LightColor = vec4(1.0f, 0.9762f, 0.86f, 1.0f);
	vec3 LightDirection = normalize(uLightPosition - Position);
	vec3 ViewDirection = normalize((uCameraPosition - Position));
	vec3 HalfVector = normalize(ViewDirection + LightDirection);	

	float LdotN = saturate(dot(LightDirection, normal));
	float VdotN = saturate(dot(ViewDirection, normal));
	float NdotH = saturate(dot(normal, HalfVector));
	float VdotH = saturate(dot(ViewDirection, HalfVector));
	
	float Roughness = saturate(uMaterial.Roughness);
	if(uHasTextures.z > 0)
	{
		Roughness = texture(RoughnessTexture, TexCoord0).r;
	}
	float Alpha = max(0.001f, Roughness * Roughness);

	float Metallic = saturate(uMaterial.Metallic);
	if(uHasTextures.w > 0)
	{
		Metallic = texture(MetalnessTexture, TexCoord0).r;
	}

	vec4 AlbedoColor = pow(uMaterial.BaseColor, vec4(Gamma, Gamma, Gamma, 1.0f));
	if(uHasTextures.x > 0)
	{
		// Gamma corrected albedo color
		AlbedoColor = pow(texture(AlbedoTexture, TexCoord0), vec4(Gamma, Gamma, Gamma, 1.0f));
	}

	vec4 BaseColor = Diffuse(AlbedoColor);
	BaseColor = mix(BaseColor, vec4(0.0f, 0.0f, 0.0f, 1.0f), Metallic);
	vec3 SpecularColor = mix(vec3(0.04f), AlbedoColor.rgb, Metallic);

	vec3 Spec = Specular(SpecularColor, LdotN, VdotN, NdotH, VdotH, Alpha);
	vec3 envFresnel = Specular_F_Roughness(SpecularColor, Alpha, VdotH);

	vec3 Reflected = reflect(-ViewDirection, normal);
	float mipIndex = Roughness * Roughness * 8.0f;
	vec4 ReflectedColor = pow(textureLod(EnvMap, Reflected, mipIndex), vec4(Gamma, Gamma, Gamma, 1.0f));

	float Shadow = CalculateShadow(FragPosLightSpace);

	vec4 Result = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	Result += BaseColor * 0.05f; // Ambient
	Result += Shadow * LightColor * LdotN * (BaseColor * (1.0f - vec4(Spec, 1.0f)) + vec4(Spec, 1.0f));
	Result += ReflectedColor * vec4(envFresnel, 1.0f) * 0.7f;

	FinalColor = pow(Result, vec4(InvGamma, InvGamma, InvGamma, 1.0f));
}