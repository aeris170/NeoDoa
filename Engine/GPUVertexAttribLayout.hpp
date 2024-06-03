#pragma once

#include <cmath>
#include <vector>

#include <Engine/Log.hpp>
#include <Engine/Graphics.hpp>

struct GPUVertexAttribLayout {

    struct Element {
        unsigned Type{};
        unsigned Count{};
        bool IsNormalized{};
    };

    unsigned Stride{};
    InputRate InputRate{ InputRate::PerVertex };
    unsigned AttribCount{};
    std::array<Element, 16> Elements{};
    std::array<unsigned, 16> Offsets{ 0u };

    template<typename T>
    void Define([[maybe_unused]] unsigned count, [[maybe_unused]] bool isNormalized = false) { DOA_LOG_FATAL("Unsupported Vertex attrib!"); std::unreachable(); }

    ND_GRAPHICS_COPYABLE_MOVEABLE_RESOURCE(GPUVertexAttribLayout);

private:
    friend struct GPUVertexArray;
};

template<>
void GPUVertexAttribLayout::Define<float_t>(unsigned count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<double_t>(unsigned count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<int8_t>(unsigned count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<uint8_t>(unsigned count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<int16_t>(unsigned count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<uint16_t>(unsigned count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<int32_t>(unsigned count, bool isNormalized);
template<>
void GPUVertexAttribLayout::Define<uint32_t>(unsigned count, bool isNormalized);