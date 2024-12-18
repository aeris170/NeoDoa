#pragma once

#include <span>
#include <array>
#include <memory>
#include <vector>
#include <cstddef>
#include <ostream>
#include <utility>
#include <optional>
#include <string_view>

#include <GL/glew.h>

#include <Engine/DataTypes.hpp>

struct Region;
struct GPUBuffer;        struct GPUBufferBuilder;
struct GPUShader;        struct GPUShaderBuilder;
struct GPUTexture;       struct GPUTextureBuilder;
struct GPUSampler;       struct GPUSamplerBuilder;
struct GPUPipeline;      struct GPUPipelineBuilder;
struct GPUFrameBuffer;   struct GPUFrameBufferBuilder;
struct GPURenderBuffer;  struct GPURenderBufferBuilder;
struct GPUShaderProgram; struct GPUShaderProgramBuilder;
struct GPUDescriptorSet; struct GPUDescriptorSetBuilder;

#pragma region Graphics Messages
using BufferAllocatorMessage = std::string;

using DescriptorSetAllocatorMessage = std::string;

using RenderBufferAllocatorMessage = std::string;

using FrameBufferAllocatorMessage = std::string;

using PipelineAllocatorMessage = std::string;

struct ShaderCompilerMessage {
    int LineNo;
    enum class Type {
        Info,
        Warning,
        Error
    } MessageType;
    std::string ShortMessage, FullMessage;
};
using ShaderLinkerMessage = std::string;

using SamplerAllocatorMessage = std::string;

using TextureAllocatorMessage = std::string;
#pragma endregion

#pragma region Constants
constexpr uint8_t MaxDescriptorBinding = 16;

constexpr uint8_t MaxFrameBufferColorAttachments = 8;

constexpr uint8_t MaxVertexBufferBinding = 8;

constexpr uint8_t MaxVertexAttributes = 16;
#pragma endregion

enum class GraphicsBackend {
    None,
    Software,
#ifdef OPENGL_4_6_SUPPORT
    OpenGL4_6,
#endif
#ifdef OPENGL_3_3_SUPPORT
    OpenGL3_3,  // TODO Implement this
#endif
#ifdef VULKAN_SUPPORT
    Vulkan,     // TODO Implement this
#endif
#ifdef DIRECT3D_12_SUPPORT
    Direct3D12, // TODO Implement this
#endif
#ifdef DIRECT3D_11_SUPPORT
    Direct3D11  // TODO Implement this
#endif
};

#define ND_GRAPHICS_COPYABLE_MOVEABLE_RESOURCE(x) \
    x() noexcept = default;                       \
    ~x() noexcept;                                \
    x(const x& other) noexcept;                   \
    x(x&& other) noexcept;                        \
    x& operator=(const x& other) noexcept;        \
    x& operator=(x&& other) noexcept

#define ND_GRAPHICS_MOVE_ONLY_RESOURCE(x) \
    x() noexcept = default;               \
    ~x() noexcept;                        \
    x(const x&) = delete;                 \
    x(x&& other) noexcept;                \
    x& operator=(const x&) = delete;      \
    x& operator=(x&& other) noexcept

#define ND_GRAPHICS_BUILDER_RULE_OF_0(x) \
    x() = default;                       \
    ~x() = default;                      \
    x(const x&) = delete;                \
    x& operator=(const x&) = delete;     \
    x(x&&) = delete;                     \
    x& operator=(x&&) = delete

#pragma region Enumerations
enum class BufferProperties : uint32_t {
    None = (1 << 0),
    DynamicStorage = (1 << 1),
    ReadableFromCPU = (1 << 2),
    WriteableFromCPU = (1 << 3),
    Persistent = (1 << 4),
    Coherent = (1 << 5),
    CPUStorage = (1 << 6)
};
constexpr BufferProperties operator &(const BufferProperties lhs, const BufferProperties rhs) {
    return static_cast<BufferProperties>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr BufferProperties operator |(const BufferProperties lhs, enum BufferProperties rhs) {
    return static_cast<BufferProperties>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

enum class ShaderType {
    Vertex,
    TessellationControl,
    TessellationEvaluation,
    Geometry,
    Fragment,
    Compute
};
enum class TextureMinificationMode {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
};
enum class TextureMagnificationMode {
    Nearest,
    Linear
};
enum class TextureWrappingMode {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    MirrorClampToEdge,
    ClampToBorder
};
enum class TextureCompareMode {
    CompareRefToTexture,
    None
};
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
enum class Multisample : uint8_t {
    None = 1,
    x2 = 2,
    x4 = 4,
    x8 = 8
};
enum class DataFormat {
    // Unsigned unified formats
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
    // Signed normalized unified formats
    R8_SNORM,
    RG8_SNORM,
    RGB8_SNORM,
    RGBA8_SNORM,
    R16_SNORM,
    RG16_SNORM,
    RGB16_SNORM,
    RGBA16_SNORM,
    // Unsigned integer unified formats
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
    // Signed integer unified formats
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
    // SRGB Formats
    SRGB8,
    SRGBA8,

    // Depth formats
    DEPTH16,
    DEPTH24,
    DEPTH32,
    DEPTH32F,
    // Stencil Formats
    STENCIL1,
    STENCIL4,
    STENCIL8,
    STENCIL16,
    // Depth-stencil formats
    DEPTH24_STENCIL8,
    DEPTH32F_STENCIL8,

    // Split Formats
    R3G3B2,
    RGB5A1,
    RGB10A2,
    RGB10A2UI,
    R11FG11FB10F,
    RGB9E5,
    // Other Formats (when do they even get used?)
    RGB4,
    RGB5,
    RGB565,
    RGB10,
    RGB12,
    RGBA2,
    RGBA4,
    RGBA12
};
enum class TopologyType {
    Points,
    Lines,
    LineStrip,
    LineLoop,
    Triangles,
    TriangleStrip,
    TriangleFan
};
enum class DataType {
    Byte,
    UnsignedByte,
    Short,
    UnsignedShort,
    Int,
    UnsignedInt,
    Float,
    Double
};
enum class PolygonMode {
    Fill,
    Line,
    Point
};
enum class CullMode {
    Front,
    Back,
    FrontAndBack
};
enum class DepthFunction {
    Always,
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    GreaterEqual,
    NotEqual,
};
enum class BlendFactor {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate,
};
enum class InputRate {
    PerVertex,
    PerInstance
};
#pragma endregion

#define GRAPHICS_FUNCTIONS(base, builders, destructors)                                                                                                                         \
namespace base {                                                                                                                                                                \
    void BufferSubData(GPUBuffer& buffer, RawDataView dataView, size_t offsetBytes = 0uLL) noexcept;                                                                            \
    void BufferSubData(GPUBuffer& buffer, size_t sizeBytes, NonOwningPointerToConstRawData data, size_t offsetBytes = 0uLL) noexcept;                                           \
    void GetBufferSubData(const GPUBuffer& buffer, RawDataWriteableView dataView, size_t offsetBytes = 0uLL) noexcept;                                                          \
    void CopyBufferSubData(const GPUBuffer& readBuffer, GPUBuffer& writeBuffer, size_t sizeBytesToCopy, size_t readOffsetBytes = 0uLL, size_t writeOffsetBytes = 0uLL) noexcept;\
    void ClearBufferSubData(GPUBuffer& buffer, DataFormat format, size_t sizeBytesToClear, size_t offsetBytes = 0uLL) noexcept;                                                 \
                                                                                                                                                                                \
    void Blit(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept;                                                                                              \
    void BlitColor(const GPUFrameBuffer& source, GPUFrameBuffer& destination, unsigned srcAttachment, std::span<unsigned> dstAttachments) noexcept;                             \
    void BlitDepth(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept;                                                                                         \
    void BlitStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept;                                                                                       \
    void BlitDepthStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept;                                                                                  \
                                                                                                                                                                                \
    void Render(int count, int first = 0) noexcept;                                                                                                                             \
    void RenderInstanced(int instanceCount, int count, int first = 0) noexcept;                                                                                                 \
                                                                                                                                                                                \
    void SetRenderTarget(const GPUFrameBuffer& renderTarget) noexcept;                                                                                                          \
    void SetRenderTarget(const GPUFrameBuffer& renderTarget, std::span<unsigned> targets) noexcept;                                                                             \
    void ClearRenderTargetColor(const GPUFrameBuffer& renderTarget, std::array<float, 4> color = { 0, 0, 0, 0 }, unsigned colorBufferIndex = 0) noexcept;                       \
    void ClearRenderTargetColors(const GPUFrameBuffer& renderTarget, std::array<float, 4> color = { 0, 0, 0, 0 }) noexcept;                                                     \
    void ClearRenderTargetDepth(const GPUFrameBuffer& renderTarget, float depth = 1) noexcept;                                                                                  \
    void ClearRenderTargetStencil(const GPUFrameBuffer& renderTarget, int stencil = 0) noexcept;                                                                                \
    void ClearRenderTarget(const GPUFrameBuffer& renderTarget, std::array<float, 4> color = { 0, 0, 0, 0 }, float depth = 1, int stencil = 0) noexcept;                         \
                                                                                                                                                                                \
    void BindPipeline(const GPUPipeline& pipeline) noexcept;                                                                                                                    \
                                                                                                                                                                                \
    void BindDescriptorSet(const GPUDescriptorSet& descriptorSet) noexcept;                                                                                                     \
}                                                                                                                                                                               \
namespace builders {                                                                                                                                                            \
    [[nodiscard]] std::pair<std::optional<GPUBuffer>,        std::vector<BufferAllocatorMessage>>        Build(GPUBufferBuilder& builder) noexcept;                             \
    [[nodiscard]] std::pair<std::optional<GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>> Build(GPUDescriptorSetBuilder& builder) noexcept;                      \
    [[nodiscard]] std::pair<std::optional<GPURenderBuffer>,  std::vector<RenderBufferAllocatorMessage>>  Build(GPURenderBufferBuilder& builder) noexcept;                       \
    [[nodiscard]] std::pair<std::optional<GPUFrameBuffer>,   std::vector<FrameBufferAllocatorMessage>>   Build(GPUFrameBufferBuilder& builder) noexcept;                        \
    [[nodiscard]] std::pair<std::optional<GPUPipeline>,      std::vector<PipelineAllocatorMessage>>      Build(GPUPipelineBuilder& builder) noexcept;                           \
    [[nodiscard]] std::pair<std::optional<GPUShader>,        std::vector<ShaderCompilerMessage>>         Build(GPUShaderBuilder& builder) noexcept;                             \
    [[nodiscard]] std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>>           Build(GPUShaderProgramBuilder& builder) noexcept;                      \
    [[nodiscard]] std::pair<std::optional<GPUSampler>,       std::vector<SamplerAllocatorMessage>>       Build(GPUSamplerBuilder& builder) noexcept;                            \
    [[nodiscard]] std::pair<std::optional<GPUTexture>,       std::vector<TextureAllocatorMessage>>       Build(GPUTextureBuilder& builder) noexcept;                            \
}                                                                                                                                                                               \
namespace destructors {                                                                                                                                                         \
    void Destruct(GPUBuffer& buffer) noexcept;                                                                                                                                  \
    void Destruct(GPUDescriptorSet& builder) noexcept;                                                                                                                          \
    void Destruct(GPURenderBuffer& renderbuffer) noexcept;                                                                                                                      \
    void Destruct(GPUFrameBuffer& framebuffer) noexcept;                                                                                                                        \
    void Destruct(GPUPipeline& pipeline) noexcept;                                                                                                                              \
    void Destruct(GPUShader& shader) noexcept;                                                                                                                                  \
    void Destruct(GPUShaderProgram& program) noexcept;                                                                                                                          \
    void Destruct(GPUSampler& sampler) noexcept;                                                                                                                                \
    void Destruct(GPUTexture& texture) noexcept;                                                                                                                                \
}

namespace Graphics {
    void ChangeGraphicsBackend(GraphicsBackend backend) noexcept;
}

GRAPHICS_FUNCTIONS(Graphics, Graphics::Builders, Graphics::Destructors)
GRAPHICS_FUNCTIONS(Graphics::None, Graphics::None, Graphics::None)
GRAPHICS_FUNCTIONS(Graphics::Software, Graphics::Software, Graphics::Software)
#ifdef OPENGL_4_6_SUPPORT
GRAPHICS_FUNCTIONS(Graphics::OpenGL, Graphics::OpenGL, Graphics::OpenGL)
namespace Graphics::OpenGL {
    void Initialize() noexcept;
}
#endif
#ifdef OPENGL_3_3_SUPPORT
GRAPHICS_FUNCTIONS(Graphics::OpenGL33, Graphics::OpenGL33, Graphics::OpenGL33)
#endif
#ifdef VULKAN_SUPPORT
GRAPHICS_FUNCTIONS(Graphics::Vulkan, Graphics::Vulkan, Graphics::Vulkan)
#endif
#ifdef DIRECT3D_12_SUPPORT
GRAPHICS_FUNCTIONS(Graphics::Direct3D12, Graphics::Direct3D12, Graphics::Direct3D12)
#endif
#ifdef DIRECT3D_11_SUPPORT
GRAPHICS_FUNCTIONS(Graphics::Direct3D11, Graphics::Direct3D11, Graphics::Direct3D11)
#endif

constexpr std::string_view ToString(GraphicsBackend backend) noexcept {
    using enum GraphicsBackend;
    switch (backend) {
    case None:       return "None";
    case Software:   return "Software";
#ifdef OPENGL_4_6_SUPPORT
    case OpenGL4_6:  return "OpenGL 4.6";
#endif
#ifdef OPENGL_3_3_SUPPORT
    case OpenGL3_3:  return "OpenGL 3.3";
#endif
#ifdef VULKAN_SUPPORT
    case Vulkan:     return "Vulkan";
#endif
#ifdef DIRECT3D_12_SUPPORT
    case Direct3D12: return "Direct3D 12";
#endif
#ifdef DIRECT3D_11_SUPPORT
    case Direct3D11: return "Direct3D 11";
#endif
    }
    std::unreachable();
}
constexpr std::string ToString(BufferProperties properties) noexcept {
    if (static_cast<uint32_t>(properties) == 0) { return "None"; }

    std::string result;
    using enum BufferProperties;
    if (static_cast<bool>(properties & DynamicStorage))   result = "DynamicStorage";
    if (static_cast<bool>(properties & ReadableFromCPU))  result += (result.empty() ? "" : " | ") + std::string("ReadableFromCPU");
    if (static_cast<bool>(properties & WriteableFromCPU)) result += (result.empty() ? "" : " | ") + std::string("WriteableFromCPU");
    if (static_cast<bool>(properties & Persistent))       result += (result.empty() ? "" : " | ") + std::string("Persistent");
    if (static_cast<bool>(properties & Coherent))         result += (result.empty() ? "" : " | ") + std::string("Coherent");
    if (static_cast<bool>(properties & CPUStorage))       result += (result.empty() ? "" : " | ") + std::string("CPUStorage");

    return result;
}
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
constexpr std::string_view ToString(TextureMagnificationMode mode) noexcept {
    using enum TextureMagnificationMode;
    switch (mode) {
    case Nearest: return "Nearest";
    case Linear:  return "Linear";
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
constexpr std::string_view ToString(TextureCompareMode mode) noexcept {
    using enum TextureCompareMode;
    switch (mode) {
    case CompareRefToTexture: return "Compare Reference to Texture";
    case None:                return "None";
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
constexpr std::string_view ToString(Multisample ms) noexcept {
    using enum Multisample;
    switch (ms) {
    case None: return "No MS";
    case x2:   return "2x MS";
    case x4:   return "4x MS";
    case x8:   return "8x MS";
    }
    std::unreachable();
}
constexpr std::string_view ToString(DataFormat format) {
    using enum DataFormat;
    switch (format) {
        // Unsigned unified formats
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
        // Signed normalized unified formats
    case R8_SNORM:           return "R8_SNORM";
    case RG8_SNORM:          return "RG8_SNORM";
    case RGB8_SNORM:         return "RGB8_SNORM";
    case RGBA8_SNORM:        return "RGBA8_SNORM";
    case R16_SNORM:          return "R16_SNORM";
    case RG16_SNORM:         return "RG16_SNORM";
    case RGB16_SNORM:        return "RGB16_SNORM";
    case RGBA16_SNORM:       return "RGBA16_SNORM";
        // Unsigned integer unified formats
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
        // Signed integer unified formats
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
        // SRGB Formats
    case SRGB8:              return "sRGB8";
    case SRGBA8:             return "sRGBA8";
        // Depth formats
    case DEPTH16:            return "DEPTH16";
    case DEPTH24:            return "DEPTH24";
    case DEPTH32:            return "DEPTH32";
    case DEPTH32F:           return "DEPTH32F";
        // Stencil formats
    case STENCIL1:           return "STENCIL1";
    case STENCIL4:           return "STENCIL4";
    case STENCIL8:           return "STENCIL8";
    case STENCIL16:          return "STENCIL16";
        // Depth-stencil formats
    case DEPTH24_STENCIL8:   return "DEPTH24_STENCIL8";
    case DEPTH32F_STENCIL8:  return "DEPTH32F_STENCIL8";
        // Split Formats
    case R3G3B2:             return "R3G3B2";
    case RGB5A1:             return "RGB5A1";
    case RGB10A2:            return "RGB10A2";
    case RGB10A2UI:          return "RGB10A2UI";
    case R11FG11FB10F:       return "R11FG11FB10F";
    case RGB9E5:             return "RGB9E5";
        // Other Formats (when do they even get used?)
    case RGB4:               return "RGB4";
    case RGB5:               return "RGB5";
    case RGB565:             return "RGB565";
    case RGB10:              return "RGB10";
    case RGB12:              return "RGB12";
    case RGBA2:              return "RGBA2";
    case RGBA4:              return "RGBA4";
    case RGBA12:             return "RGBA12";
    }
    std::unreachable();
}
constexpr std::string_view ToString(TopologyType t) noexcept {
    using enum TopologyType;
    switch (t) {
    case Points:        return "Points";
    case Lines:         return "Lines";
    case LineStrip:     return "Line Strip";
    case LineLoop:      return "Line Loop";
    case Triangles:     return "Triangles";
    case TriangleStrip: return "Triangle Strip";
    case TriangleFan:   return "Triangle Fan";
    }
    std::unreachable();
}
constexpr std::string_view ToString(DataType t) noexcept {
    using enum DataType;
    switch (t) {
    case Byte:          return "Byte";
    case UnsignedByte:  return "Unsigned Byte";
    case Short:         return "Short";
    case UnsignedShort: return "Unsigned Short";
    case Int:           return "Int";
    case UnsignedInt:   return "Unsigned Int";
    case Float:         return "Float";
    case Double:        return "Double";
    }
    std::unreachable();
}
constexpr std::string_view ToString(PolygonMode mode) noexcept {
    using enum PolygonMode;
    switch (mode) {
    case Fill:  return "Fill";
    case Line:  return "Line";
    case Point: return "Point";
    }
    std::unreachable();
}
constexpr std::string_view ToString(CullMode mode) noexcept {
    using enum CullMode;
    switch (mode) {
    case Front:         return "Front";
    case Back:          return "Back";
    case FrontAndBack:  return "Front And Back";
    }
    std::unreachable();
}
constexpr std::string_view ToString(DepthFunction mode) noexcept {
    using enum DepthFunction;
    switch (mode) {
    case Always:       return "Always";
    case Never:        return "Never";
    case Less:         return "Less";
    case Equal:        return "Equal";
    case LessEqual:    return "Less Equal";
    case Greater:      return "Greater";
    case GreaterEqual: return "Greater Equal";
    case NotEqual:     return "Not Equal";
    }
    std::unreachable();
}
constexpr std::string_view ToString(BlendFactor factor) noexcept {
    using enum BlendFactor;
    switch (factor) {
    case Zero:                  return "Zero";
    case One:                   return "One";
    case SrcColor:              return "Src Color";
    case OneMinusSrcColor:      return "One Minus Src Color";
    case DstColor:              return "Dst Color";
    case OneMinusDstColor:      return "One Minus Dst Color";
    case SrcAlpha:              return "Src Alpha";
    case OneMinusSrcAlpha:      return "One Minus Src Alpha";
    case DstAlpha:              return "Dst Alpha";
    case OneMinusDstAlpha:      return "One Minus Dst Alpha";
    case ConstantColor:         return "Constant Color";
    case OneMinusConstantColor: return "One Minus Constant Color";
    case ConstantAlpha:         return "Constant Alpha";
    case OneMinusConstantAlpha: return "One Minus Constant Alpha";
    case SrcAlphaSaturate:      return "Src Alpha Saturate";
    }
    std::unreachable();
}
constexpr std::string_view ToString(InputRate rate) noexcept {
    using enum InputRate;
    switch (rate) {
    case PerVertex:   return "Per Vertex";
    case PerInstance: return "Per Instance";
    }
    std::unreachable();
}

std::ostream& operator<<(std::ostream& os, GraphicsBackend backend);
std::ostream& operator<<(std::ostream& os, BufferProperties property);
std::ostream& operator<<(std::ostream& os, ShaderType type);
std::ostream& operator<<(std::ostream& os, TextureMinificationMode mode);
std::ostream& operator<<(std::ostream& os, TextureMagnificationMode mode);
std::ostream& operator<<(std::ostream& os, TextureWrappingMode mode);
std::ostream& operator<<(std::ostream& os, TextureCompareMode mode);
std::ostream& operator<<(std::ostream& os, TextureCompareFunction func);
std::ostream& operator<<(std::ostream& os, Multisample sample);
std::ostream& operator<<(std::ostream& os, DataFormat format);
std::ostream& operator<<(std::ostream& os, TopologyType type);
std::ostream& operator<<(std::ostream& os, DataType type);
std::ostream& operator<<(std::ostream& os, PolygonMode mode);
std::ostream& operator<<(std::ostream& os, CullMode mode);
std::ostream& operator<<(std::ostream& os, DepthFunction func);
std::ostream& operator<<(std::ostream& os, BlendFactor factor);
std::ostream& operator<<(std::ostream& os, InputRate rate);

constexpr GraphicsBackend FromString(std::string_view str) noexcept {
    if (str == "None")             return GraphicsBackend::None;
    if (str == "Software")         return GraphicsBackend::Software;
#ifdef OPENGL_4_6_SUPPORT
    if (str == "OpenGL 4.6")       return GraphicsBackend::OpenGL4_6;
#endif
#ifdef OPENGL_3_3_SUPPORT
    if (str == "OpenGL 3.3")       return GraphicsBackend::OpenGL3_3;
#endif
#ifdef VULKAN_SUPPORT
    if (str == "Vulkan")           return GraphicsBackend::Vulkan;
#endif
#ifdef DIRECT3D_12_SUPPORT
    if (str == "Direct3D 12")      return GraphicsBackend::Direct3D12;
#endif
#ifdef DIRECT3D_11_SUPPORT
    if (str == "Direct3D 11")      return GraphicsBackend::Direct3D11;
#endif
    std::unreachable();
}

constexpr BufferProperties BufferPropertiesFromString(std::string_view str) noexcept {
    if (str == "None") return BufferProperties::None;

    BufferProperties result = static_cast<BufferProperties>(0);
    if (str.find("DynamicStorage")   != std::string_view::npos) { result = result | BufferProperties::DynamicStorage;   }
    if (str.find("ReadableFromCPU")  != std::string_view::npos) { result = result | BufferProperties::ReadableFromCPU;  }
    if (str.find("WriteableFromCPU") != std::string_view::npos) { result = result | BufferProperties::WriteableFromCPU; }
    if (str.find("Persistent")       != std::string_view::npos) { result = result | BufferProperties::Persistent;       }
    if (str.find("Coherent")         != std::string_view::npos) { result = result | BufferProperties::Coherent;         }
    if (str.find("CPUStorage")       != std::string_view::npos) { result = result | BufferProperties::CPUStorage;       }

    return result;
}

constexpr ShaderType ShaderTypeFromString(std::string_view str) noexcept {
    if (str == "Vertex")                  { return ShaderType::Vertex;                 }
    if (str == "Tessellation Control")    { return ShaderType::TessellationControl;    }
    if (str == "Tessellation Evaluation") { return ShaderType::TessellationEvaluation; }
    if (str == "Geometry")                { return ShaderType::Geometry;               }
    if (str == "Fragment")                { return ShaderType::Fragment;               }
    if (str == "Compute")                 { return ShaderType::Compute;                }
    std::unreachable();
}

constexpr TextureMinificationMode TextureMinificationModeFromString(std::string_view str) noexcept {
    if (str == "Nearest")                { return TextureMinificationMode::Nearest;              }
    if (str == "Linear")                 { return TextureMinificationMode::Linear;               }
    if (str == "Nearest Mipmap Nearest") { return TextureMinificationMode::NearestMipmapNearest; }
    if (str == "Linear Mipmap Nearest")  { return TextureMinificationMode::LinearMipmapNearest;  }
    if (str == "Nearest Mipmap Linear")  { return TextureMinificationMode::NearestMipmapLinear;  }
    if (str == "Linear Mipmap Linear")   { return TextureMinificationMode::LinearMipmapLinear;   }
    std::unreachable();
}

constexpr TextureMagnificationMode TextureMagnificationModeFromString(std::string_view str) noexcept {
    if (str == "Nearest") { return TextureMagnificationMode::Nearest; }
    if (str == "Linear")  { return TextureMagnificationMode::Linear;  }
    std::unreachable();
}

constexpr TextureWrappingMode TextureWrappingModeFromString(std::string_view str) noexcept {
    if (str == "Repeat")               { return TextureWrappingMode::Repeat;            }
    if (str == "Mirrored Repeat")      { return TextureWrappingMode::MirroredRepeat;    }
    if (str == "Clamp To Edge")        { return TextureWrappingMode::ClampToEdge;       }
    if (str == "Mirror Clamp To Edge") { return TextureWrappingMode::MirrorClampToEdge; }
    if (str == "Clamp To Border")      { return TextureWrappingMode::ClampToBorder;     }
    std::unreachable();
}

constexpr TextureCompareMode TextureCompareModeFromString(std::string_view str) noexcept {
    if (str == "Compare Reference to Texture") { return TextureCompareMode::CompareRefToTexture; }
    if (str == "None")                         { return TextureCompareMode::None;                }
    std::unreachable();
}

constexpr TextureCompareFunction TextureCompareFunctionFromString(std::string_view str) noexcept {
    if (str == "Less Equal")    { return TextureCompareFunction::LessEqual;    }
    if (str == "Greater Equal") { return TextureCompareFunction::GreaterEqual; }
    if (str == "Less")          { return TextureCompareFunction::Less;         }
    if (str == "Greater")       { return TextureCompareFunction::Greater;      }
    if (str == "Equal")         { return TextureCompareFunction::Equal;        }
    if (str == "Not Equal")     { return TextureCompareFunction::NotEqual;     }
    if (str == "Always")        { return TextureCompareFunction::Always;       }
    if (str == "Never")         { return TextureCompareFunction::Never;        }
    std::unreachable();
}

constexpr Multisample MultisampleFromString(std::string_view str) noexcept {
    if (str == "No MS") { return Multisample::None; }
    if (str == "2x MS") { return Multisample::x2;   }
    if (str == "4x MS") { return Multisample::x4;   }
    if (str == "8x MS") { return Multisample::x8;   }
    std::unreachable();
}

constexpr DataFormat DataFormatFromString(std::string_view str) noexcept {
    using enum DataFormat;
    if (str == "R8")                 return R8;
    if (str == "RG8")                return RG8;
    if (str == "RGB8")               return RGB8;
    if (str == "RGBA8")              return RGBA8;
    if (str == "R16")                return R16;
    if (str == "RG16")               return RG16;
    if (str == "RGB16")              return RGB16;
    if (str == "RGBA16")             return RGBA16;
    if (str == "R16F")               return R16F;
    if (str == "RG16F")              return RG16F;
    if (str == "RGB16F")             return RGB16F;
    if (str == "RGBA16F")            return RGBA16F;
    if (str == "R32F")               return R32F;
    if (str == "RG32F")              return RG32F;
    if (str == "RGB32F")             return RGB32F;
    if (str == "RGBA32F")            return RGBA32F;

    if (str == "R8_SNORM")           return R8_SNORM;
    if (str == "RG8_SNORM")          return RG8_SNORM;
    if (str == "RGB8_SNORM")         return RGB8_SNORM;
    if (str == "RGBA8_SNORM")        return RGBA8_SNORM;
    if (str == "R16_SNORM")          return R16_SNORM;
    if (str == "RG16_SNORM")         return RG16_SNORM;
    if (str == "RGB16_SNORM")        return RGB16_SNORM;
    if (str == "RGBA16_SNORM")       return RGBA16_SNORM;

    if (str == "R8UI")               return R8UI;
    if (str == "RG8UI")              return RG8UI;
    if (str == "RGB8UI")             return RGB8UI;
    if (str == "RGBA8UI")            return RGBA8UI;
    if (str == "R16UI")              return R16UI;
    if (str == "RG16UI")             return RG16UI;
    if (str == "RGB16UI")            return RGB16UI;
    if (str == "RGBA16UI")           return RGBA16UI;
    if (str == "R32UI")              return R32UI;
    if (str == "RG32UI")             return RG32UI;
    if (str == "RGB32UI")            return RGB32UI;
    if (str == "RGBA32UI")           return RGBA32UI;

    if (str == "R8I")                return R8I;
    if (str == "RG8I")               return RG8I;
    if (str == "RGB8I")              return RGB8I;
    if (str == "RGBA8I")             return RGBA8I;
    if (str == "R16I")               return R16I;
    if (str == "RG16I")              return RG16I;
    if (str == "RGB16I")             return RGB16I;
    if (str == "RGBA16I")            return RGBA16I;
    if (str == "R32I")               return R32I;
    if (str == "RG32I")              return RG32I;
    if (str == "RGB32I")             return RGB32I;
    if (str == "RGBA32I")            return RGBA32I;

    if (str == "sRGB8")              return SRGB8;
    if (str == "sRGBA8")             return SRGBA8;

    if (str == "DEPTH16")            return DEPTH16;
    if (str == "DEPTH24")            return DEPTH24;
    if (str == "DEPTH32")            return DEPTH32;
    if (str == "DEPTH32F")           return DEPTH32F;

    if (str == "STENCIL1")           return STENCIL1;
    if (str == "STENCIL4")           return STENCIL4;
    if (str == "STENCIL8")           return STENCIL8;
    if (str == "STENCIL16")          return STENCIL16;

    if (str == "DEPTH24_STENCIL8")   return DEPTH24_STENCIL8;
    if (str == "DEPTH32F_STENCIL8")  return DEPTH32F_STENCIL8;

    if (str == "R3G3B2")             return R3G3B2;
    if (str == "RGB5A1")             return RGB5A1;
    if (str == "RGB10A2")            return RGB10A2;
    if (str == "RGB10A2UI")          return RGB10A2UI;
    if (str == "R11FG11FB10F")       return R11FG11FB10F;
    if (str == "RGB9E5")             return RGB9E5;

    if (str == "RGB4")               return RGB4;
    if (str == "RGB5")               return RGB5;
    if (str == "RGB565")             return RGB565;
    if (str == "RGB10")              return RGB10;
    if (str == "RGB12")              return RGB12;
    if (str == "RGBA2")              return RGBA2;
    if (str == "RGBA4")              return RGBA4;
    if (str == "RGBA12")             return RGBA12;

    std::unreachable();
}

constexpr TopologyType TopologyTypeFromString(std::string_view str) noexcept {
    if (str == "Points")         { return TopologyType::Points;        }
    if (str == "Lines")          { return TopologyType::Lines;         }
    if (str == "Line Strip")     { return TopologyType::LineStrip;     }
    if (str == "Line Loop")      { return TopologyType::LineLoop;      }
    if (str == "Triangles")      { return TopologyType::Triangles;     }
    if (str == "Triangle Strip") { return TopologyType::TriangleStrip; }
    if (str == "Triangle Fan")   { return TopologyType::TriangleFan;   }
    std::unreachable();
}

constexpr DataType DataTypeFromString(std::string_view str) noexcept {
    if (str == "Byte")           { return DataType::Byte;          }
    if (str == "Unsigned Byte")  { return DataType::UnsignedByte;  }
    if (str == "Short")          { return DataType::Short;         }
    if (str == "Unsigned Short") { return DataType::UnsignedShort; }
    if (str == "Int")            { return DataType::Int;           }
    if (str == "Unsigned Int")   { return DataType::UnsignedInt;   }
    if (str == "Float")          { return DataType::Float;         }
    if (str == "Double")         { return DataType::Double;        }
    std::unreachable();
}

constexpr PolygonMode PolygonModeFromString(std::string_view str) noexcept {
    if (str == "Fill")  { return PolygonMode::Fill;  }
    if (str == "Line")  { return PolygonMode::Line;  }
    if (str == "Point") { return PolygonMode::Point; }
    std::unreachable();
}

constexpr CullMode CullModeFromString(std::string_view str) noexcept {
    if (str == "Front")          { return CullMode::Front;        }
    if (str == "Back")           { return CullMode::Back;         }
    if (str == "Front And Back") { return CullMode::FrontAndBack; }
    std::unreachable();
}

constexpr DepthFunction DepthFunctionFromString(std::string_view str) noexcept {
    if (str == "Always")        { return DepthFunction::Always;       }
    if (str == "Never")         { return DepthFunction::Never;        }
    if (str == "Less")          { return DepthFunction::Less;         }
    if (str == "Equal")         { return DepthFunction::Equal;        }
    if (str == "Less Equal")    { return DepthFunction::LessEqual;    }
    if (str == "Greater")       { return DepthFunction::Greater;      }
    if (str == "Greater Equal") { return DepthFunction::GreaterEqual; }
    if (str == "Not Equal")     { return DepthFunction::NotEqual;     }
    std::unreachable();
}

constexpr BlendFactor BlendFactorFromString(std::string_view str) noexcept {
    if (str == "Zero")                     { return BlendFactor::Zero;                  }
    if (str == "One")                      { return BlendFactor::One;                   }
    if (str == "Src Color")                { return BlendFactor::SrcColor;              }
    if (str == "One Minus Src Color")      { return BlendFactor::OneMinusSrcColor;      }
    if (str == "Dst Color")                { return BlendFactor::DstColor;              }
    if (str == "One Minus Dst Color")      { return BlendFactor::OneMinusDstColor;      }
    if (str == "Src Alpha")                { return BlendFactor::SrcAlpha;              }
    if (str == "One Minus Src Alpha")      { return BlendFactor::OneMinusSrcAlpha;      }
    if (str == "Dst Alpha")                { return BlendFactor::DstAlpha;              }
    if (str == "One Minus Dst Alpha")      { return BlendFactor::OneMinusDstAlpha;      }
    if (str == "Constant Color")           { return BlendFactor::ConstantColor;         }
    if (str == "One Minus Constant Color") { return BlendFactor::OneMinusConstantColor; }
    if (str == "Constant Alpha")           { return BlendFactor::ConstantAlpha;         }
    if (str == "One Minus Constant Alpha") { return BlendFactor::OneMinusConstantAlpha; }
    if (str == "Src Alpha Saturate")       { return BlendFactor::SrcAlphaSaturate;  }
    std::unreachable();
}

constexpr InputRate InputRateFromString(std::string_view str) noexcept {
    if (str == "Per Vertex")   { return InputRate::PerVertex;   }
    if (str == "Per Instance") { return InputRate::PerInstance; }
    std::unreachable();
}
