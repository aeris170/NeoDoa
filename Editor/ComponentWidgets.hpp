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

bool FancyVectorWidget(const std::string& label, glm::vec2& vec, float reset = 0.0f, int flags = X | Y);
bool FancyVectorWidget(const std::string& label, glm::vec3& vec, float reset = 0.0f, int flags = X | Y | Z);
bool FancyVectorWidget(const std::string& label, glm::vec4& vec, float reset = 0.0f, int flags = X | Y | Z | W);
bool FancyVectorWidget(const std::string& label, float* vec, float reset = 0.0f, int flags = X | Y | Z | W);

namespace detail {
	void BeginWidget(const std::string& label);
	void EndWidget();
}

bool EnumWidget(const std::string& label, int& value, const std::vector<EnumValue>& values);

bool EntityWidget(const std::string& label, Entity& value);

void UneditableEntityWidget(const std::string& label, const Entity value);

bool IntWidget(const std::string& label, int& value);

bool FloatWidget(const std::string& label, float& value, float step = 0.1f, float min = 0.0f, float max = 0.0f);

bool DoubleWidget(const std::string& label, double& value);

bool BoolWidget(const std::string& label, bool& value);

bool StringWidget(const std::string& label, std::string& value);

void UneditableStringWidget(const std::string& label, const std::string& value);

bool ColorWidget(const std::string& label, Color& value);

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