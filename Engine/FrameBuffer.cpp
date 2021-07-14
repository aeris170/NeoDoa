#include "FrameBuffer.hpp"

#include <GL/glew.h>

#include "Log.hpp"

FrameBuffer::FrameBuffer(int width, int height) :
    _width(width),
    _height(height) {
    glCreateFramebuffers(1, &_fbo);

    glCreateTextures(GL_TEXTURE_2D, 1, &_tex);
    glTextureParameteri(_tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(_tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureStorage2D(_tex, 1, GL_RGB8, _width, _height);

    glNamedFramebufferTexture(_fbo, GL_COLOR_ATTACHMENT0, _tex, 0);

    glCreateRenderbuffers(1, &_rbo);
    glNamedRenderbufferStorage(_rbo, GL_DEPTH24_STENCIL8, _width, _height);

    glNamedFramebufferRenderbuffer(_fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

    if (glCheckNamedFramebufferStatus(_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        DOA_LOG_FATAL("Offscreen buffer couldn't initialize!");
    }
}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &_fbo);
    glDeleteTextures(1, &_tex);
    glDeleteRenderbuffers(1, &_rbo);
}
