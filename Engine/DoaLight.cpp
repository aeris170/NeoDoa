// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::scene {

	Light::Light(const glm::vec3& position, const glm::vec3& color, int    radius, int    intensity) : m_position{ new glm::vec3(position) }, m_color{ new glm::vec3(color) }, m_radius{ static_cast<float>(radius) }, m_intensity{ static_cast<float>(intensity) } {}
	Light::Light(const glm::vec3& position, const glm::vec3& color, int    radius, float  intensity) : m_position{ new glm::vec3(position) }, m_color{ new glm::vec3(color) }, m_radius{ static_cast<float>(radius) }, m_intensity{					   intensity  } {}
	Light::Light(const glm::vec3& position, const glm::vec3& color, int    radius, double intensity) : m_position{ new glm::vec3(position) }, m_color{ new glm::vec3(color) }, m_radius{ static_cast<float>(radius) }, m_intensity{ static_cast<float>(intensity) } {}
	Light::Light(const glm::vec3& position, const glm::vec3& color, float  radius, int    intensity) : m_position{ new glm::vec3(position) }, m_color{ new glm::vec3(color) }, m_radius{					radius  }, m_intensity{ static_cast<float>(intensity) } {}
	Light::Light(const glm::vec3& position, const glm::vec3& color, float  radius, float  intensity) : m_position{ new glm::vec3(position) }, m_color{ new glm::vec3(color) }, m_radius{					radius  }, m_intensity{                    intensity  } {}
	Light::Light(const glm::vec3& position, const glm::vec3& color, float  radius, double intensity) : m_position{ new glm::vec3(position) }, m_color{ new glm::vec3(color) }, m_radius{					radius  }, m_intensity{ static_cast<float>(intensity) } {}
	Light::Light(const glm::vec3& position, const glm::vec3& color, double radius, int    intensity) : m_position{ new glm::vec3(position) }, m_color{ new glm::vec3(color) }, m_radius{ static_cast<float>(radius) }, m_intensity{ static_cast<float>(intensity) } {}
	Light::Light(const glm::vec3& position, const glm::vec3& color, double radius, float  intensity) : m_position{ new glm::vec3(position) }, m_color{ new glm::vec3(color) }, m_radius{ static_cast<float>(radius) }, m_intensity{                    intensity  } {}
	Light::Light(const glm::vec3& position, const glm::vec3& color, double radius, double intensity) : m_position{ new glm::vec3(position) }, m_color{ new glm::vec3(color) }, m_radius{ static_cast<float>(radius) }, m_intensity{ static_cast<float>(intensity) } {}
	Light::~Light() { delete m_position; delete m_color; }

	PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, int    radius) : Light(position, color, static_cast<float>(radius), 0) {}
	PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, float  radius) : Light(position, color,                    radius,  0) {}
	PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, double radius) : Light(position, color, static_cast<float>(radius), 0) {}
	PointLight::~PointLight() {}

	SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& color, int    radius) : Light(position, color, static_cast<float>(radius), 1) {}
	SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& color, float  radius) : Light(position, color,                    radius,  1) {}
	SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& color, double radius) : Light(position, color, static_cast<float>(radius), 1) {}
	SpotLight::~SpotLight() {}
}