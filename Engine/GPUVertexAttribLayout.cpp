#include <Engine/GPUVertexAttribLayout.hpp>

GPUVertexAttribLayout::~GPUVertexAttribLayout() noexcept = default;
GPUVertexAttribLayout::GPUVertexAttribLayout(const GPUVertexAttribLayout&) noexcept = default;
GPUVertexAttribLayout::GPUVertexAttribLayout(GPUVertexAttribLayout&&) noexcept = default;
GPUVertexAttribLayout& GPUVertexAttribLayout::operator=(const GPUVertexAttribLayout&) noexcept = default;
GPUVertexAttribLayout& GPUVertexAttribLayout::operator=(GPUVertexAttribLayout&&) noexcept = default;

template<>
void GPUVertexAttribLayout::Define<float_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(float_t) };
    Elements.emplace_back(GL_FLOAT, count, isNormalized);
    Offsets.emplace_back(Offsets.back() + count * size);
    Stride += static_cast<GLuint>(count * size);
}

template<>
void GPUVertexAttribLayout::Define<double_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(double_t) };
    Elements.emplace_back(GL_DOUBLE, count, isNormalized);
    Offsets.emplace_back(Offsets.back() + count * size);
    Stride += static_cast<GLuint>(count * size);
}

template<>
void GPUVertexAttribLayout::Define<int8_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(int8_t) };
    Elements.emplace_back(GL_BYTE, count, isNormalized);
    Offsets.emplace_back(Offsets.back() + count * size);
    Stride += static_cast<GLuint>(count * size);
}

template<>
void GPUVertexAttribLayout::Define<uint8_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(uint8_t) };
    Elements.emplace_back(GL_UNSIGNED_BYTE, count, isNormalized);
    Offsets.emplace_back(Offsets.back() + count * size);
    Stride += static_cast<GLuint>(count * size);
}

template<>
void GPUVertexAttribLayout::Define<int16_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(int16_t) };
    Elements.emplace_back(GL_SHORT, count, isNormalized);
    Offsets.emplace_back(Offsets.back() + count * size);
    Stride += static_cast<GLuint>(count * size);
}

template<>
void GPUVertexAttribLayout::Define<uint16_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(uint16_t) };
    Elements.emplace_back(GL_UNSIGNED_SHORT, count, isNormalized);
    Offsets.emplace_back(Offsets.back() + count * size);
    Stride += static_cast<GLuint>(count * size);
}

template<>
void GPUVertexAttribLayout::Define<int32_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(int32_t) };
    Elements.emplace_back(GL_INT, count, isNormalized);
    Offsets.emplace_back(Offsets.back() + count * size);
    Stride += static_cast<GLuint>(count * size);
}

template<>
void GPUVertexAttribLayout::Define<uint32_t>(size_t count, bool isNormalized) {
    static size_t size{ sizeof(uint32_t) };
    Elements.emplace_back(GL_UNSIGNED_INT, count, isNormalized);
    Offsets.emplace_back(Offsets.back() + count * size);
    Stride += static_cast<GLuint>(count * size);
}