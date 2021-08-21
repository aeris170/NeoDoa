#pragma once

#include <string>

#include <glm/glm.hpp>

struct Color {

	static const Color WHITE;
	static const Color GRAY;
	static const Color BLACK;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color CYAN;
	static const Color MAGENTA;
	static const Color YELLOW;
	static const Color ZERO;

	static float ConvertGammaToLinear(float value);
	static float ConvertLinearToGamma(float value);

	float r, g, b, a;

	Color() noexcept;
	Color(const Color& color) noexcept = default;
	Color(const Color& color, float a) noexcept;
	Color(float v) noexcept;
	Color(float r, float g, float b) noexcept;
	Color(float r, float g, float b, float a) noexcept;
	explicit Color(const float* data) noexcept;
	explicit Color(const glm::vec3& color) noexcept;
	explicit Color(const glm::vec4& color) noexcept;

	Color& operator=(const Color& rhs) noexcept = default;
	bool operator==(const Color& rhs) const;
	bool operator!=(const Color& rhs) const;
	Color operator*(float rhs) const;
	Color operator+(const Color& rhs) const;
	Color operator-() const;
	Color operator-(const Color& rhs) const;
	Color& operator+=(const Color& rhs);

	float* Data();
	const float* Data() const;

	unsigned int ToUInt() const;
	glm::vec3 ToHSL() const;
	glm::vec3 ToHSV() const;

	void FromUInt(unsigned color);
	void FromHSL(float h, float s, float l, float a = 1.0f);
	void FromHSV(float h, float s, float v, float a = 1.0f);

	glm::vec3 ToVec3() const;
	glm::vec4 ToVec4() const;

	float SumRGB() const;
	float Average() const;

	float Luma() const; // greyscale component

	float Chroma() const;
	float Hue() const;
	float SaturationHSL() const;
	float SaturationHSV() const;
	float Value() const;
	float Lightness() const;

	Color GammaToLinear() const;
	Color LinearToGamma() const;

	void Bounds(float* min, float* max, bool clipped = false) const;
	float LargestComponent() const;
	float SmallestComponent() const;
	float ComponentRange() const;

	void Clip(bool clipAlpha = false);
	void Invert(bool invertAlpha = false);

	Color Lerp(const Color& rhs, float t) const;

	Color Abs() const;

	bool Equals(const Color& rhs) const;

	std::string ToString() const;
	unsigned int ToHash() const;

protected:
	float Hue(float min, float max) const;
	float SaturationHSV(float min, float max) const;
	float SaturationHSL(float min, float max) const;
	void FromHCM(float h, float c, float m);
};

namespace std {
	template <>
	struct hash<Color> {
		size_t operator()(const Color& color) const {
			return color.ToHash();
		}
	};
}