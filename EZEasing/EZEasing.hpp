namespace eze {

    // ratio is in range [0-1]
    float easeLinear(float ratio);

    float easeInSine(float ratio);
    float easeOutSine(float ratio);
    float easeInOutSine(float ratio);

    float easeInQuad(float ratio);
    float easeOutQuad(float ratio);
    float easeInOutQuad(float ratio);

    float easeInCubic(float ratio);
    float easeOutCubic(float ratio);
    float easeInOutCubic(float ratio);

    float easeInQuart(float ratio);
    float easeOutQuart(float ratio);
    float easeInOutQuart(float ratio);

    float easeInQuint(float ratio);
    float easeOutQuint(float ratio);
    float easeInOutQuint(float ratio);

    float easeInExpo(float ratio);
    float easeOutExpo(float ratio);
    float easeInOutExpo(float ratio);

    float easeInCirc(float ratio);
    float easeOutCirc(float ratio);
    float easeInOutCirc(float ratio);

    float easeInBack(float ratio);
    float easeOutBack(float ratio);
    float easeInOutBack(float ratio);

    float easeInElastic(float ratio);
    float easeOutElastic(float ratio);
    float easeInOutElastic(float ratio);

    float easeInBounce(float ratio);
    float easeOutBounce(float ratio);
    float easeInOutBounce(float ratio);

    // Templates...
    template <typename T>
    static inline T map(T start, T end, float value) {
        return (value - 0) / (1 - 0) * (end - start) + start;
    }

    // Linear
    template <typename T>
    T easeLinear(T start, T end, float ratio) {
        return map(start, end, easeLinear(ratio));
    }
    // Linear

    // Sine
    template <typename T>
    T easeInSine(T start, T end, float ratio) {
        return map(start, end, easeInSine(ratio));
    }

    template <typename T>
    T easeOutSine(T start, T end, float ratio) {
        return map(start, end, easeOutSine(ratio));
    }

    template <typename T>
    T easeInOutSine(T start, T end, float ratio) {
        return map(start, end, easeInOutSine(ratio));
    }
    // Sine

    // Quad
    template <typename T>
    T easeInQuad(T start, T end, float ratio) {
        return map(start, end, easeInQuad(ratio));
    }

    template <typename T>
    T easeOutQuad(T start, T end, float ratio) {
        return map(start, end, easeOutQuad(ratio));
    }

    template <typename T>
    T easeInOutQuad(T start, T end, float ratio) {
        return map(start, end, easeInOutQuad(ratio));
    }
    // Quad

    // Cubic
    template <typename T>
    T easeInCubic(T start, T end, float ratio) {
        return map(start, end, easeInCubic(ratio));
    }

    template <typename T>
    T easeOutCubic(T start, T end, float ratio) {
        return map(start, end, easeOutCubic(ratio));
    }

    template <typename T>
    T easeInOutCubic(T start, T end, float ratio) {
        return map(start, end, easeInOutCubic(ratio));
    }
    // Cubic

    // Quart
    template <typename T>
    T easeInQuart(T start, T end, float ratio) {
        return map(start, end, easeInQuart(ratio));
    }

    template <typename T>
    T easeOutQuart(T start, T end, float ratio) {
        return map(start, end, easeOutQuart(ratio));
    }

    template <typename T>
    T easeInOutQuart(T start, T end, float ratio) {
        return map(start, end, easeInOutQuart(ratio));
    }
    // Quart

    // Quint
    template <typename T>
    T easeInQuint(T start, T end, float ratio) {
        return map(start, end, easeInQuint(ratio));
    }

    template <typename T>
    T easeOutQuint(T start, T end, float ratio) {
        return map(start, end, easeOutQuint(ratio));
    }

    template <typename T>
    T easeInOutQuint(T start, T end, float ratio) {
        return map(start, end, easeInOutQuint(ratio));
    }
    // Quint

    // Expo
    template <typename T>
    T easeInExpo(T start, T end, float ratio) {
        return map(start, end, easeInExpo(ratio));
    }

    template <typename T>
    T easeOutExpo(T start, T end, float ratio) {
        return map(start, end, easeOutExpo(ratio));
    }

    template <typename T>
    T easeInOutExpo(T start, T end, float ratio) {
        return map(start, end, easeInOutExpo(ratio));
    }
    // Expo

    // Circ
    template <typename T>
    T easeInCirc(T start, T end, float ratio) {
        return map(start, end, easeInCirc(ratio));
    }

    template <typename T>
    T easeOutCirc(T start, T end, float ratio) {
        return map(start, end, easeOutCirc(ratio));
    }

    template <typename T>
    T easeInOutCirc(T start, T end, float ratio) {
        return map(start, end, easeInOutCirc(ratio));
    }
    // Circ

    // Back
    template <typename T>
    T easeInBack(T start, T end, float ratio) {
        return map(start, end, easeInBack(ratio));
    }

    template <typename T>
    T easeOutBack(T start, T end, float ratio) {
        return map(start, end, easeOutBack(ratio));
    }

    template <typename T>
    T easeInOutBack(T start, T end, float ratio) {
        return map(start, end, easeInOutBack(ratio));
    }
    // Back

    // Elastic
    template <typename T>
    T easeInElastic(T start, T end, float ratio) {
        return map(start, end, easeInElastic(ratio));
    }

    template <typename T>
    T easeOutElastic(T start, T end, float ratio) {
        return map(start, end, easeOutElastic(ratio));
    }

    template <typename T>
    T easeInOutElastic(T start, T end, float ratio) {
        return map(start, end, easeInOutElastic(ratio));
    }
    // Elatic

    // Bounce
    template <typename T>
    T easeInBounce(T start, T end, float ratio) {
        return map(start, end, easeInBounce(ratio));
    }

    template <typename T>
    T easeOutBounce(T start, T end, float ratio) {
        return map(start, end, easeOutBounce(ratio));
    }

    template <typename T>
    T easeInOutBounce(T start, T end, float ratio) {
        return map(start, end, easeInOutBounce(ratio));
    }
    // Bounce
}