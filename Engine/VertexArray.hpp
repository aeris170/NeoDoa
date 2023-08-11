#pragma once

#include <unordered_map>

#include <GL/glew.h>

#include <Engine/Buffer.hpp>
#include <Engine/VertexAttribLayout.hpp>

struct VertexArray {

    VertexArray() noexcept;
    ~VertexArray() noexcept;
    VertexArray(const VertexArray& other) noexcept;
    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(const VertexArray& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void BindVertexAttribBuffer(const Buffer& buffer, const VertexAttribLayout& layout) noexcept;
    void BindVertexAttribBuffer(Buffer&& buffer, VertexAttribLayout&& layout) noexcept;
    void BindElementBuffer(const Buffer& buffer) noexcept;
    void BindElementBuffer(Buffer&& buffer) noexcept;

    void Use() const noexcept;

private:
    GLuint object{};
    std::vector<Buffer> buffers{};
    std::vector<VertexAttribLayout> layouts{};
    Buffer elemBuffer{};

    void AllocateGPU() noexcept;
    void DeallocateGPU() noexcept;
};