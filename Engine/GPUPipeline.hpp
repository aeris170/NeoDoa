#pragma once

#include <array>
#include <vector>
#include <utility>
#include <optional>
#include <string_view>
#include <type_traits>

#include <Engine/Region.hpp>
#include <Engine/Graphics.hpp>
#include <Engine/GPUVertexAttribLayout.hpp>

struct GPUPipeline {

    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{};
#endif
    std::array<std::optional<std::reference_wrapper<const GPUBuffer>>, MaxVertexBufferBinding> VertexBuffers{};
    std::array<GPUVertexAttribLayout, MaxVertexBufferBinding> VertexLayouts{};
    const GPUBuffer* IndexBuffer{};
    DataType IndexType{};

    TopologyType Topology{};
    PolygonMode Polygon{};

    bool IsFaceCullingEnabled{};
    CullMode Cull{};
    Region Viewport{};

    bool IsScissorEnabled{};
    Region Scissor{};

    bool IsDepthTestEnabled{};
    bool IsDepthWriteEnabled{};
    DepthFunction DepthFunc{};
    bool IsDepthClampEnabled{};

    bool IsMultisampleEnabled{};

    bool IsBlendEnabled{};
    BlendFactor SourceFactor{}, DestinationFactor{};
    BlendFactor SourceAlphaFactor{}, DestinationAlphaFactor{};

    std::optional<std::reference_wrapper<const GPUShaderProgram>> ShaderProgram{};

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPUPipeline);
};

struct GPUPipelineBuilder {

    GPUPipelineBuilder& SetName(std::string_view name) noexcept;
    GPUPipelineBuilder& SetArrayBuffer(uint8_t binding, const GPUBuffer& buffer, const GPUVertexAttribLayout& layout) noexcept;
    GPUPipelineBuilder& SetIndexBuffer(const GPUBuffer& buffer, DataType indexType = DataType::UnsignedShort) noexcept;
    GPUPipelineBuilder& SetTopology(TopologyType topology) noexcept;
    GPUPipelineBuilder& SetPolygonMode(PolygonMode polygonMode) noexcept;
    GPUPipelineBuilder& SetFaceCullEnabled(bool enabled) noexcept;
    GPUPipelineBuilder& SetCullMode(CullMode cullMode) noexcept;
    GPUPipelineBuilder& SetViewport(Region region) noexcept;
    GPUPipelineBuilder& SetScissorTestEnabled(bool enabled) noexcept;
    GPUPipelineBuilder& SetScissorRegion(Region scissor) noexcept;
    GPUPipelineBuilder& SetDepthTestEnabled(bool enabled) noexcept;
    GPUPipelineBuilder& SetDepthWriteEnabled(bool enabled) noexcept;
    GPUPipelineBuilder& SetDepthFunc(DepthFunction depthFunction) noexcept;
    GPUPipelineBuilder& SetDepthClampEnabled(bool enabled) noexcept;
    //GPUPipelineBuilder& SetStencilTestEnabled(bool enabled);
    //GPUPipelineBuilder& SetStencilWriteEnabled(bool enabled);
    GPUPipelineBuilder& SetMultisampleEnabled(bool enabled) noexcept;
    GPUPipelineBuilder& SetBlendEnabled(bool enabled) noexcept;
    GPUPipelineBuilder& SetBlendFunction(BlendFactor srcFactor, BlendFactor dstFactor) noexcept;
    GPUPipelineBuilder& SetBlendFunctionSeparate(BlendFactor srcRGBFactor, BlendFactor dstRGBFactor, BlendFactor srcAlphaFactor, BlendFactor dstAlphaFactor) noexcept;
    GPUPipelineBuilder& SetShaderProgram(const GPUShaderProgram& program) noexcept;

    [[nodiscard]] std::pair<std::optional<GPUPipeline>, std::vector<PipelineAllocatorMessage>> Build() noexcept;

    GPUPipelineBuilder& SetArrayBuffer(uint8_t binding, GPUBuffer&& buffer, const GPUVertexAttribLayout& layout) noexcept = delete;
    GPUPipelineBuilder& SetIndexBuffer(GPUBuffer&& buffer, DataType indexType = DataType::UnsignedShort) noexcept = delete;
    GPUPipelineBuilder& SetShaderProgram(GPUShaderProgram&& program) noexcept = delete;

private:
#ifdef DEBUG
    std::string name{};
#endif
    std::array<std::optional<std::reference_wrapper<const GPUBuffer>>, MaxVertexBufferBinding> vertexBuffers{};
    std::array<GPUVertexAttribLayout, MaxVertexBufferBinding> vertexLayouts{};
    const GPUBuffer* indexBuffer{};
    DataType indexType{};

    TopologyType topology{ TopologyType::Triangles };
    PolygonMode polygonMode{ PolygonMode::Fill };

    bool isFaceCullingEnabled{};
    CullMode cullMode{ CullMode::Back };
    Region viewport{};

    bool isScissorEnabled{};
    Region scissor{};

    bool isDepthTestEnabled{};
    bool isDepthWriteEnabled{};
    DepthFunction depthFunction{ DepthFunction::Less };
    bool isDepthClampEnabled{};

    bool isMultisampleEnabled{};

    bool isBlendEnabled{};
    BlendFactor srcRGBFactor{ BlendFactor::One };
    BlendFactor dstRGBFactor{ BlendFactor::Zero };
    BlendFactor srcAlphaFactor{ srcRGBFactor };
    BlendFactor dstAlphaFactor{ dstRGBFactor };

    std::optional<std::reference_wrapper<const GPUShaderProgram>> shaderProgam{};

public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUPipelineBuilder);

private:
#ifdef OPENGL_4_6_SUPPORT
    friend std::pair<std::optional<GPUPipeline>, std::vector<PipelineAllocatorMessage>> Graphics::OpenGL::Build(GPUPipelineBuilder&) noexcept;
#endif
};