#pragma once

#include <Engine/Graphics.hpp>
#include <Engine/GPUBuffer.hpp>
#include <Engine/GPUVertexAttribLayout.hpp>

using VertexArrayAllocatorMessage = std::string;

struct GPUVertexArray {

    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{};
#endif
    std::vector<GPUBuffer> ArrayBuffers{};
    std::vector<GPUVertexAttribLayout> Layouts{};
    std::optional<GPUBuffer> ElementBuffer{};
    DataType IndexType{};
    TopologyType Topology{};

    bool HasElementBuffer() const noexcept;

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPUVertexArray);
};

struct GPUVertexArrayBuilder {
    GPUVertexArrayBuilder& SetName(std::string_view name) noexcept;
    GPUVertexArrayBuilder& SetArrayBuffer(const GPUBuffer& buffer, const GPUVertexAttribLayout& layout) noexcept;
    GPUVertexArrayBuilder& SetArrayBuffer(GPUBuffer&& buffer, GPUVertexAttribLayout&& layout) noexcept;
    GPUVertexArrayBuilder& SetElementBuffer(const GPUBuffer& buffer, DataType indexType = DataType::UnsignedShort) noexcept;
    GPUVertexArrayBuilder& SetElementBuffer(GPUBuffer&& buffer, DataType indexType = DataType::UnsignedShort) noexcept;
    GPUVertexArrayBuilder& SetTopology(TopologyType topology) noexcept;

    [[nodiscard]] std::pair<std::optional<GPUVertexArray>, std::vector<VertexArrayAllocatorMessage>> Build() noexcept;

private:
#ifdef DEBUG
    std::string name{};
#endif
    std::vector<GPUBuffer> arrayBuffers{};
    std::vector<GPUVertexAttribLayout> layouts{};
    std::optional<GPUBuffer> elementBuffer{};
    DataType indexType{};
    TopologyType topology{ TopologyType::Triangles };

public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUVertexArrayBuilder);
};