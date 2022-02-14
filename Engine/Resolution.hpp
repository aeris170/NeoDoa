#pragma once

struct Resolution {
	int w, h;

	inline float Aspect() const { return w / h; }
};