// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::scene {

	Collider::Collider() {}

	Collider::Collider(const Collider& right) {
		m_boundary_points = std::vector<glm::vec2>(right.m_boundary_points);
	}

	Collider::Collider(const glm::vec2* points, int count) {
		m_boundary_points.reserve(count);
		for (int i = 0; i < count; ++i) {
			m_boundary_points.push_back(points[i]);
		}
	}

	Collider::Collider(const std::vector<glm::vec2>& points) : m_boundary_points{ points } {}

	Collider::~Collider() {}

	Collider& Collider::operator= (const Collider& right) {
		parent = NULL;
		m_boundary_points = std::vector<glm::vec2>(right.m_boundary_points);
		return *this;
	}
}