#include <Engine/GPUFrameBuffer.hpp>

#include <cassert>

// RenderBuffer
GPURenderBuffer::~GPURenderBuffer() noexcept {
    Graphics::Destructors::Destruct(*this);
}
GPURenderBuffer::GPURenderBuffer(GPURenderBuffer&& other) noexcept {
    *this = std::move(other);
}
GPURenderBuffer& GPURenderBuffer::operator=(GPURenderBuffer&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    Width = std::exchange(other.Width, {});
    Height = std::exchange(other.Height, {});
    Format = std::exchange(other.Format, {});
    Samples = std::exchange(other.Samples, {});
    return *this;
}

GPURenderBufferBuilder& GPURenderBufferBuilder::SetName(std::string_view name) noexcept {
#ifdef DEBUG
    this->name = name;
#endif
    return *this;
}
GPURenderBufferBuilder& GPURenderBufferBuilder::SetWidth(unsigned width) noexcept {
    this->width = width;
    return *this;
}
GPURenderBufferBuilder& GPURenderBufferBuilder::SetHeight(unsigned height) noexcept {
    this->height = height;
    return *this;
}
GPURenderBufferBuilder& GPURenderBufferBuilder::SetFormat(DataFormat format) noexcept {
    this->format = format;
    return *this;
}
GPURenderBufferBuilder& GPURenderBufferBuilder::SetLayout(unsigned width, unsigned height, DataFormat format) noexcept {
    SetWidth(width).SetHeight(height).SetFormat(format);
    return *this;
}
GPURenderBufferBuilder& GPURenderBufferBuilder::SetSamples(Multisample multisample) noexcept {
    samples = multisample;
    return *this;
}

std::pair<std::optional<GPURenderBuffer>, std::vector<RenderBufferAllocatorMessage>> GPURenderBufferBuilder::Build() noexcept {
    return Graphics::Builders::Build(*this);
}

// FrameBuffer
GPUFrameBuffer::~GPUFrameBuffer() noexcept {
    Graphics::Destructors::Destruct(*this);
}
GPUFrameBuffer::GPUFrameBuffer(GPUFrameBuffer&& other) noexcept {
    *this = std::move(other);
}
GPUFrameBuffer& GPUFrameBuffer::operator=(GPUFrameBuffer&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    std::swap(ColorAttachments, other.ColorAttachments);
    std::swap(DepthAttachment, other.DepthAttachment);
    std::swap(StencilAttachment, other.StencilAttachment);
    std::swap(DepthStencilAttachment, other.DepthStencilAttachment);
    return *this;
}

GPUFrameBufferBuilder& GPUFrameBufferBuilder::SetName(std::string_view name) noexcept {
#ifdef DEBUG
    this->name = name;
#endif
    return *this;
}
GPUFrameBufferBuilder& GPUFrameBufferBuilder::AttachColorTexture(GPUTexture&& texture, unsigned index) noexcept {
    assert(index < colorAttachments.size());
    assert(texture.Depth == 1); // Only 2D textures are allowed!
    colorAttachments[index].emplace(std::move(texture));
    return *this;
}
GPUFrameBufferBuilder& GPUFrameBufferBuilder::AttachDepthTexture(GPUTexture&& texture) noexcept {
    assert(texture.Depth == 1); // Only 2D textures are allowed!
    depthAttachment.emplace(std::move(texture));
    return *this;
}
GPUFrameBufferBuilder& GPUFrameBufferBuilder::AttachStencilTexture(GPUTexture&& texture) noexcept {
    assert(texture.Depth == 1); // Only 2D textures are allowed!
    stencilAttachment.emplace(std::move(texture));
    return *this;
}
GPUFrameBufferBuilder& GPUFrameBufferBuilder::AttachDepthStencilTexture(GPUTexture&& texture) noexcept {
    assert(texture.Depth == 1); // Only 2D textures are allowed!
    depthStencilAttachment.emplace(std::move(texture));
    return *this;
}
GPUFrameBufferBuilder& GPUFrameBufferBuilder::AttachColorRenderBuffer(GPURenderBuffer&& renderBuffer, unsigned index) noexcept {
    assert(index < colorAttachments.size());
    colorAttachments[index].emplace(std::move(renderBuffer));
    return *this;
}
GPUFrameBufferBuilder& GPUFrameBufferBuilder::AttachDepthRenderBuffer(GPURenderBuffer&& renderBuffer) noexcept {
    depthAttachment.emplace(std::move(renderBuffer));
    return *this;
}
GPUFrameBufferBuilder& GPUFrameBufferBuilder::AttachStencilRenderBuffer(GPURenderBuffer&& renderBuffer) noexcept {
    stencilAttachment.emplace(std::move(renderBuffer));
    return *this;
}
GPUFrameBufferBuilder& GPUFrameBufferBuilder::AttachDepthStencilRenderBuffer(GPURenderBuffer&& renderBuffer) noexcept {
    depthStencilAttachment.emplace(std::move(renderBuffer));
    return *this;
}

std::pair<std::optional<GPUFrameBuffer>, std::vector<FrameBufferAllocatorMessage>> GPUFrameBufferBuilder::Build() noexcept {
    return Graphics::Builders::Build(*this);
}

