#include "EZEasing.hpp"
namespace eze {

	float easeLinear(float ratio) {
		return ratio;
	}

	float easeInSine(float ratio) {
		return 1 - cos((ratio * M_PI) / 2);
	}

	float easeOutSine(float ratio) {
		return sin((ratio * M_PI) / 2);
	}

	float easeInOutSine(float ratio) {
		return -cos(((ratio * M_PI) - 1) / 2);
	}

	float easeInQuad(float ratio) {
		return ratio * ratio;
	}

	float easeOutQuad(float ratio) {
		return 1 - (1 - ratio) * (1 - ratio);
	}

	float easeInOutQuad(float ratio) {
		return ratio < 0.5 ? 2 * ratio * ratio : 1 - pow(-2 * ratio + 2, 2) / 2;
	}

	float easeInCubic(float ratio) {
		return ratio * ratio * ratio;
	}

	float easeOutCubic(float ratio) {
		return 1 - pow(1 - ratio, 3);
	}

	float easeInOutCubic(float ratio) {
		return ratio < 0.5 ? 4 * ratio * ratio * ratio : 1 - pow(-2 * ratio + 2, 3) / 2;
	}

	float easeInQuart(float ratio) {
		return ratio * ratio * ratio * ratio;
	}

	float easeOutQuart(float ratio) {
		return 1 - pow(1 - ratio, 4);
	}

	float easeInOutQuart(float ratio) {
		return ratio < 0.5 ? 8 * ratio * ratio * ratio * ratio : 1 - pow(-2 * ratio + 2, 4) / 2;
	}

	float easeInQuint(float ratio) {
		return ratio * ratio * ratio * ratio * ratio;
	}

	float easeOutQuint(float ratio) {
		return 1 - pow(1 - ratio, 5);
	}

	float easeInOutQuint(float ratio) {
		return ratio < 0.5 ? 16 * ratio * ratio * ratio * ratio * ratio : 1 - pow(-2 * ratio + 2, 5) / 2;
	}


	float easeInExpo(float ratio) {
		return ratio == 0 ? 0 : pow(2, 10 * ratio - 10);
	}

	float easeOutExpo(float ratio) {
		return ratio == 1 ? 1 : 1 - pow(2, -10 * ratio);
	}

	float easeInOutExpo(float ratio) {
		return ratio == 0
			? 0
			: ratio == 1
			? 1
			: ratio < 0.5 ? pow(2, 20 * ratio - 10) / 2
			: (2 - pow(2, -20 * ratio + 10)) / 2;
	}

	float easeInCirc(float ratio) {
		return 1 - sqrt(1 - pow(ratio, 2));
	}

	float easeOutCirc(float ratio) {
		return sqrt(1 - pow(ratio - 1, 2));
	}

	float easeInOutCirc(float ratio) {
		return ratio < 0.5
			? (1 - sqrt(1 - pow(2 * ratio, 2))) / 2
			: (sqrt(1 - pow(-2 * ratio + 2, 2)) + 1) / 2;
	}

	float easeInBack(float ratio) {
		static const float c1 = 1.70158;
		static const float c3 = c1 + 1;

		return c3 * ratio * ratio * ratio - c1 * ratio * ratio;
	}

	float easeOutBack(float ratio) {
		static const float c1 = 1.70158;
		static const float c3 = c1 + 1;

		return 1 + c3 * pow(ratio - 1, 3) + c1 * pow(ratio - 1, 2);
	}

	float easeInOutBack(float ratio) {
		static const float c1 = 1.70158;
		static const float c2 = c1 * 1.525;

		return ratio < 0.5
			? (pow(2 * ratio, 2) * ((c2 + 1) * 2 * ratio - c2)) / 2
			: (pow(2 * ratio - 2, 2) * ((c2 + 1) * (ratio * 2 - 2) + c2) + 2) / 2;
	}

	float easeInElastic(float ratio) {
		static const float c4 = (2 * M_PI) / 3;

		return ratio == 0
			? 0
			: ratio == 1
			? 1
			: -pow(2, 10 * ratio - 10) * sin((ratio * 10 - 10.75) * c4);
	}

	float easeOutElastic(float ratio) {
		static const float c4 = (2 * M_PI) / 3;

		return ratio == 0
			? 0
			: ratio == 1
			? 1
			: pow(2, -10 * ratio) * sin((ratio * 10 - 0.75) * c4) + 1;
	}

	float easeInOutElastic(float ratio) {
		static const float c5 = (2 * M_PI) / 4.5;

		return ratio == 0
			? 0
			: ratio == 1
			? 1
			: ratio < 0.5
			? -(pow(2, 20 * ratio - 10) * sin((20 * ratio - 11.125) * c5)) / 2
			: (pow(2, -20 * ratio + 10) * sin((20 * ratio - 11.125) * c5)) / 2 + 1;
	}


	float easeInBounce(float ratio) {
		return 1 - easeOutBounce(1 - ratio);
	}

	float easeOutBounce(float ratio) {
		static const float n1 = 7.5625;
		static const float d1 = 2.75;

		if (ratio < 1 / d1) {
			return n1 * ratio * ratio;
		} else if (ratio < 2 / d1) {
			return n1 * (ratio -= 1.5 / d1) * ratio + 0.75;
		} else if (ratio < 2.5 / d1) {
			return n1 * (ratio -= 2.25 / d1) * ratio + 0.9375;
		} else {
			return n1 * (ratio -= 2.625 / d1) * ratio + 0.984375;
		}
	}

	float easeInOutBounce(float ratio) {
		return ratio < 0.5
			? (1 - easeOutBounce(1 - 2 * ratio)) / 2
			: (1 + easeOutBounce(2 * ratio - 1)) / 2;
	}

}
