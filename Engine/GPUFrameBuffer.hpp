#pragma once

#include <array>
#include <string>
#include <vector>
#include <utility>
#include <variant>
#include <optional>
#include <string_view>

#include <Engine/Graphics.hpp>
#include <Engine/GPUTexture.hpp>

// RenderBuffer
struct GPURenderBuffer {
    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{};
#endif
    unsigned Width{};
    unsigned Height{};
    DataFormat Format{};
    Multisample Samples{};

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPURenderBuffer);
};
struct GPURenderBufferBuilder {

    GPURenderBufferBuilder& SetName(std::string_view name) noexcept;
    GPURenderBufferBuilder& SetWidth(unsigned width) noexcept;
    GPURenderBufferBuilder& SetHeight(unsigned height) noexcept;
    GPURenderBufferBuilder& SetFormat(DataFormat format) noexcept;
    GPURenderBufferBuilder& SetLayout(unsigned width, unsigned height, DataFormat format) noexcept;
    GPURenderBufferBuilder& SetSamples(Multisample multisample) noexcept;

    [[nodiscard]] std::pair<std::optional<GPURenderBuffer>, std::vector<RenderBufferAllocatorMessage>> Build() noexcept;

private:
#ifdef DEBUG
    std::string name{};
#endif
    unsigned width{ 1 };
    unsigned height{ 1 };
    DataFormat format{};
    Multisample samples{};
public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPURenderBufferBuilder);

private:
#ifdef OPENGL_4_6_SUPPORT
    friend std::pair<std::optional<GPURenderBuffer>, std::vector<RenderBufferAllocatorMessage>> Graphics::OpenGL::Build(GPURenderBufferBuilder&) noexcept;
#endif
};

// FrameBuffer
struct GPUFrameBuffer {
    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{ "Backbuffer" };
#endif
    std::array<std::optional<std::variant<GPUTexture, GPURenderBuffer>>, MaxFrameBufferColorAttachments> ColorAttachments{};
    std::optional<std::variant<GPUTexture, GPURenderBuffer>> DepthAttachment{};
    std::optional<std::variant<GPUTexture, GPURenderBuffer>> StencilAttachment{};
    std::optional<std::variant<GPUTexture, GPURenderBuffer>> DepthStencilAttachment{};

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPUFrameBuffer);
};
struct GPUFrameBufferBuilder {

    GPUFrameBufferBuilder& SetName(std::string_view name) noexcept;
    GPUFrameBufferBuilder& AttachColorTexture(GPUTexture&& texture, unsigned index) noexcept;
    GPUFrameBufferBuilder& AttachDepthTexture(GPUTexture&& texture) noexcept;
    GPUFrameBufferBuilder& AttachStencilTexture(GPUTexture&& texture) noexcept;
    GPUFrameBufferBuilder& AttachDepthStencilTexture(GPUTexture&& texture) noexcept;
    GPUFrameBufferBuilder& AttachColorRenderBuffer(GPURenderBuffer&& renderBuffer, unsigned index) noexcept;
    GPUFrameBufferBuilder& AttachDepthRenderBuffer(GPURenderBuffer&& renderBuffer) noexcept;
    GPUFrameBufferBuilder& AttachStencilRenderBuffer(GPURenderBuffer&& renderBuffer) noexcept;
    GPUFrameBufferBuilder& AttachDepthStencilRenderBuffer(GPURenderBuffer&& renderBuffer) noexcept;
    [[nodiscard]] std::pair<std::optional<GPUFrameBuffer>, std::vector<FrameBufferAllocatorMessage>> Build() noexcept;

private:
#ifdef DEBUG
    std::string name{};
#endif
    std::array<std::optional<std::variant<GPUTexture, GPURenderBuffer>>, MaxFrameBufferColorAttachments> colorAttachments{};
    std::optional<std::variant<GPUTexture, GPURenderBuffer>> depthAttachment{};
    std::optional<std::variant<GPUTexture, GPURenderBuffer>> stencilAttachment{};
    std::optional<std::variant<GPUTexture, GPURenderBuffer>> depthStencilAttachment{};
public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUFrameBufferBuilder);

private:
#ifdef OPENGL_4_6_SUPPORT
    friend std::pair<std::optional<GPUFrameBuffer>, std::vector<FrameBufferAllocatorMessage>> Graphics::OpenGL::Build(GPUFrameBufferBuilder&) noexcept;
#endif
};