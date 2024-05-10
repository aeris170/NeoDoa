#include <Engine/GPUFrameBuffer.hpp>

#include <cassert>

#include <Utility/TemplateUtilities.hpp>

// RenderBuffer
GPURenderBuffer::~GPURenderBuffer() noexcept {
    glDeleteRenderbuffers(1, &GLObjectID);
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

[[nodiscard]] std::pair<std::optional<GPURenderBuffer>, RenderBufferAllocatorMessage> GPURenderBufferBuilder::Build() noexcept {
    GLuint renderBuffer;
    glCreateRenderbuffers(1, &renderBuffer);
    glNamedRenderbufferStorage(renderBuffer, ToGLSizedFormat(format), width, height);

    std::optional<GPURenderBuffer> gpuRenderBuffer{ std::nullopt };
    gpuRenderBuffer.emplace();
    gpuRenderBuffer->GLObjectID = renderBuffer;
#ifdef DEBUG
    gpuRenderBuffer->Name = std::move(name);
#endif
    gpuRenderBuffer->Width = width;
    gpuRenderBuffer->Height = height;
    gpuRenderBuffer->Format = format;

    return { std::move(gpuRenderBuffer), {} };
}

// FrameBuffer
GPUFrameBuffer::~GPUFrameBuffer() noexcept {
    glDeleteFramebuffers(1, &GLObjectID);
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

[[nodiscard]] std::pair<std::optional<GPUFrameBuffer>, FrameBufferAllocatorMessage> GPUFrameBufferBuilder::Build() noexcept {
    GLuint frameBuffer;
    glCreateFramebuffers(1, &frameBuffer);

    unsigned i{};
    for (const auto& colorAttachment : colorAttachments) {
        if (colorAttachment) {
            std::visit(overloaded::lambda{
                [frameBuffer, &i](const GPUTexture& texture) {
                    glNamedFramebufferTexture(frameBuffer, GL_COLOR_ATTACHMENT0 + i, texture.GLObjectID, 0);
                },
                [frameBuffer, &i](const GPURenderBuffer& renderBuffer) {
                    glNamedFramebufferRenderbuffer(frameBuffer, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, renderBuffer.GLObjectID);
                }
            }, colorAttachment.value());
            i++;
        }
    }

    if (depthStencilAttachment) {
        std::visit(overloaded::lambda{
            [frameBuffer](const GPUTexture& texture) {
                glNamedFramebufferTexture(frameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, texture.GLObjectID, 0);
            },
            [frameBuffer](const GPURenderBuffer& renderBuffer) {
                glNamedFramebufferRenderbuffer(frameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GLObjectID);
            }
        }, depthStencilAttachment.value());
    } else {
        if (depthAttachment) {
            std::visit(overloaded::lambda{
                [frameBuffer](const GPUTexture& texture) {
                    glNamedFramebufferTexture(frameBuffer, GL_DEPTH_ATTACHMENT, texture.GLObjectID, 0);
                },
                [frameBuffer](const GPURenderBuffer& renderBuffer) {
                    glNamedFramebufferRenderbuffer(frameBuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GLObjectID);
                }
            }, depthAttachment.value());
        }
        if (stencilAttachment) {
            std::visit(overloaded::lambda{
                [frameBuffer](const GPUTexture& texture) {
                    glNamedFramebufferTexture(frameBuffer, GL_STENCIL_ATTACHMENT, texture.GLObjectID, 0);
                },
                [frameBuffer](const GPURenderBuffer& renderBuffer) {
                    glNamedFramebufferRenderbuffer(frameBuffer, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GLObjectID);
                }
            }, stencilAttachment.value());
        }
    }

    std::optional<GPUFrameBuffer> gpuFrameBuffer{ std::nullopt };
    gpuFrameBuffer.emplace();
    gpuFrameBuffer->GLObjectID = frameBuffer;
#ifdef DEBUG
        gpuFrameBuffer->Name = std::move(name);
#endif
    gpuFrameBuffer->ColorAttachments = std::move(colorAttachments);
    gpuFrameBuffer->DepthAttachment = std::move(depthAttachment);
    gpuFrameBuffer->StencilAttachment = std::move(stencilAttachment);
    gpuFrameBuffer->DepthStencilAttachment = std::move(depthStencilAttachment);

    return { std::move(gpuFrameBuffer), {} };
}

