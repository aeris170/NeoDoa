#include <Engine/Graphics.hpp>

#include <cassert>

#include <Utility/TemplateUtilities.hpp>

#include <Engine/Region.hpp>
#include <Engine/GPUFrameBuffer.hpp>
#include <Engine/GPUPipeline.hpp>

void Graphics::Blit(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    bool blitColor = !source.ColorAttachments.empty() && !destination.ColorAttachments.empty();
    bool blitDepth =
        (source.DepthAttachment.has_value() || source.DepthStencilAttachment.has_value()) &&
        (destination.DepthAttachment.has_value() || destination.DepthStencilAttachment.has_value());
    bool blitStencil =
        (source.StencilAttachment.has_value() || source.DepthStencilAttachment.has_value()) &&
        (destination.StencilAttachment.has_value() || destination.DepthStencilAttachment.has_value());

    unsigned srcX{}, srcY{}, dstX{}, dstY{};
    if (blitColor) {
        const auto& src { source.ColorAttachments[0].value() };
        std::visit(overloaded::lambda{
        [&srcX, &srcY](const GPUTexture& t) {
            srcX = t.Width;
            srcY = t.Height;
        },
        [&srcX, &srcY](const GPURenderBuffer& rb) {
            srcX = rb.Width;
            srcY = rb.Height;
        }}, src);

        const auto& dst{ destination.ColorAttachments[0].value() };
        std::visit(overloaded::lambda{
        [&dstX, &dstY](const GPUTexture& t) {
            dstX = t.Width;
            dstY = t.Height;
        },
        [&dstX, &dstY](const GPURenderBuffer& rb) {
            dstX = rb.Width;
            dstY = rb.Height;
        }}, dst);
    } else if (blitDepth) {
        const std::variant<GPUTexture, GPURenderBuffer>* src{};
        if (source.DepthAttachment.has_value()) {
            src = &source.DepthAttachment.value();
        } else {
            src = &source.DepthStencilAttachment.value();
        }
        std::visit(overloaded::lambda{
        [&srcX, &srcY](const GPUTexture& t) {
            srcX = t.Width;
            srcY = t.Height;
        },
        [&srcX, &srcY](const GPURenderBuffer& rb) {
            srcX = rb.Width;
            srcY = rb.Height;
        }}, *src);

        const std::variant<GPUTexture, GPURenderBuffer>* dst{};
        if (destination.DepthAttachment.has_value()) {
            dst = &destination.DepthAttachment.value();
        } else {
            dst = &destination.DepthStencilAttachment.value();
        }
        std::visit(overloaded::lambda{
        [&dstX, &dstY](const GPUTexture& t) {
            dstX = t.Width;
            dstY = t.Height;
        },
        [&dstX, &dstY](const GPURenderBuffer& rb) {
            dstX = rb.Width;
            dstY = rb.Height;
        }}, *dst);
    } else if (blitStencil) {
        const std::variant<GPUTexture, GPURenderBuffer>* src{};
        if (source.StencilAttachment.has_value()) {
            src = &source.StencilAttachment.value();
        } else {
            src = &source.DepthStencilAttachment.value();
        }
        std::visit(overloaded::lambda{
        [&srcX, &srcY](const GPUTexture& t) {
            srcX = t.Width;
            srcY = t.Height;
        },
        [&srcX, &srcY](const GPURenderBuffer& rb) {
            srcX = rb.Width;
            srcY = rb.Height;
        }}, *src);

        const std::variant<GPUTexture, GPURenderBuffer>* dst{};
        if (destination.StencilAttachment.has_value()) {
            dst = &destination.StencilAttachment.value();
        } else {
            dst = &destination.DepthStencilAttachment.value();
        }
        std::visit(overloaded::lambda{
        [&dstX, &dstY](const GPUTexture& t) {
            dstX = t.Width;
            dstY = t.Height;
        },
        [&dstX, &dstY](const GPURenderBuffer& rb) {
            dstX = rb.Width;
            dstY = rb.Height;
        }}, *dst);
    } else {
        return;
    }

    auto mask = (blitColor ? GL_COLOR_BUFFER_BIT : GLuint()) ||
                (blitDepth ? GL_DEPTH_BUFFER_BIT : GLuint()) ||
                (blitStencil ? GL_STENCIL_BUFFER_BIT : GLuint());

    glBlitNamedFramebuffer(
        source.GLObjectID,
        destination.GLObjectID,
        0, 0, srcX, srcY,
        0, 0, dstX, dstY,
        mask,
        GL_NEAREST
    );
}
void Graphics::Render(const GPUVertexArray& vao, int count, int first) noexcept {
    glBindVertexArray(vao.GLObjectID);
    if (vao.ElementBuffer) {
        glDrawElements(ToGLTopology(vao.Topology), count, ToGLDataType(vao.IndexType), nullptr);
    } else {
        glDrawArrays(ToGLTopology(vao.Topology), first, count);
    }
}
void Graphics::RenderInstanced(const GPUVertexArray& vao, int instanceCount, int count, int first) noexcept {
    glBindVertexArray(vao.GLObjectID);
    if (vao.ElementBuffer) {
        glDrawElementsInstanced(ToGLTopology(vao.Topology), count, ToGLDataType(vao.IndexType), nullptr, instanceCount);
    } else {
        glDrawArraysInstanced(ToGLTopology(vao.Topology), first, count, instanceCount);
    }
}

void Graphics::SetRenderArea(const Region region) noexcept {
    glViewport(region.X, region.Y, region.Width, region.Height);
}
void Graphics::SetRenderTarget(const GPUFrameBuffer& renderTarget) noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.GLObjectID);
}
void Graphics::ClearRenderTargetColor(const GPUFrameBuffer& renderTarget, std::array<float, 4> color, unsigned colorBufferIndex) noexcept {
    assert(renderTarget.ColorAttachments[colorBufferIndex]);
    glClearNamedFramebufferfv(renderTarget.GLObjectID, GL_COLOR, colorBufferIndex, color.data());
}
void Graphics::ClearRenderTargetColors(const GPUFrameBuffer& renderTarget, std::array<float, 4> color) noexcept {
    for (int i = 0; i < renderTarget.ColorAttachments.size(); i++) {
        if (renderTarget.ColorAttachments[i]) {
            glClearNamedFramebufferfv(renderTarget.GLObjectID, GL_COLOR, i, color.data());
        }
    }
}
void Graphics::ClearRenderTargetDepth(const GPUFrameBuffer& renderTarget, float depth) noexcept {
    assert(renderTarget.DepthAttachment || renderTarget.DepthStencilAttachment);
    glClearNamedFramebufferfv(renderTarget.GLObjectID, GL_DEPTH, 0, &depth);
}
void Graphics::ClearRenderTargetStencil(const GPUFrameBuffer& renderTarget, int stencil) noexcept {
    assert(renderTarget.StencilAttachment || renderTarget.DepthStencilAttachment);
    glClearNamedFramebufferiv(renderTarget.GLObjectID, GL_STENCIL, 0, &stencil);
}
void Graphics::ClearRenderTarget(const GPUFrameBuffer& renderTarget, std::array<float, 4> color, float depth, int stencil) noexcept {
    if (!renderTarget.ColorAttachments.empty()) {
        ClearRenderTargetColors(renderTarget, color);
    }
    if (renderTarget.DepthStencilAttachment || (renderTarget.DepthAttachment && renderTarget.StencilAttachment)) {
        glClearNamedFramebufferfi(renderTarget.GLObjectID, GL_DEPTH_STENCIL, 0, depth, stencil);
    } else {
        if (renderTarget.DepthAttachment) {
            ClearRenderTargetDepth(renderTarget, depth);
        }
        if (renderTarget.StencilAttachment) {
            ClearRenderTargetStencil(renderTarget, stencil);
        }
    }
}