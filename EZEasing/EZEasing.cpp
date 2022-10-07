#include "EZEasing.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

namespace eze {

    constexpr static float PI { static_cast<float>(M_PI) };

    float easeLinear(float ratio) {
        return ratio;
    }

    float easeInSine(float ratio) {
        return 1.0f - cos((ratio * PI) / 2.0f);
    }

    float easeOutSine(float ratio) {
        return sin((ratio * PI) / 2.0f);
    }

    float easeInOutSine(float ratio) {
        return -cos(((ratio * PI) - 1.0f) / 2.0f);
    }

    float easeInQuad(float ratio) {
        return ratio * ratio;
    }

    float easeOutQuad(float ratio) {
        return 1.0f - (1.0f - ratio) * (1.0f - ratio);
    }

    float easeInOutQuad(float ratio) {
        return ratio < 0.5f ? 2.0f * ratio * ratio : 1.0f - pow(-2.0f * ratio + 2.0f, 2.0f) / 2.0f;
    }

    float easeInCubic(float ratio) {
        return ratio * ratio * ratio;
    }

    float easeOutCubic(float ratio) {
        return 1.0f - pow(1.0f - ratio, 3.0f);
    }

    float easeInOutCubic(float ratio) {
        return ratio < 0.5f ? 4.0f * ratio * ratio * ratio : 1.0f - pow(-2.0f * ratio + 2.0f, 3.0f) / 2.0f;
    }

    float easeInQuart(float ratio) {
        return ratio * ratio * ratio * ratio;
    }

    float easeOutQuart(float ratio) {
        return 1.0f - pow(1.0f - ratio, 4.0f);
    }

    float easeInOutQuart(float ratio) {
        return ratio < 0.5f ? 8.0f * ratio * ratio * ratio * ratio : 1.0f - pow(-2.0f * ratio + 2.0f, 4.0f) / 2.0f;
    }

    float easeInQuint(float ratio) {
        return ratio * ratio * ratio * ratio * ratio;
    }

    float easeOutQuint(float ratio) {
        return 1.0f - pow(1.0f - ratio, 5.0f);
    }

    float easeInOutQuint(float ratio) {
        return ratio < 0.5f ? 16.0f * ratio * ratio * ratio * ratio * ratio : 1.0f - pow(-2.0f * ratio + 2.0f, 5.0f) / 2.0f;
    }


    float easeInExpo(float ratio) {
        return ratio == 0.0f ? 0.0f : pow(2.0f, 10.0f * ratio - 10.0f);
    }

    float easeOutExpo(float ratio) {
        return ratio == 1.0f ? 1.0f : 1.0f - pow(2.0f, -10.0f * ratio);
    }

    float easeInOutExpo(float ratio) {
        return ratio == 0.0f
            ? 0.0f
            : ratio == 1.0f
            ? 1.0f
            : ratio < 0.5f ? pow(2.0f, 20.0f * ratio - 10.0f) / 2.0f
            : (2.0f - pow(2.0f, -20.0f * ratio + 10.0f)) / 2.0f;
    }

    float easeInCirc(float ratio) {
        return 1.0f - sqrt(1.0f - pow(ratio, 2.0f));
    }

    float easeOutCirc(float ratio) {
        return sqrt(1.0f - pow(ratio - 1.0f, 2.0f));
    }

    float easeInOutCirc(float ratio) {
        return ratio < 0.5f
            ? (1.0f - sqrt(1.0f - pow(2.0f * ratio, 2.0f))) / 2.0f
            : (sqrt(1.0f - pow(-2.0f * ratio + 2.0f, 2.0f)) + 1.0f) / 2.0f;
    }

    float easeInBack(float ratio) {
        static constexpr float c1 = 1.70158f;
        static constexpr float c3 = c1 + 1.0f;

        return c3 * ratio * ratio * ratio - c1 * ratio * ratio;
    }

    float easeOutBack(float ratio) {
        static constexpr float c1 = 1.70158f;
        static constexpr float c3 = c1 + 1.0f;

        return 1.0f + c3 * pow(ratio - 1.0f, 3.0f) + c1 * pow(ratio - 1.0f, 2.0f);
    }

    float easeInOutBack(float ratio) {
        static constexpr float c1 = 1.70158f;
        static constexpr float c2 = c1 * 1.525f;

        return ratio < 0.5
            ? (pow(2.0f * ratio, 2.0f) * ((c2 + 1) * 2.0f * ratio - c2)) / 2.0f
            : (pow(2.0f * ratio - 2.0f, 2.0f) * ((c2 + 1.0f) * (ratio * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
    }

    float easeInElastic(float ratio) {
        static constexpr float c4 = (2.0f * PI) / 3.0f;

        return ratio == 0.0f
            ? 0.0f
            : ratio == 1.0f
            ? 1.0f
            : -pow(2.0f, 10.0f * ratio - 10.0f) * sin((ratio * 10.0f - 10.75f) * c4);
    }

    float easeOutElastic(float ratio) {
        static constexpr float c4 = (2.0f * PI) / 3.0f;

        return ratio == 0.0f
            ? 0.0f
            : ratio == 1.0f
            ? 1.0f
            : pow(2.0f, -10.0f * ratio) * sin((ratio * 10.0f - 0.75f) * c4) + 1.0f;
    }

    float easeInOutElastic(float ratio) {
        static const float c5 = (2.0f * PI) / 4.5f;

        return ratio == 0.0f
            ? 0.0f
            : ratio == 1.0f
            ? 1.0f
            : ratio < 0.5f
            ? -(pow(2.0f, 20.0f * ratio - 10.0f) * sin((20.0f * ratio - 11.125f) * c5)) / 2.0f
            : (pow(2.0f, -20.0f * ratio + 10.0f) * sin((20.0f * ratio - 11.125f) * c5)) / 2.0f + 1.0f;
    }


    float easeInBounce(float ratio) {
        return 1.0f - easeOutBounce(1.0f - ratio);
    }

    float easeOutBounce(float ratio) {
        static constexpr float n1 = 7.5625f;
        static constexpr float d1 = 2.75f;

        if (ratio < 1.0f / d1) {
            return n1 * ratio * ratio;
        } else if (ratio < 2.0f / d1) {
            return n1 * (ratio -= 1.5f / d1) * ratio + 0.75f;
        } else if (ratio < 2.5f / d1) {
            return n1 * (ratio -= 2.25f / d1) * ratio + 0.9375f;
        } else {
            return n1 * (ratio -= 2.625f / d1) * ratio + 0.984375f;
        }
    }

    float easeInOutBounce(float ratio) {
        return ratio < 0.5f
            ? (1.0f - easeOutBounce(1.0f - 2.0f * ratio)) / 2.0f
            : (1.0f + easeOutBounce(2.0f * ratio - 1.0f)) / 2.0f;
    }

}
