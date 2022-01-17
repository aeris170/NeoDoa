#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "NeoDoa.hpp"

inline int X = 0b0001;
inline int Y = 0b0010;
inline int Z = 0b0100;
inline int W = 0b1000;

inline constexpr int compFieldWidth = 276; // must be divisible by both 3 and 4 (and 1 and 2, but you know... MaThS...) (x,y,z,w)

void FancyVectorWidget(const std::string& label, glm::vec2& vec, float reset = 0.0f, int flags = X | Y);
void FancyVectorWidget(const std::string& label, glm::vec3& vec, float reset = 0.0f, int flags = X | Y | Z);
void FancyVectorWidget(const std::string& label, glm::vec4& vec, float reset = 0.0f, int flags = X | Y | Z | W);
void FancyVectorWidget(const std::string& label, float* vec, float reset = 0.0f, int flags = X | Y | Z | W);

namespace detail {
	void BeginWidget(const std::string& label);
	void EndWidget();
}

void EnumWidget(const std::string& label, int& value, const std::vector<EnumValue>& values);

void EntityWidget(const std::string& label, Entity& value);

void UneditableEntityWidget(const std::string& label, const Entity value);

void IntWidget(const std::string& label, int& value);

void FloatWidget(const std::string& label, float& value);

void DoubleWidget(const std::string& label, double& value);

void BoolWidget(const std::string& label, bool& value);

void StringWidget(const std::string& label, std::string& value);

void UneditableStringWidget(const std::string& label, const std::string& value);

void ColorWidget(const std::string& label, Color& value);

using namespace detail;

template<typename T>
requires requires (T x) { std::to_string(x); }
void UneditableArrayWidget(const std::string& label, const std::vector<T>& array) {
	BeginWidget(label);
	for (int i = 0; i < array.size(); i++) {
		ImGui::Text(std::to_string(array[i]).c_str());
	}
	EndWidget();
}