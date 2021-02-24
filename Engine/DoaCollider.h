#pragma once

namespace doa::scene {

	class DOA_API Collider {
	protected:
		GameObject* parent{ NULL };

		std::vector<glm::vec2> m_boundary_points;

	public:
		Collider();
		Collider(const Collider& right);
		Collider(const glm::vec2* points, int count);
		Collider(const std::vector<glm::vec2>& points);
		virtual ~Collider() = 0;

		Collider& operator= (const Collider& right);
	private:
		void update();
	};
}