// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::mouse {

	enum ButtonCode {
		MB1, MB2, MB3, MB4, MB5, MB6, MB7, MB8
	};

	extern DOA_API float x;
	extern DOA_API float y;
	extern DOA_API float z;
	extern DOA_API float step_z;
	extern DOA_API float min_z;
	extern DOA_API float max_z;

	DOA_API bool IsButtonPressed(ButtonCode button);
	DOA_API bool IsButtonReleased(ButtonCode button);
	DOA_API bool IsButtonHold(ButtonCode button);
}

namespace internal::mouse {

	extern bool press[8];
	extern bool release[8];
	extern bool hold[8];

	void update();
}