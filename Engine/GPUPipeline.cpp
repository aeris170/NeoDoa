#include <Engine/GPUPipeline.hpp>

#include <Engine/GPUBuffer.hpp>

#include <cassert>

GPUPipeline::~GPUPipeline() noexcept {
    Graphics::Destructors::Destruct(*this);
}
GPUPipeline::GPUPipeline(GPUPipeline&& other) noexcept {
    *this = std::move(other);
}
GPUPipeline& GPUPipeline::operator=(GPUPipeline&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    VertexBuffers = std::move(other.VertexBuffers);
    VertexLayouts = std::move(other.VertexLayouts);
    IndexBuffer = other.IndexBuffer;
    IndexType = other.IndexType;
    Topology = other.Topology;
    Polygon = other.Polygon;
    IsFaceCullingEnabled = other.IsFaceCullingEnabled;
    Cull = other.Cull;
    Viewport = other.Viewport;
    IsScissorEnabled = other.IsScissorEnabled;
    Scissor = other.Scissor;
    IsDepthTestEnabled = other.IsDepthTestEnabled;
    IsDepthWriteEnabled = other.IsDepthWriteEnabled;
    DepthFunc = other.DepthFunc;
    IsDepthClampEnabled = other.IsDepthClampEnabled;
    IsMultisampleEnabled = other.IsMultisampleEnabled;
    IsBlendEnabled = other.IsBlendEnabled;
    SourceFactor = other.SourceFactor;
    DestinationFactor = other.DestinationFactor;
    SourceAlphaFactor = other.SourceAlphaFactor;
    DestinationAlphaFactor = other.DestinationAlphaFactor;
    ShaderProgram = std::move(other.ShaderProgram);
    return *this;
}

GPUPipelineBuilder& GPUPipelineBuilder::SetName(std::string_view name) noexcept {
#ifdef DEBUG
    this->name = name;
#endif
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetArrayBuffer(uint8_t binding, const GPUBuffer& buffer, const GPUVertexAttribLayout& layout) noexcept {
    assert(binding < MaxVertexBufferBinding);
    vertexBuffers[binding].emplace(buffer);
    vertexLayouts[binding] = layout;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetIndexBuffer(const GPUBuffer& buffer, DataType indexType) noexcept {
    indexBuffer = &buffer;
    this->indexType = indexType;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetTopology(TopologyType topology) noexcept {
    this->topology = topology;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetPolygonMode(PolygonMode polygonMode) noexcept {
    this->polygonMode = polygonMode;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetFaceCullEnabled(bool enabled) noexcept {
    this->isFaceCullingEnabled = enabled;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetCullMode(CullMode cullMode) noexcept {
    this->cullMode = cullMode;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetViewport(Region viewport) noexcept {
    this->viewport = viewport;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetScissorTestEnabled(bool enabled) noexcept {
    isScissorEnabled = enabled;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetScissorRegion(Region scissor) noexcept {
    this->scissor = scissor;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetDepthTestEnabled(bool enabled) noexcept {
    isDepthTestEnabled = enabled;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetDepthWriteEnabled(bool enabled) noexcept {
    isDepthWriteEnabled = enabled;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetDepthFunc(DepthFunction depthFunction) noexcept {
    this->depthFunction = depthFunction;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetDepthClampEnabled(bool enabled) noexcept {
    isDepthClampEnabled = enabled;
    return *this;
}

GPUPipelineBuilder& GPUPipelineBuilder::SetMultisampleEnabled(bool enabled) noexcept {
    isMultisampleEnabled = enabled;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetBlendEnabled(bool enabled) noexcept {
    isBlendEnabled = enabled;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetBlendFunction(BlendFactor srcFactor, BlendFactor dstFactor) noexcept {
    srcRGBFactor = srcFactor;
    dstRGBFactor = dstFactor;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetBlendFunctionSeparate(BlendFactor srcRGBFactor, BlendFactor dstRGBFactor, BlendFactor srcAlphaFactor, BlendFactor dstAlphaFactor) noexcept {
    this->srcRGBFactor = srcRGBFactor;
    this->dstRGBFactor = dstRGBFactor;
    this->srcAlphaFactor = srcAlphaFactor;
    this->dstAlphaFactor = dstAlphaFactor;
    return *this;
}
GPUPipelineBuilder& GPUPipelineBuilder::SetShaderProgram(const GPUShaderProgram& program) noexcept {
    shaderProgam.emplace(program);
    return *this;
}

std::pair<std::optional<GPUPipeline>, std::vector<PipelineAllocatorMessage>> GPUPipelineBuilder::Build() noexcept {
    return Graphics::Builders::Build(*this);
}