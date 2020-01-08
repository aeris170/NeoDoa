// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::scene {

	class DOA_API Light {
	protected:
		glm::vec3 *m_position;
		glm::vec3 *m_color;
		float m_radius;
		float m_intensity;

	public:
		Light(const glm::vec3& position, const glm::vec3& color, int    radius, int    intensity);
		Light(const glm::vec3& position, const glm::vec3& color, int    radius, float  intensity);
		Light(const glm::vec3& position, const glm::vec3& color, int    radius, double intensity);
		Light(const glm::vec3& position, const glm::vec3& color, float  radius, int    intensity);
		Light(const glm::vec3& position, const glm::vec3& color, float  radius, float  intensity);
		Light(const glm::vec3& position, const glm::vec3& color, float  radius, double intensity);
		Light(const glm::vec3& position, const glm::vec3& color, double radius, int    intensity);
		Light(const glm::vec3& position, const glm::vec3& color, double radius, float  intensity);
		Light(const glm::vec3& position, const glm::vec3& color, double radius, double intensity);
		virtual ~Light();

		virtual inline glm::vec3* GetPosition() const { return m_position; }
		virtual inline void SetPosition(const glm::vec3& position) { m_position->x = position.x; m_position->y = position.y; m_position->z = position.z; }

		virtual inline glm::vec3* GetColor() const { return m_color; }
		virtual inline void SetColor(const glm::vec3& color) { m_color->r = color.r; m_color->g = color.g; m_color->b = color.b; }

		virtual inline float GetRadius() const { return m_radius; }
		virtual inline void SetRadius(float radius) { m_radius = radius; }

		virtual inline float GetIntensity() const { return m_intensity; }
		virtual inline void SetIntensity(float intensity) { m_intensity = intensity; }
	};

	class DOA_API PointLight : public Light {
	public:
		PointLight(const glm::vec3& position, const glm::vec3& color, int    radius);
		PointLight(const glm::vec3& position, const glm::vec3& color, float  radius);
		PointLight(const glm::vec3& position, const glm::vec3& color, double radius);
		virtual ~PointLight();
	};

	class DOA_API SpotLight : public Light {
	public:
		SpotLight(const glm::vec3& position, const glm::vec3& color, int    radius);
		SpotLight(const glm::vec3& position, const glm::vec3& color, float  radius);
		SpotLight(const glm::vec3& position, const glm::vec3& color, double radius);
		virtual ~SpotLight();
	};
}