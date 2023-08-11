#pragma once

#include <vector>

#include <GL/glew.h>

namespace OpenGL {
    using BufferData = std::vector<std::byte>;

    enum BufferBit {
        COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT,
        DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT,
        STENCIL_BUFFER_BIT = GL_STENCIL_BUFFER_BIT,
    };

    enum TextureWrapMode {
        REPEAT = GL_REPEAT,
        MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
        CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
        CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
    };

    enum TextureFormat {
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8,
        RGB16 = GL_RGB16,
        RGBA16 = GL_RGBA16,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,
        R11F_G11F_B10F = GL_R11F_G11F_B10F
    };

    enum DepthFormat {
        DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16,
        DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24,
        DEPTH_COMPONENT32 = GL_DEPTH_COMPONENT32,
        DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F,
    };

    enum StencilFormat {
        STENCIL_INDEX1 = GL_STENCIL_INDEX1,
        STENCIL_INDEX4 = GL_STENCIL_INDEX4,
        STENCIL_INDEX8 = GL_STENCIL_INDEX8,
        STENCIL_INDEX16 = GL_STENCIL_INDEX16,
    };

    enum DepthStencilFormat {
        DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
        DEPTH32F_STENCIL8 = GL_DEPTH32F_STENCIL8
    };
}