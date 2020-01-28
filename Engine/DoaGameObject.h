// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::scene {

	class DOA_API GameObject {
	protected:
		glm::vec3* m_position{ NULL };
		glm::vec3* m_rotation{ NULL };
		glm::vec2* m_scale{ NULL };

		bool m_is_fixed{ false };

		shader::ShaderProgram* m_shader{ NULL };

	public:
		GameObject(const glm::vec3& position = glm::vec3(0, 0, 0), const glm::vec3& rotation = glm::vec3(0, 0, 0), const glm::vec2& scale = glm::vec2(1, 1));
		virtual ~GameObject() = 0;

		virtual inline glm::vec3* GetPosition() const { return m_position; }
		virtual inline void SetPosition(const glm::vec3& newPosition) { *m_position = newPosition; }
		virtual inline void SetPosition(float newX, float newY, float newZ) { m_position->x = newX; m_position->y = newY; m_position->z = newZ; }
		virtual inline void SetPosition(const glm::vec2& newPosition) { m_position->x = newPosition.x; m_position->y = newPosition.y; }
		virtual inline void SetPosition(float newX, float newY) { m_position->x = newX; m_position->y = newY; }

		virtual inline glm::vec3* GetRotation() const { return m_rotation; }
		virtual inline void SetRotation(const glm::vec3& newRotation) { *m_rotation = newRotation; }
		virtual inline void SetRotation(float newRotationX, float newRotationY, float newRotationZ) { m_rotation->x = newRotationX; m_rotation->y = newRotationY; m_rotation->z = newRotationZ; }

		virtual inline glm::vec2* GetScale() const { return m_scale; }
		virtual inline void SetScale(const glm::vec2& newScale) { *m_scale = newScale; }
		virtual inline void SetScale(float newScaleX, float newScaleY) { m_scale->x = newScaleX; m_scale->y = newScaleY; }

		virtual inline const bool IsFixed() const { return m_is_fixed; }
		virtual inline void SetFixed(const bool isFixed) { m_is_fixed = isFixed; }

		virtual inline const shader::ShaderProgram* GetShaderProgramID() const { return m_shader; }
		virtual inline void SetShaderProgram(shader::ShaderProgram* shader) { m_shader = shader; }

		virtual void update(const Scene& parent, const std::vector<GameObject*>& objects, const std::vector<Light*>& lights) = 0;
		virtual void render(const Scene& parent, const std::vector<GameObject*>& objects, const std::vector<Light*>& lights) const = 0;
	};
}