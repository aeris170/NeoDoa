// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::camera {
	using namespace internal::camera;

	float x{ 0 };
	float y{ 0 };
	float z{ 1 };

	float min_x{ std::numeric_limits<float>::lowest() };
	float min_y{ std::numeric_limits<float>::lowest() };
	float min_z{ std::numeric_limits<float>::lowest() };

	float max_x{ std::numeric_limits<float>::max() };
	float max_y{ std::numeric_limits<float>::max() };
	float max_z{ std::numeric_limits<float>::max() };

	float tween_x{ .005f };
	float tween_y{ .005f };
	float tween_z{ .025f };

	bool follow_enabled{ false };
	scene::GameObject* follow_target{ NULL };

	bool zoom_enabled{ false };
	scene::GameObject* zoom_target{ NULL };
}

namespace internal::camera {

	void update() {
		if (doa::camera::follow_enabled) {
			glm::vec3& pos{ *doa::camera::follow_target->GetPosition() }; // if errs, remove uniform initialization and turn into copy initialization
			doa::camera::x += (pos.x - doa::camera::x - doa::window_width / 2.f) * doa::camera::tween_x;
			doa::camera::y += (pos.y - doa::camera::y - doa::window_height / 2.f) * doa::camera::tween_y;
		}
		if (doa::camera::zoom_enabled) {
			doa::camera::z += (doa::mouse::z - doa::camera::z) * doa::camera::tween_z;
		}
		if (doa::camera::x < doa::camera::min_x) {
			doa::camera::x = doa::camera::min_x;
		}
		else if (doa::camera::x > doa::camera::max_x) {
			doa::camera::x = doa::camera::max_x;
		}
		if (doa::camera::y < doa::camera::min_y) {
			doa::camera::y = doa::camera::min_y;
		}
		else if (doa::camera::y > doa::camera::max_y) {
			doa::camera::y = doa::camera::max_y;
		}
		if (doa::camera::z < doa::camera::min_z) {
			doa::camera::z = doa::camera::min_z;
		}
		else if (doa::camera::z > doa::camera::max_z) {
			doa::camera::z = doa::camera::max_z;
		}
	}
}