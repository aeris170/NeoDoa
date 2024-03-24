#include <Engine/FrameBuffer.hpp>

#include <GL/glew.h>

#include <Engine/Log.hpp>
#include <Engine/Core.hpp>
#include <Engine/Window.hpp>

FrameBufferBuilder& FrameBufferBuilder::Clear() noexcept {
    resolution = {};
    msaaSamples = {};
    colorAttachmentCount = {};
    colorFormats = {};
    renderFormat = {};
    return *this;
}

FrameBufferBuilder& FrameBufferBuilder::SetResolution(Resolution resolution) noexcept { this->resolution = resolution; return *this; }
FrameBufferBuilder& FrameBufferBuilder::SetMSAASamples(int samples) noexcept { msaaSamples = samples; return *this; }
FrameBufferBuilder& FrameBufferBuilder::AddColorAttachment(OpenGL::TextureFormat format, OpenGL::TextureWrapMode mode) noexcept {
    if (colorAttachmentCount < colorFormats.size()) {
        auto& fmt = colorFormats[colorAttachmentCount];
        fmt.format = format;
        fmt.mode = mode;
        colorAttachmentCount++;
    } else {
        DOA_LOG_WARNING("Trying to add color but builder already has max amount!");
    }
    return *this;
}
FrameBufferBuilder& FrameBufferBuilder::SetDepthStencilAttachment(OpenGL::DepthFormat format) noexcept { renderFormat = format; return *this; }
FrameBufferBuilder& FrameBufferBuilder::SetDepthStencilAttachment(OpenGL::StencilFormat format) noexcept { renderFormat = format; return *this; }
FrameBufferBuilder& FrameBufferBuilder::SetDepthStencilAttachment(OpenGL::DepthStencilFormat format) noexcept { renderFormat = format; return *this; }

FrameBuffer FrameBufferBuilder::Build() noexcept { return { resolution, msaaSamples, colorFormats, colorAttachmentCount, renderFormat }; }
FrameBuffer* FrameBufferBuilder::BuildRaw() noexcept { return new FrameBuffer{ resolution, msaaSamples, colorFormats, colorAttachmentCount, renderFormat }; }
std::unique_ptr<FrameBuffer> FrameBufferBuilder::BuildUnique() noexcept { return std::make_unique<FrameBuffer>(resolution, msaaSamples, colorFormats, colorAttachmentCount, renderFormat); }

FrameBuffer FrameBuffer::BackBuffer() noexcept { return {}; }

FrameBuffer::FrameBuffer(Resolution resolution, int msaaSamples, std::array<FrameBufferBuilder::ColorAttachment, 8> colorFormats, int colorAttachmentCount, GLuint renderFormat) noexcept :
    resolution(resolution),
    msaaSamples(msaaSamples),
    colorAttachmentCount(colorAttachmentCount) {
    auto a = OpenGL::BufferBit::COLOR_BUFFER_BIT | OpenGL::BufferBit::DEPTH_BUFFER_BIT;
    hasDepth =
        renderFormat == OpenGL::DepthFormat::DEPTH_COMPONENT16 ||
        renderFormat == OpenGL::DepthFormat::DEPTH_COMPONENT24 ||
        renderFormat == OpenGL::DepthFormat::DEPTH_COMPONENT32 ||
        renderFormat == OpenGL::DepthFormat::DEPTH_COMPONENT32F ||
        renderFormat == OpenGL::DepthStencilFormat::DEPTH24_STENCIL8 ||
        renderFormat == OpenGL::DepthStencilFormat::DEPTH32F_STENCIL8;
    hasStencil =
        renderFormat == OpenGL::StencilFormat::STENCIL_INDEX1 ||
        renderFormat == OpenGL::StencilFormat::STENCIL_INDEX4 ||
        renderFormat == OpenGL::StencilFormat::STENCIL_INDEX8 ||
        renderFormat == OpenGL::StencilFormat::STENCIL_INDEX16 ||
        renderFormat == OpenGL::DepthStencilFormat::DEPTH24_STENCIL8 ||
        renderFormat == OpenGL::DepthStencilFormat::DEPTH32F_STENCIL8;

    AllocateGPU(colorFormats, renderFormat);
}
FrameBuffer::~FrameBuffer() noexcept {
    DeallocateGPU();
}
FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept :
    frameBufferObject(std::exchange(other.frameBufferObject, {})),
    resolution(std::exchange(other.resolution, {})),
    colorAttachments(std::exchange(other.colorAttachments, {})),
    colorAttachmentCount(std::exchange(other.colorAttachmentCount, {})),
    renderBuffer(std::exchange(other.renderBuffer, {})),
    hasDepth(std::exchange(other.hasDepth, {})),
    hasStencil(std::exchange(other.hasStencil, {})) {}
FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept {
    frameBufferObject = std::exchange(other.frameBufferObject, {});
    resolution = std::exchange(other.resolution, {});
    colorAttachments = std::exchange(other.colorAttachments, {});
    colorAttachmentCount = std::exchange(other.colorAttachmentCount, {});
    renderBuffer = std::exchange(other.renderBuffer, {});
    hasDepth = std::exchange(other.hasDepth, {});
    hasStencil = std::exchange(other.hasStencil, {});
    return *this;
}

GLuint FrameBuffer::GetHandle() const noexcept { return frameBufferObject; }
GLuint FrameBuffer::GetColorAttachment(unsigned int index) const noexcept { return colorAttachments[index]; }
GLuint FrameBuffer::GetDepthAttachment() const noexcept {
    if (!HasDepthAttachment()) { return 0; }
    return renderBuffer;
}
GLuint FrameBuffer::GetStencilAttachment() const noexcept {
    if (!HasStencilAttachment()) { return 0; }
    return renderBuffer;
}
bool FrameBuffer::HasColorAttachment() const noexcept { return colorAttachmentCount > 0; }
bool FrameBuffer::HasDepthAttachment() const noexcept { return hasDepth; }
bool FrameBuffer::HasStencilAttachment() const noexcept { return hasStencil; }

void FrameBuffer::Bind() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
    glViewport(0, 0, resolution.Width, resolution.Height);
}
void FrameBuffer::BindDraw() const noexcept {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObject);
    glViewport(0, 0, resolution.Width, resolution.Height);
}
void FrameBuffer::BindRead() const noexcept {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferObject);
}

void FrameBuffer::BlitTo(const FrameBuffer& target, OpenGL::BufferBit buffers, Region source, Region destination) const noexcept {
    glBlitNamedFramebuffer(
        frameBufferObject,
        target.frameBufferObject,
        source.X, source.Y,
        source.X + source.Width, source.Y + source.Height,
        destination.X, destination.Y,
        destination.X + destination.Width, destination.Y + destination.Height,
        buffers,
        GL_LINEAR
    );
}

void FrameBuffer::ClearBuffers(float r, float g, float b, float a, float depth, int stencil) noexcept {
    ClearColorBuffer(r, g, b, a);
    ClearDepthBuffer(depth);
    ClearStencilBuffer(stencil);
}
void FrameBuffer::ClearColorBuffer(float r, float g, float b, float a) noexcept {
    const float color[] { r, g, b, a };
    for (int i = 0; i < colorAttachmentCount; i++) {
        glClearTexImage(colorAttachments[i], 0, GL_RGBA, GL_FLOAT, &color);
    }
}
void FrameBuffer::ClearDepthBuffer(float depth) noexcept {
    if (!HasDepthAttachment()) { return; }
    glClearNamedFramebufferfv(frameBufferObject, GL_DEPTH, 0, &depth);
}
void FrameBuffer::ClearStencilBuffer(int stencil) noexcept {
    if (!HasStencilAttachment()) { return; }
    glClearNamedFramebufferiv(frameBufferObject, GL_STENCIL, 0, &stencil);
}

void FrameBuffer::AllocateGPU(std::array<FrameBufferBuilder::ColorAttachment, 8> colorFormats, GLuint renderFormat) noexcept {
    glCreateFramebuffers(1, &frameBufferObject);

    if (msaaSamples > 1) {
        /* Multisampled FBO request*/
        if (HasColorAttachment()) {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, colorAttachmentCount, &colorAttachments[0]);
            for (unsigned int i = 0; i < colorAttachmentCount; i++) {
                glTextureStorage2DMultisample(colorAttachments[i], msaaSamples, colorFormats[i].format, resolution.Width, resolution.Height, GL_TRUE);
            }
        }
        if (HasDepthAttachment() || HasStencilAttachment()) {
            /* Create depth/stencil attachment render buffer */
            glCreateRenderbuffers(1, &renderBuffer);
            glNamedRenderbufferStorageMultisample(renderBuffer, msaaSamples, renderFormat, resolution.Width, resolution.Height);

            glNamedFramebufferRenderbuffer(frameBufferObject, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
        }
    } else {
        /* Singlesampled FBO request*/
        if (colorAttachmentCount > 0) {
            /* Create color attachment texture */
            glCreateTextures(GL_TEXTURE_2D, colorAttachmentCount, &colorAttachments[0]);
            for (unsigned int i = 0; i < colorAttachmentCount; i++) {
                glTextureParameteri(colorAttachments[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(colorAttachments[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTextureParameteri(colorAttachments[i], GL_TEXTURE_WRAP_S, colorFormats[i].mode);
                glTextureParameteri(colorAttachments[i], GL_TEXTURE_WRAP_T, colorFormats[i].mode);
                glTextureStorage2D(colorAttachments[i], 1, colorFormats[i].format, resolution.Width, resolution.Height);
            }
        }
        if (HasDepthAttachment() || HasStencilAttachment()) {
            /* Create depth/stencil attachment render buffer */
            glCreateRenderbuffers(1, &renderBuffer);
            glNamedRenderbufferStorage(renderBuffer, renderFormat, resolution.Width, resolution.Height);

            glNamedFramebufferRenderbuffer(frameBufferObject, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
        }
    }

    static std::array<GLenum, 8> drawBufs;
    for (unsigned int i = 0; i < colorAttachmentCount; i++) {
        glNamedFramebufferTexture(frameBufferObject, GL_COLOR_ATTACHMENT0 + i, colorAttachments[i], 0);
        drawBufs[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glNamedFramebufferDrawBuffers(frameBufferObject, colorAttachmentCount, &drawBufs[0]);

    GLenum error = glCheckNamedFramebufferStatus(frameBufferObject, GL_FRAMEBUFFER);
    if (error != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer couldn't initialize! ERR_CODE: " << error << std::endl;
    }
}
void FrameBuffer::DeallocateGPU() noexcept {
    glDeleteFramebuffers(1, &frameBufferObject);
    glDeleteTextures(colorAttachmentCount, &colorAttachments[0]);
    glDeleteRenderbuffers(1, &renderBuffer);
}