// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::physics {
	void SetGravity(const glm::vec2& gravity);
	void SetGravity(const float x, const float y);
	void SetGravityX(const float x);
	void SetGravityY(const float y);

	glm::vec2 GetGravity();
	float GetGravityX();
	float GetGravityY();
}

namespace internal::physics {
	extern b2World* m_physics;
	extern glm::vec2 m_gravity;

	void init();
	void purge();
}