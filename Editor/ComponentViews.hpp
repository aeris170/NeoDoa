#pragma once

#include "NeoDoa.hpp"

inline int X = 0b0001;
inline int Y = 0b0010;
inline int Z = 0b0100;
inline int W = 0b1000;

inline constexpr int compFieldWidth = 276; // must be divisible by both 3 and 4 (and 1 and 2, but you know... MaThS...)

void FancyVectorWidget(const char* label, glm::vec2& vec, float reset = 0, int flags = X | Y);
void FancyVectorWidget(const char* label, glm::vec3& vec, float reset = 0, int flags = X | Y | Z);
void FancyVectorWidget(const char* label, glm::vec4& vec, float reset = 0, int flags = X | Y | Z | W);
void FancyVectorWidget(const char* label, float* vec, float reset = 0, int flags = X | Y | Z | W);

static void BeginWidget(const char* label);
static void EndWidget();

void EnumWidget(const char* label, int& value, const std::vector<EnumValue>& values);

void IntWidget(const char* label, int& value);

void FloatWidget(const char* label, float& value);

void DoubleWidget(const char* label, double& value);

void StringWidget(const char* label, std::string& value);

void UneditableStringWidget(const char* label, const std::string& value);
