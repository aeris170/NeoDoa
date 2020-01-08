// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::keyboard {
	using namespace internal::keyboard;

	bool IsKeyPressed(KeyCode key) {
		return press[key];
	}

	bool IsKeyReleased(KeyCode key) {
		return release[key];
	}

	bool IsKeyHold(KeyCode key) {
		return hold[key];
	}
}

namespace internal::keyboard {

	bool press[400];
	bool release[400];
	bool hold[400];

	void update() {
		for (int i{ 0 }; i < 400; ++i) {
			press[i] = 0;
			release[i] = 0;
		}
	}
}