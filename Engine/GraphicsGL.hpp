#pragma once

#ifdef OPENGL_4_6_SUPPORT

#include <GL/glew.h>

#include <Engine/Graphics.hpp>

constexpr GLenum ToGLBufferFlags(BufferProperties properties) noexcept {
    GLenum glBitmask = 0;
    using enum BufferProperties;
    if (static_cast<bool>(properties & DynamicStorage))   glBitmask |= GL_DYNAMIC_STORAGE_BIT;
    if (static_cast<bool>(properties & ReadableFromCPU))  glBitmask |= GL_MAP_READ_BIT;
    if (static_cast<bool>(properties & WriteableFromCPU)) glBitmask |= GL_MAP_WRITE_BIT;
    if (static_cast<bool>(properties & Persistent))       glBitmask |= GL_MAP_PERSISTENT_BIT;
    if (static_cast<bool>(properties & Coherent))         glBitmask |= GL_MAP_COHERENT_BIT;
    if (static_cast<bool>(properties & CPUStorage))       glBitmask |= GL_CLIENT_STORAGE_BIT;
    return glBitmask;
}
constexpr GLenum ToGLTextureMinificationMode(TextureMinificationMode mode) noexcept {
    using enum TextureMinificationMode;
    switch (mode) {
    case Nearest:               return GL_NEAREST;
    case Linear:                return GL_LINEAR;
    case NearestMipmapNearest:  return GL_NEAREST_MIPMAP_NEAREST;
    case LinearMipmapNearest:   return GL_LINEAR_MIPMAP_NEAREST;
    case NearestMipmapLinear:   return GL_NEAREST_MIPMAP_LINEAR;
    case LinearMipmapLinear:    return GL_LINEAR_MIPMAP_LINEAR;
    }
    std::unreachable();
}
constexpr GLenum ToGLTextureMagnificationMode(TextureMagnificationMode mode) noexcept {
    using enum TextureMagnificationMode;
    switch (mode) {
    case Nearest: return GL_NEAREST;
    case Linear:  return GL_LINEAR;
    }
    std::unreachable();
}
constexpr GLenum ToGLTextureWrappingMode(TextureWrappingMode mode) noexcept {
    using enum TextureWrappingMode;
    switch (mode) {
    case Repeat:             return GL_REPEAT;
    case MirroredRepeat:     return GL_MIRRORED_REPEAT;
    case ClampToEdge:        return GL_CLAMP_TO_EDGE;
    case MirrorClampToEdge:  return GL_MIRROR_CLAMP_TO_EDGE;
    case ClampToBorder:      return GL_CLAMP_TO_BORDER;
    }
    std::unreachable();
}
constexpr GLenum ToGLTextureCompareMode(TextureCompareMode mode) noexcept {
    using enum TextureCompareMode;
    switch (mode) {
    case CompareRefToTexture: return GL_COMPARE_REF_TO_TEXTURE;
    case None:                return GL_NONE;
    }
    std::unreachable();
}
constexpr GLenum ToGLTextureCompareFunction(TextureCompareFunction func) noexcept {
    using enum TextureCompareFunction;
    switch (func) {
    case LessEqual:    return GL_LEQUAL;
    case GreaterEqual: return GL_GEQUAL;
    case Less:         return GL_LESS;
    case Greater:      return GL_GREATER;
    case Equal:        return GL_EQUAL;
    case NotEqual:     return GL_NOTEQUAL;
    case Always:       return GL_ALWAYS;
    case Never:        return GL_NEVER;
    }
    std::unreachable();
}
constexpr GLenum ToGLSizedFormat(DataFormat format) {
    using enum DataFormat;
    switch (format) {
        // Unsigned normalized formats
    case R8:                 return GL_R8;
    case RG8:                return GL_RG8;
    case RGB8:               return GL_RGB8;
    case RGBA8:              return GL_RGBA8;
    case R16:                return GL_R16;
    case RG16:               return GL_RG16;
    case RGB16:              return GL_RGB16;
    case RGBA16:             return GL_RGBA16;
    case R16F:               return GL_R16F;
    case RG16F:              return GL_RG16F;
    case RGB16F:             return GL_RGB16F;
    case RGBA16F:            return GL_RGBA16F;
    case R32F:               return GL_R32F;
    case RG32F:              return GL_RG32F;
    case RGB32F:             return GL_RGB32F;
    case RGBA32F:            return GL_RGBA32F;
        // Signed normalized formats
    case R8_SNORM:           return GL_R8_SNORM;
    case RG8_SNORM:          return GL_RG8_SNORM;
    case RGB8_SNORM:         return GL_RGB8_SNORM;
    case RGBA8_SNORM:        return GL_RGBA8_SNORM;
    case R16_SNORM:          return GL_R16_SNORM;
    case RG16_SNORM:         return GL_RG16_SNORM;
    case RGB16_SNORM:        return GL_RGB16_SNORM;
    case RGBA16_SNORM:       return GL_RGBA16_SNORM;
        // Unsigned integer formats
    case R8UI:               return GL_R8UI;
    case RG8UI:              return GL_RG8UI;
    case RGB8UI:             return GL_RGB8UI;
    case RGBA8UI:            return GL_RGBA8UI;
    case R16UI:              return GL_R16UI;
    case RG16UI:             return GL_RG16UI;
    case RGB16UI:            return GL_RGB16UI;
    case RGBA16UI:           return GL_RGBA16UI;
    case R32UI:              return GL_R32UI;
    case RG32UI:             return GL_RG32UI;
    case RGB32UI:            return GL_RGB32UI;
    case RGBA32UI:           return GL_RGBA32UI;
        // Signed integer formats
    case R8I:                return GL_R8I;
    case RG8I:               return GL_RG8I;
    case RGB8I:              return GL_RGB8I;
    case RGBA8I:             return GL_RGBA8I;
    case R16I:               return GL_R16I;
    case RG16I:              return GL_RG16I;
    case RGB16I:             return GL_RGB16I;
    case RGBA16I:            return GL_RGBA16I;
    case R32I:               return GL_R32I;
    case RG32I:              return GL_RG32I;
    case RGB32I:             return GL_RGB32I;
    case RGBA32I:            return GL_RGBA32I;
        // SRGB Formats
    case SRGB8:              return GL_SRGB8;
    case SRGBA8:             return GL_SRGB8_ALPHA8;
        // Depth formats
    case DEPTH16:            return GL_DEPTH_COMPONENT16;
    case DEPTH24:            return GL_DEPTH_COMPONENT24;
    case DEPTH32:            return GL_DEPTH_COMPONENT32;
    case DEPTH32F:           return GL_DEPTH_COMPONENT32F;
        // Stencil Formats
    case STENCIL1:           return GL_STENCIL_INDEX1;
    case STENCIL4:           return GL_STENCIL_INDEX4;
    case STENCIL8:           return GL_STENCIL_INDEX8;
    case STENCIL16:          return GL_STENCIL_INDEX16;
        // Depth-stencil formats
    case DEPTH24_STENCIL8:   return GL_DEPTH24_STENCIL8;
    case DEPTH32F_STENCIL8:  return GL_DEPTH32F_STENCIL8;
        // Split Formats
    case R3G3B2:             return GL_R3_G3_B2;
    case RGB5A1:             return GL_RGB5_A1;
    case RGB10A2:            return GL_RGB10_A2;
    case RGB10A2UI:          return GL_RGB10_A2UI;
    case R11FG11FB10F:       return GL_R11F_G11F_B10F;
    case RGB9E5:             return GL_RGB9_E5;
        // Other Formats (when do they even get used?)
    case RGB4:               return GL_RGB4;
    case RGB5:               return GL_RGB5;
    case RGB565:             return GL_RGB565;
    case RGB10:              return GL_RGB10;
    case RGB12:              return GL_RGB12;
    case RGBA2:              return GL_RGBA2;
    case RGBA4:              return GL_RGBA4;
    case RGBA12:             return GL_RGBA12;
    }
    std::unreachable();
}
constexpr GLenum ToGLBaseFormat(DataFormat format) {
    using enum DataFormat;
    switch (format) {
    case R8:
    case R16:
    case R16F:
    case R32F:
    case R8_SNORM:
    case R16_SNORM:
    case R8UI:
    case R16UI:
    case R32UI:
    case R8I:
    case R16I:
    case R32I:
        return GL_RED;
    case RG8:
    case RG16:
    case RG16F:
    case RG32F:
    case RG8_SNORM:
    case RG16_SNORM:
    case RG8UI:
    case RG16UI:
    case RG32UI:
    case RG8I:
    case RG16I:
    case RG32I:
        return GL_RG;
    case RGB8:
    case RGB16:
    case RGB16F:
    case RGB32F:
    case RGB8_SNORM:
    case RGB16_SNORM:
    case RGB8UI:
    case RGB16UI:
    case RGB32UI:
    case RGB8I:
    case RGB16I:
    case RGB32I:
    case R3G3B2:
    case R11FG11FB10F:
    case RGB9E5:
    case RGB4:
    case RGB5:
    case RGB565:
    case RGB10:
    case RGB12:
        return GL_RGB;
    case RGBA8:
    case RGBA16:
    case RGBA16F:
    case RGBA32F:
    case RGBA8_SNORM:
    case RGBA16_SNORM:
    case RGBA8UI:
    case RGBA16UI:
    case RGBA32UI:
    case RGBA8I:
    case RGBA16I:
    case RGBA32I:
    case RGB5A1:
    case RGB10A2:
    case RGB10A2UI:
    case RGBA2:
    case RGBA4:
    case RGBA12:
        return GL_RGBA;
    case SRGB8:
        return GL_SRGB;
    case SRGBA8:
        return GL_SRGB_ALPHA;
    case DEPTH16:
    case DEPTH24:
    case DEPTH32:
    case DEPTH32F:
        return GL_DEPTH_COMPONENT;
    case STENCIL1:
    case STENCIL4:
    case STENCIL8:
    case STENCIL16:
        return GL_STENCIL_INDEX;
    case DEPTH24_STENCIL8:
    case DEPTH32F_STENCIL8:
        return GL_DEPTH_STENCIL;
    }
    std::unreachable();
}
constexpr GLenum ToGLTopology(TopologyType t) noexcept {
    using enum TopologyType;
    switch (t) {
    case Points: return GL_POINTS;
    case Lines: return GL_LINES;
    case LineStrip: return GL_LINE_STRIP;
    case LineLoop: return GL_LINE_LOOP;
    case Triangles: return GL_TRIANGLES;
    case TriangleStrip: return GL_TRIANGLE_STRIP;
    case TriangleFan: return GL_TRIANGLE_FAN;
    }
    std::unreachable();
}
constexpr GLenum ToGLDataType(DataType t) noexcept {
    using enum DataType;
    switch (t) {
    case Byte:          return GL_BYTE;
    case UnsignedByte:  return GL_UNSIGNED_BYTE;
    case Short:         return GL_SHORT;
    case UnsignedShort: return GL_UNSIGNED_SHORT;
    case Int:           return GL_INT;
    case UnsignedInt:   return GL_UNSIGNED_INT;
    case Float:         return GL_FLOAT;
    case Double:        return GL_DOUBLE;
    }
    std::unreachable();
}
constexpr GLenum ToGLPolygonMode(PolygonMode mode) noexcept {
    using enum PolygonMode;
    switch (mode) {
    case Fill:  return GL_FILL;
    case Line:  return GL_LINE;
    case Point: return GL_POINT;
    }
    std::unreachable();
}
constexpr GLenum ToGLCullMode(CullMode mode) noexcept {
    using enum CullMode;
    switch (mode) {
    case Front:        return GL_FRONT;
    case Back:         return GL_BACK;
    case FrontAndBack: return GL_FRONT_AND_BACK;
    }
    std::unreachable();
}
constexpr GLenum ToGLDepthFunction(DepthFunction mode) noexcept {
    using enum DepthFunction;
    switch (mode) {
    case Always:       return GL_ALWAYS;
    case Never:        return GL_NEVER;
    case Less:         return GL_LESS;
    case Equal:        return GL_EQUAL;
    case LessEqual:    return GL_LEQUAL;
    case Greater:      return GL_GREATER;
    case GreaterEqual: return GL_NOTEQUAL;
    case NotEqual:     return GL_GEQUAL;
    }
    std::unreachable();
}
constexpr GLenum ToGLBlendFactor(BlendFactor factor) noexcept {
    using enum BlendFactor;
    switch (factor) {
    case Zero:                  return GL_ZERO;
    case One:                   return GL_ONE;
    case SrcColor:              return GL_SRC_COLOR;
    case OneMinusSrcColor:      return GL_ONE_MINUS_SRC_COLOR;
    case DstColor:              return GL_DST_COLOR;
    case OneMinusDstColor:      return GL_ONE_MINUS_DST_COLOR;
    case SrcAlpha:              return GL_SRC_ALPHA;
    case OneMinusSrcAlpha:      return GL_ONE_MINUS_SRC_ALPHA;
    case DstAlpha:              return GL_DST_ALPHA;
    case OneMinusDstAlpha:      return GL_ONE_MINUS_DST_ALPHA;
    case ConstantColor:         return GL_CONSTANT_COLOR;
    case OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
    case ConstantAlpha:         return GL_CONSTANT_ALPHA;
    case OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
    case SrcAlphaSaturate:      return GL_SRC_ALPHA_SATURATE;
    }
    std::unreachable();
}
constexpr GLuint ToGLInputRate(InputRate rate) noexcept {
    using enum InputRate;
    switch (rate) {
    case PerVertex:   return 0;
    case PerInstance: return 1;
    }
    std::unreachable();
}
#endif