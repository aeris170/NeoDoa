// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::camera {

	extern DOA_API float x;
	extern DOA_API float y;
	extern DOA_API float z;
	extern DOA_API float min_x;
	extern DOA_API float min_y;
	extern DOA_API float min_z;
	extern DOA_API float max_x;
	extern DOA_API float max_y;
	extern DOA_API float max_z;
	extern DOA_API float tween_x;
	extern DOA_API float tween_y;
	extern DOA_API float tween_z;
	extern DOA_API bool follow_enabled;
	extern DOA_API doa::scene::GameObject* follow_target;
	extern DOA_API bool zoom_enabled;
	extern DOA_API doa::scene::GameObject* zoom_target;
}

namespace internal::camera {

	void update();
}