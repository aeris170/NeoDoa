#pragma once

#include <Engine/Graphics.hpp>

#include <array>
#include <string>
#include <variant>
#include <optional>

#include <Engine/GPUTexture.hpp>

// RenderBuffer
using RenderBufferAllocatorMessage = std::string;

struct GPURenderBuffer {
    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{};
#endif
    DataFormat Format{};
    unsigned Width{};
    unsigned Height{};

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPURenderBuffer);
};
struct GPURenderBufferBuilder {

    GPURenderBufferBuilder& SetName(std::string_view name) noexcept;
    GPURenderBufferBuilder& SetWidth(unsigned width) noexcept;
    GPURenderBufferBuilder& SetHeight(unsigned height) noexcept;
    GPURenderBufferBuilder& SetFormat(DataFormat format) noexcept;
    GPURenderBufferBuilder& SetLayout(unsigned width, unsigned height, DataFormat format) noexcept;

    [[nodiscard]] std::pair<std::optional<GPURenderBuffer>, RenderBufferAllocatorMessage> Build() noexcept;

private:
#ifdef DEBUG
    std::string name{};
#endif
    unsigned width{ 1 };
    unsigned height{ 1 };
    DataFormat format{};
public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPURenderBufferBuilder);
};

// FrameBuffer
using FrameBufferAllocatorMessage = std::string;

struct GPUFrameBuffer {
    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{ "Backbuffer" };
#endif
    std::array<std::optional<std::variant<GPUTexture, GPURenderBuffer>>, 8> ColorAttachments{};
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
    [[nodiscard]] std::pair<std::optional<GPUFrameBuffer>, FrameBufferAllocatorMessage> Build() noexcept;

private:
#ifdef DEBUG
    std::string name{};
#endif
    std::array<std::optional<std::variant<GPUTexture, GPURenderBuffer>>, 8> colorAttachments{};
    std::optional<std::variant<GPUTexture, GPURenderBuffer>> depthAttachment{};
    std::optional<std::variant<GPUTexture, GPURenderBuffer>> stencilAttachment{};
    std::optional<std::variant<GPUTexture, GPURenderBuffer>> depthStencilAttachment{};
public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUFrameBufferBuilder);
};