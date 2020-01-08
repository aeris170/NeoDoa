// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::mouse {
	using namespace internal::mouse;

	float x{ 0 };
	float y{ 0 };
	float z{ 1 };
	float step_z{ .05f };
	float min_z{ .5 };
	float max_z{ 2 };

	bool IsButtonPressed(ButtonCode button) {
		return press[button];
	}

	bool IsButtonReleased(ButtonCode button) {
		return release[button];
	}

	bool IsButtonHold(ButtonCode button) {
		return hold[button];
	}
}

namespace internal::mouse {

	bool press[8];
	bool release[8];
	bool hold[8];

	void update() {
		for (int i{ 0 }; i < 8; ++i) {
			press[i] = false;
			release[i] = false;
		}
	}
}