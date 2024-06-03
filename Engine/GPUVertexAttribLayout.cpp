#include <Engine/GPUVertexAttribLayout.hpp>

GPUVertexAttribLayout::~GPUVertexAttribLayout() noexcept = default;
GPUVertexAttribLayout::GPUVertexAttribLayout(const GPUVertexAttribLayout&) noexcept = default;
GPUVertexAttribLayout::GPUVertexAttribLayout(GPUVertexAttribLayout&&) noexcept = default;
GPUVertexAttribLayout& GPUVertexAttribLayout::operator=(const GPUVertexAttribLayout&) noexcept = default;
GPUVertexAttribLayout& GPUVertexAttribLayout::operator=(GPUVertexAttribLayout&&) noexcept = default;

template<>
void GPUVertexAttribLayout::Define<float_t>(unsigned count, bool isNormalized) {
    static unsigned size{ sizeof(float_t) };
    Elements[AttribCount++] = { GL_FLOAT, count, isNormalized };
    Offsets[AttribCount] = Offsets[AttribCount - 1] + count * size;
    Stride += count * size;
}

template<>
void GPUVertexAttribLayout::Define<double_t>(unsigned count, bool isNormalized) {
    static unsigned size{ sizeof(double_t) };
    Elements[AttribCount++] = { GL_DOUBLE, count, isNormalized };
    Offsets[AttribCount] = Offsets[AttribCount - 1] + count * size;
    Stride += count * size;
}

template<>
void GPUVertexAttribLayout::Define<int8_t>(unsigned count, bool isNormalized) {
    static unsigned size{ sizeof(int8_t) };
    Elements[AttribCount++] = { GL_BYTE, count, isNormalized };
    Offsets[AttribCount] = Offsets[AttribCount - 1] + count * size;
    Stride += count * size;
}

template<>
void GPUVertexAttribLayout::Define<uint8_t>(unsigned count, bool isNormalized) {
    static unsigned size{ sizeof(uint8_t) };
    Elements[AttribCount++] = { GL_UNSIGNED_BYTE, count, isNormalized };
    Offsets[AttribCount] = Offsets[AttribCount - 1] + count * size;
    Stride += count * size;
}

template<>
void GPUVertexAttribLayout::Define<int16_t>(unsigned count, bool isNormalized) {
    static unsigned size{ sizeof(int16_t) };
    Elements[AttribCount++] = { GL_SHORT, count, isNormalized };
    Offsets[AttribCount] = Offsets[AttribCount - 1] + count * size;
    Stride += count * size;
}

template<>
void GPUVertexAttribLayout::Define<uint16_t>(unsigned count, bool isNormalized) {
    static unsigned size{ sizeof(uint16_t) };
    Elements[AttribCount++] = { GL_UNSIGNED_SHORT, count, isNormalized };
    Offsets[AttribCount] = Offsets[AttribCount - 1] + count * size;
    Stride += count * size;
}

template<>
void GPUVertexAttribLayout::Define<int32_t>(unsigned count, bool isNormalized) {
    static unsigned size{ sizeof(int32_t) };
    Elements[AttribCount++] = { GL_INT, count, isNormalized };
    Offsets[AttribCount] = Offsets[AttribCount - 1] + count * size;
    Stride += count * size;
}

template<>
void GPUVertexAttribLayout::Define<uint32_t>(unsigned count, bool isNormalized) {
    static unsigned size{ sizeof(uint32_t) };
    Elements[AttribCount++] = { GL_UNSIGNED_INT, count, isNormalized };
    Offsets[AttribCount] = Offsets[AttribCount - 1] + count * size;
    Stride += count * size;
}