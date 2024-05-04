#include <Engine/VertexAttribLayout.hpp>
/*
template<>
void VertexAttribLayout::Define<float_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(float_t) };
    elements.emplace_back(GL_FLOAT, count, isNormalized);
    offsets.emplace_back(offsets.back() + count * size);
    stride += static_cast<GLuint>(count * size);
}

template<>
void VertexAttribLayout::Define<double_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(double_t) };
    elements.emplace_back(GL_DOUBLE, count, isNormalized);
    offsets.emplace_back(offsets.back() + count * size);
    stride += static_cast<GLuint>(count * size);
}

template<>
void VertexAttribLayout::Define<int8_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(int8_t) };
    elements.emplace_back(GL_BYTE, count, isNormalized);
    offsets.emplace_back(offsets.back() + count * size);
    stride += static_cast<GLuint>(count * size);
}

template<>
void VertexAttribLayout::Define<uint8_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(uint8_t) };
    elements.emplace_back(GL_UNSIGNED_BYTE, count, isNormalized);
    offsets.emplace_back(offsets.back() + count * size);
    stride += static_cast<GLuint>(count * size);
}

template<>
void VertexAttribLayout::Define<int16_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(int16_t) };
    elements.emplace_back(GL_SHORT, count, isNormalized);
    offsets.emplace_back(offsets.back() + count * size);
    stride += static_cast<GLuint>(count * size);
}

template<>
void VertexAttribLayout::Define<uint16_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(uint16_t) };
    elements.emplace_back(GL_UNSIGNED_SHORT, count, isNormalized);
    offsets.emplace_back(offsets.back() + count * size);
    stride += static_cast<GLuint>(count * size);
}

template<>
void VertexAttribLayout::Define<int32_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(int32_t) };
    elements.emplace_back(GL_INT, count, isNormalized);
    offsets.emplace_back(offsets.back() + count * size);
    stride += static_cast<GLuint>(count * size);
}

template<>
void VertexAttribLayout::Define<uint32_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(uint32_t) };
    elements.emplace_back(GL_UNSIGNED_INT, count, isNormalized);
    offsets.emplace_back(offsets.back() + count * size);
    stride += static_cast<GLuint>(count * size);
}*/