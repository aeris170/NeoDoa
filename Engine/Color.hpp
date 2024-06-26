#pragma once

#include <cmath>
#include <string>
#include <format>
#include <algorithm>

#include <imgui.h>
#include <glm/glm.hpp>

struct Color {

    static constexpr Color WHITE() { return { 1.0f, 1.0f, 1.0f, 1.0f }; };
    static constexpr Color GRAY() { return { 0.5f, 0.5f, 0.5f, 1.0f }; };
    static constexpr Color BLACK() { return { 0.0f, 0.0f, 0.0f, 1.0f }; };
    static constexpr Color RED() { return { 1.0f, 0.0f, 0.0f, 1.0f }; };
    static constexpr Color GREEN() { return { 0.0f, 1.0f, 0.0f, 1.0f }; };
    static constexpr Color BLUE() { return { 0.0f, 0.0f, 1.0f, 1.0f }; };
    static constexpr Color CYAN() { return { 0.0f, 1.0f, 1.0f, 1.0f }; };
    static constexpr Color MAGENTA() { return { 1.0f, 0.0f, 1.0f, 1.0f }; };
    static constexpr Color YELLOW() { return { 1.0f, 1.0f, 0.0f, 1.0f }; };
    static constexpr Color ZERO() { return { 0.0f, 0.0f, 0.0f, 0.0f }; };

    constexpr static float ConvertGammaToLinear(float value) {
        if (value <= 0.04045f) {
            return value / 12.92f;
        } else if (value < 1.0f) {
            return std::pow((value + 0.055f) / 1.055f, 2.4f);
        } else {
            return std::pow(value, 2.2f);
        }
    }
    constexpr static float ConvertLinearToGamma(float value) {
        if (value <= 0.0f) {
            return 0.0f;
        } else if (value <= 0.0031308f) {
            return 12.92f * value;
        } else if (value < 1.0f) {
            return 1.055f * std::pow(value, 0.4166667f) - 0.055f;
        } else {
            return std::pow(value, 0.45454545f);
        }
    }

    float r, g, b, a;

    constexpr Color() noexcept : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    constexpr Color(const Color& color) noexcept = default;
    constexpr Color(const Color& color, float a) noexcept : r(color.r), g(color.g), b(color.b), a(a) {}
    constexpr explicit Color(float v) noexcept : r(v), g(v), b(v), a(1.0f) {}
    constexpr Color(float r, float g, float b) noexcept : r(r), g(g), b(b), a(1.0f) {}
    constexpr Color(float r, float g, float b, float a) noexcept : r(r), g(g), b(b), a(a) {}
    constexpr explicit Color(const float* data) noexcept : r(data[0]), g(data[1]), b(data[2]), a(data[3]) {}
    constexpr explicit Color(const glm::vec3& color) noexcept : Color(color.r, color.g, color.b) {}
    constexpr explicit Color(const glm::vec4& color) noexcept : Color(color.r, color.g, color.b, color.a) {}

    constexpr Color& operator=(const Color& rhs) noexcept = default;
    constexpr bool operator==(const Color& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
    constexpr Color operator*(float rhs) const { return Color(r * rhs, g * rhs, b * rhs, a * rhs); }
    constexpr Color operator+(const Color& rhs) const { return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a); }
    constexpr Color operator-() const { return Color(-r, -g, -b, -a); }
    constexpr Color operator-(const Color& rhs) const { return Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a); }
    constexpr Color& operator+=(const Color& rhs) {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
        a += rhs.a;
        return *this;
    }

    constexpr float* Data() { return &r; }
    constexpr const float* Data() const { return &r; }

    constexpr unsigned int ToUInt() const {
        auto r = (unsigned) std::clamp(((int) (this->r * 255.0f)), 0, 255);
        auto g = (unsigned) std::clamp(((int) (this->g * 255.0f)), 0, 255);
        auto b = (unsigned) std::clamp(((int) (this->b * 255.0f)), 0, 255);
        auto a = (unsigned) std::clamp(((int) (this->a * 255.0f)), 0, 255);
        return (a << 24u) | (b << 16u) | (g << 8u) | r;
    }
    constexpr glm::vec3 ToHSL() const {
        float min, max;
        Bounds(&min, &max, true);

        float h = Hue(min, max);
        float s = SaturationHSL(min, max);
        float l = (max + min) * 0.5f;

        return { h, s, l };
    }
    constexpr glm::vec3 ToHSV() const {
        float min, max;
        Bounds(&min, &max, true);

        float h = Hue(min, max);
        float s = SaturationHSV(min, max);
        float v = max;

        return { h, s, v };
    }

    constexpr void FromUInt(unsigned color) {
        a = ((color >> 24u) & 0xffu) / 255.0f;
        b = ((color >> 16u) & 0xffu) / 255.0f;
        g = ((color >> 8u) & 0xffu) / 255.0f;
        r = ((color >> 0u) & 0xffu) / 255.0f;
    }
    inline void FromHSL(float h, float s, float l, float a = 1.0f) {
        float c;
        if (l < 0.5f) {
            c = (1.0f + (2.0f * l - 1.0f)) * s;
        } else {
            c = (1.0f - (2.0f * l - 1.0f)) * s;
        }
        float m = l - 0.5f * c;
        FromHCM(h, c, m);
        this->a = a;
    }
    inline void FromHSV(float h, float s, float v, float a = 1.0f) {
        float c = v * s;
        float m = v - c;
        FromHCM(h, c, m);
        this->a = a;
    }

    constexpr glm::vec3 ToVec3() const { return { r, g, b }; }
    constexpr glm::vec4 ToVec4() const { return { r, g, b, a }; }

    constexpr ImVec4 ToImVec4() const { return { r, g, b, a }; }

    constexpr float SumRGB() const { return r + g + b; }
    constexpr float Average() const { return (r + g + b) / 3.0f; }
    constexpr float Luma() const { return r * 0.299f + g * 0.587f + b * 0.114f; } // greyscale component

    constexpr float Chroma() const {
        float min, max;
        Bounds(&min, &max, true);
        return max - min;
    }
    constexpr float Hue() const {
        float min, max;
        Bounds(&min, &max, true);
        return Hue(min, max);
    }
    constexpr float SaturationHSL() const {
        float min, max;
        Bounds(&min, &max, true);
        return SaturationHSL(min, max);
    }
    constexpr float SaturationHSV() const {
        float min, max;
        Bounds(&min, &max, true);
        return SaturationHSV(min, max);
    }
    constexpr float Value() const {
        return LargestComponent();
    }
    constexpr float Lightness() const {
        float min, max;
        Bounds(&min, &max, true);
        return (max + min) * 0.5f;
    }

    constexpr Color GammaToLinear() const { return { ConvertGammaToLinear(r), ConvertGammaToLinear(g), ConvertGammaToLinear(b), a }; }
    constexpr Color LinearToGamma() const { return { ConvertLinearToGamma(r), ConvertLinearToGamma(g), ConvertLinearToGamma(b), a }; }

    constexpr void Bounds(float* min, float* max, bool clipped) const {
        assert(min && max);

        if (r > g) {
            if (g > b) {
                // r > g > b
                *max = r;
                *min = b;
            } else {
                // r > g && g <= b
                *max = r > b ? r : b;
                *min = g;
            }
        } else {
            if (b > g) {
                // r <= g < b
                *max = b;
                *min = r;
            } else {
                // r <= g && b <= g
                *max = g;
                *min = r < b ? r : b;
            }
        }

        if (clipped) {
            *max = *max > 1.0f ? 1.0f : (*max < 0.0f ? 0.0f : *max);
            *min = *min > 1.0f ? 1.0f : (*min < 0.0f ? 0.0f : *min);
        }
    }
    constexpr float LargestComponent() const { return std::max({ r, g, b }); }
    constexpr float SmallestComponent() const { return std::min({ r, g, b }); }
    constexpr float ComponentRange() const {
        float min, max;
        Bounds(&min, &max, true);
        return max - min;
    }

    constexpr void Clip(bool clipAlpha) {
        r = std::clamp(r, 0.0f, 1.0f);
        g = std::clamp(g, 0.0f, 1.0f);
        b = std::clamp(b, 0.0f, 1.0f);
        if (clipAlpha) {
            a = std::clamp(a, 0.0f, 1.0f);
        }
    }
    constexpr void Invert(bool invertAlpha) {
        r = 1.0f - r;
        g = 1.0f - g;
        b = 1.0f - b;
        if (invertAlpha) {
            a = 1.0f - a;
        }
    }

    constexpr Color Lerp(const Color& rhs, float t) const {
        float invT = 1.0f - t;
        return Color(
            r * invT + rhs.r * t,
            g * invT + rhs.g * t,
            b * invT + rhs.b * t,
            a * invT + rhs.a * t
        );
    }
    constexpr Color Lighten(float t) const {
        return Lerp(WHITE(), t);
    }
    constexpr Color Darken(float t) const {
        return Lerp(BLACK(), t);
    }

    constexpr Color Abs() const { return { std::abs(r), std::abs(g), std::abs(b), std::abs(a) }; }

    constexpr bool Equals(const Color& rhs) const { return epsilonEquals(r, rhs.r) && epsilonEquals(g, rhs.g) && epsilonEquals(b, rhs.b) && epsilonEquals(a, rhs.a); }

    inline std::string ToString() const { return std::format("Color[{}, {}, {}, {}]", r, g, b, a); }
    constexpr unsigned int ToHash() const { return ToUInt(); }

protected:
    constexpr float Hue(float min, float max) const {
        float chroma = max - min;

        // If chroma equals zero, hue is undefined
        if (chroma <= std::numeric_limits<float>::epsilon())
            return 0.0f;

        // Calculate and return hue
        if (epsilonEquals(g, max))
            return (b + 2.0f * chroma - r) / (6.0f * chroma);
        else if (epsilonEquals(b, max))
            return (4.0f * chroma - g + r) / (6.0f * chroma);
        else {
            float r = (g - b) / (6.0f * chroma);
            return (r < 0.0f) ? 1.0f + r : ((r >= 1.0f) ? r - 1.0f : r);
        }

    }
    constexpr float SaturationHSV(float min, float max) const {
        if (max <= std::numeric_limits<float>::epsilon()) {
            return 0.0f;
        }
        return 1.0f - (min / max);
    }
    constexpr float SaturationHSL(float min, float max) const {
        if (max <= std::numeric_limits<float>::epsilon() || min >= 1.0f - std::numeric_limits<float>::epsilon()) {
            return 0.0f;
        }
        float hl = (max + min);
        if (hl <= 1.0f) {
            return (max - min) / hl;
        } else {
            return (min - max) / (hl - 2.0f);
        }
    }
    inline void FromHCM(float h, float c, float m) {
        if (h < 0.0f || h >= 1.0f) {
            h -= floorf(h);
        }

        float hs = h * 6.0f;
        float x = c * (1.0f - std::abs(fmodf(hs, 2.0f) - 1.0f));

        if (hs < 2.0f) {
            b = 0.0f;
            if (hs < 1.0f) {
                g = x;
                r = c;
            } else {
                g = c;
                r = x;
            }
        } else if (hs < 4.0f) {
            r = 0.0f;
            if (hs < 3.0f) {
                g = c;
                b = x;
            } else {
                g = x;
                b = c;
            }
        } else {
            g = 0.0f;
            if (hs < 5.0f) {
                r = x;
                b = c;
            } else {
                r = c;
                b = x;
            }
        }

        r += m;
        g += m;
        b += m;
    }

private:
    constexpr static bool epsilonEquals(float lhs, float rhs) {
        return lhs + std::numeric_limits<float>::epsilon() >= rhs && lhs - std::numeric_limits<float>::epsilon() <= rhs;
    }
};

namespace std {
    template <>
    struct hash<Color> {
        size_t operator()(const Color& color) const {
            return color.ToHash();
        }
    };
}