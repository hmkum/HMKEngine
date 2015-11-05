#include "Light.h"

using namespace hmk;

Light::Light()
{
	name_		= "Default Light";
	position_	= glm::vec3(0.f);
	color_		= glm::vec3(1.f);
	intensity_	= 5.f;
	light_type_ = LightType::None;
}

Light::~Light()
{
}

void Light::set_name(std::string n)
{
	name_ = n;
}

void Light::set_position(const glm::vec3& pos)
{
	position_ = pos;
}

void Light::set_color(const glm::vec3& color)
{
	color_ = color;
}

void Light::set_intensity(float intensity)
{
	intensity_ = intensity;
}

DirectionalLight::DirectionalLight()
	: Light()
{
	target_		= glm::vec3(0.f, 0.f, -1.f);
	light_type_ = LightType::Directional;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::set_target(const glm::vec3& target)
{
	target_ = target;
}

glm::mat4 DirectionalLight::get_vp_matrix() const
{
	glm::mat4 view = glm::lookAt(-get_direction(), glm::vec3(0.f), glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -10.0f, 50.0f); // TODO_HMK: fix for scene size
	return proj * view;
}

PointLight::PointLight()
	: Light()
{
	attenuation_ = glm::vec3(1.f, 0.045f, 0.0075f);
	range_		 = 50.f;
	light_type_	 = LightType::Point;
}
PointLight::~PointLight()
{
}

void PointLight::set_attenuation(const glm::vec3& att)
{
	attenuation_ = att;
}

void PointLight::set_range(float range)
{
	range_ = range;
}

SpotLight::SpotLight()
	: Light()
{
	attenuation_ = glm::vec3(1.f, 0.045f, 0.0075f);
	range_		 = 50.f;
	angle_		 = 60.f;
	light_type_  = LightType::Spot;
}

SpotLight::~SpotLight()
{
}

void SpotLight::set_attenuation(const glm::vec3& att)
{
	attenuation_ = att;
}

void SpotLight::set_range(float range)
{
	range_ = range;
}