#pragma once

#include <array>
#include <vector>
#include <memory>

#include <GL/glew.h>

#include <Engine/OpenGLCommon.hpp>
#include <Engine/Region.hpp>
#include <Engine/Resolution.hpp>

struct FrameBuffer;

struct FrameBufferBuilder {
    struct ColorAttachment {
        GLuint format;
        GLuint mode;
    };

    FrameBufferBuilder& Clear() noexcept;

    FrameBufferBuilder& SetResolution(Resolution resolution) noexcept;
    FrameBufferBuilder& SetMSAASamples(int samples) noexcept;
    FrameBufferBuilder& AddColorAttachment(OpenGL::TextureFormat format, OpenGL::TextureWrapMode mode = OpenGL::TextureWrapMode::REPEAT) noexcept;
    FrameBufferBuilder& SetDepthStencilAttachment(OpenGL::DepthFormat format) noexcept;
    FrameBufferBuilder& SetDepthStencilAttachment(OpenGL::StencilFormat format) noexcept;
    FrameBufferBuilder& SetDepthStencilAttachment(OpenGL::DepthStencilFormat format) noexcept;

    FrameBuffer Build() noexcept;
    FrameBuffer* BuildRaw() noexcept;
    std::unique_ptr<FrameBuffer> BuildUnique() noexcept;

private:
    Resolution resolution{};
    int msaaSamples{};
    std::array<ColorAttachment, 8> colorFormats{};
    int colorAttachmentCount{};
    GLuint renderFormat{};
};

struct FrameBuffer {

    static FrameBuffer BackBuffer() noexcept;

    FrameBuffer() noexcept = default;
    FrameBuffer(Resolution resolution, int msaaSamples, std::array<FrameBufferBuilder::ColorAttachment, 8> colorFormats, int colorAttachmentCount, GLuint renderFormat) noexcept;
    ~FrameBuffer() noexcept;
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&& other) noexcept;
    FrameBuffer& operator=(const FrameBuffer&) = delete;
    FrameBuffer& operator=(FrameBuffer&& other) noexcept;

    GLuint GetHandle() const noexcept;
    GLuint GetColorAttachment(unsigned int index = 0) const noexcept;
    GLuint GetDepthAttachment() const noexcept;
    GLuint GetStencilAttachment() const noexcept;
    bool HasColorAttachment() const noexcept;
    bool HasDepthAttachment() const noexcept;
    bool HasStencilAttachment() const noexcept;

    void Bind() const noexcept;
    void BindDraw() const noexcept;
    void BindRead() const noexcept;

    void BlitTo(const FrameBuffer& target, OpenGL::BufferBit buffers, Region source, Region destination) const noexcept;

    void ClearBuffers(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f, float depth = 1.0f, int stencil = 0) noexcept;
    void ClearColorBuffer(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f) noexcept;
    void ClearDepthBuffer(float depth = 1.0f) noexcept;
    void ClearStencilBuffer(int stencil = 0) noexcept;

private:
    GLuint frameBufferObject{};

    Resolution resolution{};
    int msaaSamples{};
    std::array<GLuint, 8> colorAttachments{};
    int colorAttachmentCount{};
    GLuint renderBuffer{};
    bool hasDepth, hasStencil;

    void AllocateGPU(std::array<FrameBufferBuilder::ColorAttachment, 8> colorFormats, GLuint renderFormat) noexcept;
    void DeallocateGPU() noexcept;
};
