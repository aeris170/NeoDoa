#pragma once

#include <cmath>
#include <vector>

#include <GL/glew.h>

struct VertexAttribLayout {

    struct Element {
        GLuint type{};
        GLuint count{};
        GLboolean isNormalized{};
    };

    template<typename T>
    void Define([[maybe_unused]] size_t count, [[maybe_unused]] bool isNormalized = false) {}

private:
    GLuint stride{};
    std::vector<Element> elements{};
    std::vector<GLuint> offsets{ 0 };

    friend struct VertexArray;
};
/*
template<>
void VertexAttribLayout::Define<float_t>(size_t count, bool isNormalized);
template<>
void VertexAttribLayout::Define<double_t>(size_t count, bool isNormalized);
template<>
void VertexAttribLayout::Define<int8_t>(size_t count, bool isNormalized);
template<>
void VertexAttribLayout::Define<uint8_t>(size_t count, bool isNormalized);
template<>
void VertexAttribLayout::Define<int16_t>(size_t count, bool isNormalized);
template<>
void VertexAttribLayout::Define<uint16_t>(size_t count, bool isNormalized);
template<>
void VertexAttribLayout::Define<int32_t>(size_t count, bool isNormalized);
template<>
void VertexAttribLayout::Define<uint32_t>(size_t count, bool isNormalized);
*/