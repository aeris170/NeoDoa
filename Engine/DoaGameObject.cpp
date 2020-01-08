// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::scene {

	GameObject::GameObject(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& scale) :
		m_position{ new glm::vec3(position) }, m_rotation{ new glm::vec3(rotation) }, m_scale{ new glm::vec2(scale) } {}

	GameObject::~GameObject() {
		delete m_position;
		delete m_rotation;
		delete m_scale;
	}
}