#include <Engine/Graphics.hpp>

#include <cassert>

#include <Utility/TemplateUtilities.hpp>

#include <Engine/Region.hpp>
#include <Engine/Resolution.hpp>
#include <Engine/GPUShader.hpp>
#include <Engine/GPUBuffer.hpp>
#include <Engine/GPUFrameBuffer.hpp>
#include <Engine/GPUPipeline.hpp>
#include <Engine/GPUDescriptorSet.hpp>

namespace {
    std::optional<std::reference_wrapper<const GPUPipeline>> currentPipeline;

    Resolution GetAttachmentDimensions(const std::variant<GPUTexture, GPURenderBuffer>& attachment) noexcept {
        return std::visit(overloaded::lambda{
            [](const GPUTexture& t) -> Resolution {
                return { t.Width, t.Height };
            },
            [](const GPURenderBuffer& rb) -> Resolution {
                return { rb.Width, rb.Height };
            }
        }, attachment);
    }
}

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

    auto mask = (blitColor ? GL_COLOR_BUFFER_BIT : GLuint()) |
                (blitDepth ? GL_DEPTH_BUFFER_BIT : GLuint()) |
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
void Graphics::BlitDepth(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    const auto* srcAttachment =      source.DepthAttachment ?      &source.DepthAttachment : nullptr;
    const auto* dstAttachment = destination.DepthAttachment ? &destination.DepthAttachment : nullptr;

    assert(srcAttachment && srcAttachment->has_value());
    assert(dstAttachment && dstAttachment->has_value());

    Resolution srcResolution{ GetAttachmentDimensions(srcAttachment->value()) };
    Resolution dstResolution{ GetAttachmentDimensions(dstAttachment->value()) };

    glBlitNamedFramebuffer(
        source.GLObjectID,
        destination.GLObjectID,
        0, 0, srcResolution.Width, srcResolution.Height,
        0, 0, dstResolution.Width, dstResolution.Height,
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );
}
void Graphics::BlitStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    const auto* srcAttachment =      source.StencilAttachment ?      &source.StencilAttachment : nullptr;
    const auto* dstAttachment = destination.StencilAttachment ? &destination.StencilAttachment : nullptr;

    assert(srcAttachment && srcAttachment->has_value());
    assert(dstAttachment && dstAttachment->has_value());

    Resolution srcResolution{ GetAttachmentDimensions(srcAttachment->value()) };
    Resolution dstResolution{ GetAttachmentDimensions(dstAttachment->value()) };

    glBlitNamedFramebuffer(
        source.GLObjectID,
        destination.GLObjectID,
        0, 0, srcResolution.Width, srcResolution.Height,
        0, 0, dstResolution.Width, dstResolution.Height,
        GL_STENCIL_BUFFER_BIT,
        GL_NEAREST
    );
}
void Graphics::BlitDepthStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    const auto* srcAttachment =      source.DepthStencilAttachment ?      &source.DepthStencilAttachment : nullptr;
    const auto* dstAttachment = destination.DepthStencilAttachment ? &destination.DepthStencilAttachment : nullptr;

    assert(srcAttachment && srcAttachment->has_value());
    assert(dstAttachment && dstAttachment->has_value());

    Resolution srcResolution{ GetAttachmentDimensions(srcAttachment->value()) };
    Resolution dstResolution{ GetAttachmentDimensions(dstAttachment->value()) };

    glBlitNamedFramebuffer(
        source.GLObjectID,
        destination.GLObjectID,
        0, 0, srcResolution.Width, srcResolution.Height,
        0, 0, dstResolution.Width, dstResolution.Height,
        GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GL_NEAREST
    );
}

void Graphics::Render(int count, int first) noexcept {
    const GPUPipeline& pipeline = currentPipeline->get();
    if (pipeline.IndexBuffer) {
        glDrawElements(ToGLTopology(pipeline.Topology), count, ToGLDataType(pipeline.IndexType), nullptr);
    } else {
        glDrawArrays(ToGLTopology(pipeline.Topology), first, count);
    }
}
void Graphics::RenderInstanced(int instanceCount, int count, int first) noexcept {
    const GPUPipeline& pipeline = currentPipeline->get();
    if (pipeline.IndexBuffer) {
        glDrawElementsInstanced(ToGLTopology(pipeline.Topology), count, ToGLDataType(pipeline.IndexType), nullptr, instanceCount);
    } else {
        glDrawArraysInstanced(ToGLTopology(pipeline.Topology), first, count, instanceCount);
    }
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

void Graphics::BindPipeline(const GPUPipeline& pipeline) noexcept {
    currentPipeline.emplace(pipeline);

    glPolygonMode(GL_FRONT_AND_BACK, ToGLPolygonMode(pipeline.Polygon));
    pipeline.IsFaceCullingEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    glCullFace(ToGLCullMode(pipeline.Cull));
    glViewport(pipeline.Viewport.X, pipeline.Viewport.Y, pipeline.Viewport.Width, pipeline.Viewport.Height);

    pipeline.IsScissorEnabled ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
    glScissor(pipeline.Scissor.X, pipeline.Scissor.Y, pipeline.Scissor.Width, pipeline.Scissor.Height);

    pipeline.IsDepthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glDepthMask(pipeline.IsDepthWriteEnabled ? GL_TRUE : GL_FALSE);
    glDepthFunc(ToGLDepthFunction(pipeline.DepthFunc));
    pipeline.IsDepthClampEnabled ? glEnable(GL_DEPTH_CLAMP) : glDisable(GL_DEPTH_CLAMP);

    pipeline.IsMultisampleEnabled ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);

    pipeline.IsBlendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFuncSeparate(
        ToGLBlendFactor(pipeline.SourceFactor),
        ToGLBlendFactor(pipeline.DestinationFactor),
        ToGLBlendFactor(pipeline.SourceAlphaFactor),
        ToGLBlendFactor(pipeline.DestinationAlphaFactor)
    );

    assert(pipeline.ShaderProgram);
    glUseProgram(pipeline.ShaderProgram->get().GLObjectID);
    glBindVertexArray(pipeline.GLObjectID);
}

void Graphics::BindDescriptorSet(const GPUDescriptorSet& descriptorSet) noexcept {
    for (const DescriptorBinding& binding : descriptorSet.Bindings) {
        if (std::holds_alternative<std::monostate>(binding.Descriptor)) { continue; }

        std::visit(overloaded::lambda {
            [] (const std::monostate&) { /* empty */ },
            [&binding](const DescriptorBinding::UniformBuffer& uniformBuffer) {
                glBindBufferBase(GL_UNIFORM_BUFFER, binding.BindingSlot, uniformBuffer.Buffer.get().GLObjectID);
            },
            [&binding](const DescriptorBinding::StorageBuffer& storageBuffer) {
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding.BindingSlot, storageBuffer.Buffer.get().GLObjectID);
            },
            [&binding](const DescriptorBinding::CombinedImageSampler& combinedImageSampler) {
                glBindTextureUnit(binding.BindingSlot, combinedImageSampler.Texture.get().GLObjectID);
                glBindSampler(binding.BindingSlot, combinedImageSampler.Sampler.get().GLObjectID);
            },
        }, binding.Descriptor);
    }
}