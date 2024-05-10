#pragma once

#include <cmath>
#include <vector>

#include <Engine/Log.hpp>
#include <Engine/Graphics.hpp>

struct GPUVertexAttribLayout {

    struct Element {
        GLuint Type{};
        GLuint Count{};
        GLboolean IsNormalized{};
    };

    GLuint Stride{};
    GLuint Divisor{};
    std::vector<Element> Elements{};
    std::vector<GLuint> Offsets{ 0u };

    void Divide(GLuint divisor) noexcept;

    template<typename T>
    void Define([[maybe_unused]] size_t count, [[maybe_unused]] bool isNormalized = false) { DOA_LOG_FATAL("Unsupported Vertex attrib!"); std::unreachable(); }

    ND_GRAPHICS_COPYABLE_MOVEABLE_RESOURCE(GPUVertexAttribLayout);

private:
    friend struct GPUVertexArray;
};

template<>
void GPUVertexAttribLayout::Define<float_t>(size_t count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<double_t>(size_t count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<int8_t>(size_t count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<uint8_t>(size_t count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<int16_t>(size_t count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<uint16_t>(size_t count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<int32_t>(size_t count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<uint32_t>(size_t count, bool isNormalized);