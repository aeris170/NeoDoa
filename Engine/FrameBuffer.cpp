#include "FrameBuffer.hpp"

#include <GL/glew.h>

#include "Log.hpp"
#include "Core.hpp"
#include "Window.hpp"

FrameBuffer::FrameBuffer(Resolution resolution) noexcept :
    _resolution(resolution) {
    glCreateFramebuffers(1, &_fbo);

    glCreateTextures(GL_TEXTURE_2D, 1, &_tex);
    glTextureParameteri(_tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(_tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureStorage2D(_tex, 1, GL_RGB8, _resolution.w, _resolution.w);

    glNamedFramebufferTexture(_fbo, GL_COLOR_ATTACHMENT0, _tex, 0);

    glCreateRenderbuffers(1, &_rbo);
    glNamedRenderbufferStorage(_rbo, GL_DEPTH24_STENCIL8, _resolution.w, _resolution.h);

    glNamedFramebufferRenderbuffer(_fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

    if (glCheckNamedFramebufferStatus(_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        DOA_LOG_FATAL("Offscreen buffer couldn't initialize!");
    }
}

FrameBuffer::~FrameBuffer() noexcept {
    DeAlloc();
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept :
    _resolution(std::move(other._resolution)),
    _fbo(std::exchange(other._fbo, 0)),
    _tex(std::exchange(other._tex, 0)),
    _rbo(std::exchange(other._rbo, 0)) {}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept {
    DeAlloc(); //!!!
    _resolution = std::move(other._resolution);
    _fbo = std::exchange(other._fbo, 0);
    _tex = std::exchange(other._tex, 0);
    _rbo = std::exchange(other._rbo, 0);
    return *this;
}

void FrameBuffer::DeAlloc() {
    glDeleteFramebuffers(1, &_fbo);
    glDeleteTextures(1, &_tex);
    glDeleteRenderbuffers(1, &_rbo);
}

void FrameBuffer::Bind() const {
    glViewport(0, 0, _resolution.w, _resolution.h);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void FrameBuffer::UnBind() const {
    static auto& core = GetCore();
    Resolution rs = core->_window->_contentResolution;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, rs.w, rs.h);
}

void FrameBuffer::UnBind(Resolution oldViewport) const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, oldViewport.w, oldViewport.h);
}

void FrameBuffer::ClearBuffers() const {
    glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void FrameBuffer::ClearColorBuffer() const {
    glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void FrameBuffer::ClearDepthBuffer() const {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::ClearStencilBuffer() const {
    glClear(GL_STENCIL_BUFFER_BIT);
}