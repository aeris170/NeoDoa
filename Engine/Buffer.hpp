#pragma once

#include <span>
#include <vector>

#include <Engine/OpenGLCommon.hpp>

struct Buffer {

    template<typename T>
    static OpenGL::BufferData Accept(std::vector<T> values) {
        return Accept(std::span{ values });
    }

    template<typename T>
    static OpenGL::BufferData Accept(std::span<T> values) {
        auto bytes{ std::as_bytes(values) };
        return { bytes.begin(), bytes.end() };
    }

    Buffer() noexcept = default;
    explicit Buffer(const OpenGL::BufferData& data) noexcept;
    explicit Buffer(OpenGL::BufferData&& data) noexcept;
    ~Buffer() noexcept;
    Buffer(const Buffer& other) noexcept;
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(const Buffer& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    bool HasData() const;
    const OpenGL::BufferData& GetData() const;
    OpenGL::BufferData::const_iterator begin() const;
    OpenGL::BufferData::const_iterator end() const;

private:

    GLuint object{};
    OpenGL::BufferData data{};

    void AllocateGPU() noexcept;
    void DeallocateGPU() noexcept;

    friend struct VertexArray;
};

