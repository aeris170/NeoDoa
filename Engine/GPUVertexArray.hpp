#pragma once

#include <Engine/Graphics.hpp>
#include <Engine/GPUBuffer.hpp>
#include <Engine/GPUVertexAttribLayout.hpp>

using VertexArrayAllocatorMessage = std::string;

struct GPUVertexArray {

    GLuint GLObjectID{};
    std::string Name{};
    std::vector<GPUBuffer> ArrayBuffers{};
    std::vector<GPUVertexAttribLayout> Layouts{};
    std::optional<GPUBuffer> ElementBuffer{};

    bool HasElementBuffer() const noexcept;

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPUVertexArray);
};

struct GPUVertexArrayBuilder {
    GPUVertexArrayBuilder& SetName(std::string_view name) noexcept;
    GPUVertexArrayBuilder& SetArrayBuffer(const GPUBuffer& buffer, const GPUVertexAttribLayout& layout) noexcept;
    GPUVertexArrayBuilder& SetArrayBuffer(GPUBuffer&& buffer, GPUVertexAttribLayout&& layout) noexcept;
    GPUVertexArrayBuilder& SetElementBuffer(const GPUBuffer& buffer) noexcept;
    GPUVertexArrayBuilder& SetElementBuffer(GPUBuffer&& buffer) noexcept;

    [[nodiscard]] std::pair<std::optional<GPUVertexArray>, std::vector<VertexArrayAllocatorMessage>> Build() noexcept;

private:
    std::string name{};
    std::vector<GPUBuffer> arrayBuffers{};
    std::vector<GPUVertexAttribLayout> layouts{};
    std::optional<GPUBuffer> elementBuffer{};

public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUVertexArrayBuilder);
};