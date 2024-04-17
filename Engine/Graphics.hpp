#pragma once

#include <span>
#include <memory>
#include <vector>
#include <cstddef>
#include <string_view>

#include <GL/glew.h>

#define ND_GRAPHICS_MOVE_ONLY_RESOURCE(x) \
    x() noexcept = default; \
    ~x() noexcept; \
    x(const x&) = delete; \
    x(x&& other) noexcept; \
    x& operator=(const x&) = delete; \
    x& operator=(x&& other) noexcept

#define ND_GRAPHICS_BUILDER_RULE_OF_0(x) \
    x() = default; \
    ~x() = default; \
    x(const x&) = delete; \
    x& operator=(const x&) = delete; \
    x(x&&) = delete; \
    x& operator=(x&&) = delete

using OwningPointerToRawData = std::unique_ptr<std::byte>;
using NonOwningPointerToRawData = std::byte*;
using NonOwningPointerToConstRawData = const std::byte*;
using RawData = std::vector<std::byte>;
using RawDataView = std::span<const std::byte>;
using RawDataWriteableView = std::span<std::byte>;

enum class BufferProperties : uint32_t {
    None             = (1 << 0),
    DynamicStorage   = (1 << 1),
    ReadableFromCPU  = (1 << 2),
    WriteableFromCPU = (1 << 3),
    Persistent       = (1 << 4),
    Coherent         = (1 << 5),
    CPUStorage       = (1 << 6)
};
constexpr BufferProperties operator &(const BufferProperties lhs, const BufferProperties rhs) {
    return static_cast<BufferProperties>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr BufferProperties operator |(const BufferProperties lhs, enum BufferProperties rhs) {
    return static_cast<BufferProperties>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
constexpr GLenum ToGLBufferFlags(BufferProperties properties) noexcept {
    GLenum glBitmask = 0;
    if (static_cast<bool>(properties & BufferProperties::DynamicStorage))   glBitmask |= GL_DYNAMIC_STORAGE_BIT;
    if (static_cast<bool>(properties & BufferProperties::ReadableFromCPU))  glBitmask |= GL_MAP_READ_BIT;
    if (static_cast<bool>(properties & BufferProperties::WriteableFromCPU)) glBitmask |= GL_MAP_WRITE_BIT;
    if (static_cast<bool>(properties & BufferProperties::Persistent))       glBitmask |= GL_MAP_PERSISTENT_BIT;
    if (static_cast<bool>(properties & BufferProperties::Coherent))         glBitmask |= GL_MAP_COHERENT_BIT;
    if (static_cast<bool>(properties & BufferProperties::CPUStorage))       glBitmask |= GL_CLIENT_STORAGE_BIT;
    return glBitmask;
}

enum class ShaderType {
    Vertex,
    TessellationControl,
    TessellationEvaluation,
    Geometry,
    Fragment,
    Compute
};
constexpr std::string_view ToString(ShaderType type) noexcept {
    using enum ShaderType;
    switch (type) {
    case Vertex:                 return "Vertex";
    case TessellationControl:    return "Tessellation Control";
    case TessellationEvaluation: return "Tessellation Evaluation";
    case Geometry:               return "Geometry";
    case Fragment:               return "Fragment";
    case Compute:                return "Compute";
    }
    std::unreachable();
}

enum class TextureMinificationMode {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
};
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
constexpr std::string_view ToString(TextureMinificationMode mode) noexcept {
    using enum TextureMinificationMode;
    switch (mode) {
    case Nearest:               return "Nearest";
    case Linear:                return "Linear";
    case NearestMipmapNearest:  return "Nearest Mipmap Nearest";
    case LinearMipmapNearest:   return "Linear Mipmap Nearest";
    case NearestMipmapLinear:   return "Nearest Mipmap Linear";
    case LinearMipmapLinear:    return "Linear Mipmap Linear";
    }
    std::unreachable();
}

enum class TextureMagnificationMode {
    Nearest,
    Linear
};
constexpr GLenum ToGLTextureMagnificationMode(TextureMagnificationMode mode) noexcept {
    using enum TextureMagnificationMode;
    switch (mode) {
    case Nearest: return GL_NEAREST;
    case Linear:  return GL_LINEAR;
    }
    std::unreachable();
}
constexpr std::string_view ToString(TextureMagnificationMode mode) noexcept {
    using enum TextureMagnificationMode;
    switch (mode) {
    case Nearest: return "Nearest";
    case Linear:  return "Linear";
    }
    std::unreachable();
}

enum class TextureWrappingMode {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    MirrorClampToEdge,
    ClampToBorder
};
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
constexpr std::string_view ToString(TextureWrappingMode mode) noexcept {
    using enum TextureWrappingMode;
    switch (mode) {
    case Repeat:            return "Repeat";
    case MirroredRepeat:    return "Mirrored Repeat";
    case ClampToEdge:       return "Clamp To Edge";
    case MirrorClampToEdge: return "Mirror Clamp To Edge";
    case ClampToBorder:     return "Clamp To Border";
    }
    std::unreachable();
}

enum class TextureCompareMode {
    CompareRefToTexture,
    None
};
constexpr GLenum ToGLTextureCompareMode(TextureCompareMode mode) noexcept {
    using enum TextureCompareMode;
    switch (mode) {
    case CompareRefToTexture: return GL_COMPARE_REF_TO_TEXTURE;
    case None:                return GL_NONE;
    }
    std::unreachable();
}
constexpr std::string_view ToString(TextureCompareMode mode) noexcept {
    using enum TextureCompareMode;
    switch (mode) {
    case CompareRefToTexture: return "Compare Reference to Texture";
    case None:                return "None";
    }
    std::unreachable();
}

enum class TextureCompareFunction {
    LessEqual,
    GreaterEqual,
    Less,
    Greater,
    Equal,
    NotEqual,
    Always,
    Never
};
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
constexpr std::string_view ToString(TextureCompareFunction func) noexcept {
    using enum TextureCompareFunction;
    switch (func) {
    case LessEqual:    return "Less Equal";
    case GreaterEqual: return "Greater Equal";
    case Less:         return "Less";
    case Greater:      return "Greater";
    case Equal:        return "Equal";
    case NotEqual:     return "Not Equal";
    case Always:       return "Always";
    case Never:        return "Never";
    }
    std::unreachable();
}

enum class DataFormat {
    // Unsigned normalized formats
    R8,
    RG8,
    RGB8,
    RGBA8,
    R16,
    RG16,
    RGB16,
    RGBA16,
    R16F,
    RG16F,
    RGB16F,
    RGBA16F,
    R32F,
    RG32F,
    RGB32F,
    RGBA32F,
    // Signed normalized formats
    R8_SNORM,
    RG8_SNORM,
    RGB8_SNORM,
    RGBA8_SNORM,
    R16_SNORM,
    RG16_SNORM,
    RGB16_SNORM,
    RGBA16_SNORM,
    // Unsigned integer formats
    R8UI,
    RG8UI,
    RGB8UI,
    RGBA8UI,
    R16UI,
    RG16UI,
    RGB16UI,
    RGBA16UI,
    R32UI,
    RG32UI,
    RGB32UI,
    RGBA32UI,
    // Signed integer formats
    R8I,
    RG8I,
    RGB8I,
    RGBA8I,
    R16I,
    RG16I,
    RGB16I,
    RGBA16I,
    R32I,
    RG32I,
    RGB32I,
    RGBA32I,
    // Depth formats
    DEPTH_COMPONENT16,
    DEPTH_COMPONENT24,
    DEPTH_COMPONENT32,
    DEPTH32F,
    // Depth-stencil formats
    DEPTH24_STENCIL8,
    DEPTH32F_STENCIL8
};

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
    // Depth formats
    case DEPTH_COMPONENT16:  return GL_DEPTH_COMPONENT16;
    case DEPTH_COMPONENT24:  return GL_DEPTH_COMPONENT24;
    case DEPTH_COMPONENT32:  return GL_DEPTH_COMPONENT32;
    case DEPTH32F:           return GL_DEPTH_COMPONENT32F;
    // Depth-stencil formats
    case DEPTH24_STENCIL8:   return GL_DEPTH24_STENCIL8;
    case DEPTH32F_STENCIL8:  return GL_DEPTH32F_STENCIL8;
    default:                 std::unreachable();
    }
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
        return GL_RGBA;
    case DEPTH_COMPONENT16:
    case DEPTH_COMPONENT24:
    case DEPTH_COMPONENT32:
    case DEPTH32F:
        return GL_DEPTH_COMPONENT;
    case DEPTH24_STENCIL8:
    case DEPTH32F_STENCIL8:
        return GL_DEPTH_STENCIL;
    default:
        std::unreachable();
    }
}

constexpr std::string_view ToString(DataFormat format) {
    using enum DataFormat;
    switch (format) {
    // Unsigned normalized formats
    case R8:                 return "R8";
    case RG8:                return "RG8";
    case RGB8:               return "RGB8";
    case RGBA8:              return "RGBA8";
    case R16:                return "R16";
    case RG16:               return "RG16";
    case RGB16:              return "RGB16";
    case RGBA16:             return "RGBA16";
    case R16F:               return "R16F";
    case RG16F:              return "RG16F";
    case RGB16F:             return "RGB16F";
    case RGBA16F:            return "RGBA16F";
    case R32F:               return "R32F";
    case RG32F:              return "RG32F";
    case RGB32F:             return "RGB32F";
    case RGBA32F:            return "RGBA32F";
    // Signed normalized formats
    case R8_SNORM:           return "R8_SNORM";
    case RG8_SNORM:          return "RG8_SNORM";
    case RGB8_SNORM:         return "RGB8_SNORM";
    case RGBA8_SNORM:        return "RGBA8_SNORM";
    case R16_SNORM:          return "R16_SNORM";
    case RG16_SNORM:         return "RG16_SNORM";
    case RGB16_SNORM:        return "RGB16_SNORM";
    case RGBA16_SNORM:       return "RGBA16_SNORM";
    // Unsigned integer formats
    case R8UI:               return "R8UI";
    case RG8UI:              return "RG8UI";
    case RGB8UI:             return "RGB8UI";
    case RGBA8UI:            return "RGBA8UI";
    case R16UI:              return "R16UI";
    case RG16UI:             return "RG16UI";
    case RGB16UI:            return "RGB16UI";
    case RGBA16UI:           return "RGBA16UI";
    case R32UI:              return "R32UI";
    case RG32UI:             return "RG32UI";
    case RGB32UI:            return "RGB32UI";
    case RGBA32UI:           return "RGBA32UI";
    // Signed integer formats
    case R8I:                return "R8I";
    case RG8I:               return "RG8I";
    case RGB8I:              return "RGB8I";
    case RGBA8I:             return "RGBA8I";
    case R16I:               return "R16I";
    case RG16I:              return "RG16I";
    case RGB16I:             return "RGB16I";
    case RGBA16I:            return "RGBA16I";
    case R32I:               return "R32I";
    case RG32I:              return "RG32I";
    case RGB32I:             return "RGB32I";
    case RGBA32I:            return "RGBA32I";
    // Depth formats
    case DEPTH_COMPONENT16:  return "DEPTH_COMPONENT16";
    case DEPTH_COMPONENT24:  return "DEPTH_COMPONENT24";
    case DEPTH_COMPONENT32:  return "DEPTH_COMPONENT32";
    case DEPTH32F:           return "DEPTH32F";
    // Depth-stencil formats
    case DEPTH24_STENCIL8:   return "DEPTH24_STENCIL8";
    case DEPTH32F_STENCIL8:  return "DEPTH32F_STENCIL8";
    default:                 std::unreachable();
    }
}