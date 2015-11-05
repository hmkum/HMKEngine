#pragma once
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace hmk
{
enum class LightType {None, Directional, Point, Spot, Area};

struct LightUniform
{
	glm::vec3 position_ = glm::vec3(0.f);
	glm::vec3 target_ = glm::vec3(0, 0, -1);
	glm::vec4 color_ = glm::vec4(1, 0, 0, 5); // .rgb:color, .a:intensity
	glm::vec4 attenuation_ = glm::vec4(1.f, 0.045f, 0.0075f, 50.f); // .rgb:attenuation, .a:range
	float spot_light_angle_ = 60.f;
	int light_type_ = (int)LightType::Directional; // 1:Directional, 2:Point, 3:Spot
};

class Light
{
public:
	Light();
	virtual ~Light();

	void set_name(std::string n);
	void set_position(const glm::vec3& pos);
	void set_color(const glm::vec3& color);
	void set_intensity(float intensity);
	void set_type(LightType type);

	inline std::string get_name()	const { return name_; }
	inline glm::vec3 get_position() const { return position_; }
	inline glm::vec3 get_color()	const { return color_; }
	inline float get_intensity()	const { return intensity_; }
	inline LightType get_type()		const { return light_type_; }

protected:
	std::string name_;
	glm::vec3 position_;
	glm::vec3 color_;
	float intensity_;
	LightType light_type_;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	virtual ~DirectionalLight();

	void set_target(const glm::vec3& target);

	inline glm::vec3 get_direction()	const { return glm::normalize(target_ - position_); }
	inline glm::vec3 get_target()		const { return target_; }
	glm::mat4 get_vp_matrix()			const;

private:
	glm::vec3 target_;
};

class PointLight : public Light
{
public:
	PointLight();
	virtual ~PointLight();

	void set_attenuation(const glm::vec3& att);
	void set_range(float range);
	
	inline glm::vec3 get_attenuation()	const { return attenuation_; }
	inline float get_range()			const { return range_; }

private:
	glm::vec3 attenuation_;
	float range_;
};

class SpotLight : public Light
{
public:
	SpotLight();
	virtual ~SpotLight();

	void set_attenuation(const glm::vec3& att);
	void set_range(float range);
	void set_angle(float angle);

	inline glm::vec3 get_attenuation()	const { return attenuation_; }
	inline float get_range()			const { return range_; }
	inline float get_angle()			const { return angle_; }

private:
	glm::vec3 attenuation_;
	float range_;
	float angle_;
};

}