#include <Engine/Graphics.hpp>

#include <cassert>
#include <algorithm>
#include <functional>

#include <Utility/TemplateUtilities.hpp>

#include <Engine/Core.hpp>
#include <Engine/GraphicsGL.hpp>

#include <Engine/GPUShader.hpp>
#include <Engine/GPUBuffer.hpp>
#include <Engine/GPUFrameBuffer.hpp>
#include <Engine/GPUPipeline.hpp>
#include <Engine/GPUDescriptorSet.hpp>

namespace {
    GraphicsBackend currentBackend = static_cast<GraphicsBackend>(-1);

    std::function<void(GPUBuffer&, size_t, NonOwningPointerToConstRawData, size_t)> bufferSubData;
    std::function<void(const GPUBuffer&, RawDataWriteableView, size_t)> getBufferSubData;
    std::function<void(const GPUBuffer&, GPUBuffer&, size_t, size_t, size_t)> copyBufferSubData;
    std::function<void(GPUBuffer&, DataFormat, size_t, size_t)> clearBufferSubData;

    std::function<void(const GPUFrameBuffer&, GPUFrameBuffer&)> blit;
    std::function<void(const GPUFrameBuffer&, GPUFrameBuffer&, unsigned, std::span<unsigned>)> blitColor;
    std::function<void(const GPUFrameBuffer&, GPUFrameBuffer&)> blitDepth;
    std::function<void(const GPUFrameBuffer&, GPUFrameBuffer&)> blitStencil;
    std::function<void(const GPUFrameBuffer&, GPUFrameBuffer&)> blitDepthStencil;

    std::function<void(int, int)> render;
    std::function<void(int, int, int)> renderInstanced;

    std::function<void(const GPUFrameBuffer&)> setRenderTarget;
    std::function<void(const GPUFrameBuffer&, std::span<unsigned>)> setRenderTargetPartial;
    std::function<void(const GPUFrameBuffer&, std::array<float, 4>, unsigned)> clearRenderTargetColor;
    std::function<void(const GPUFrameBuffer&, std::array<float, 4>)> clearRenderTargetColors;
    std::function<void(const GPUFrameBuffer&, float)> clearRenderTargetDepth;
    std::function<void(const GPUFrameBuffer&, int)> clearRenderTargetStencil;
    std::function<void(const GPUFrameBuffer&, std::array<float, 4>, float, int)> clearRenderTarget;

    std::function<void(const GPUPipeline&)> bindPipeline;

    std::function<void(const GPUDescriptorSet&)> bindDescriptorSet;

    std::function<std::pair<std::optional<GPUBuffer>,        std::vector<BufferAllocatorMessage>>       (GPUBufferBuilder&)>        buildBuffer;
    std::function<std::pair<std::optional<GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>>(GPUDescriptorSetBuilder&)> buildDescriptorSet;
    std::function<std::pair<std::optional<GPURenderBuffer>,  std::vector<RenderBufferAllocatorMessage>> (GPURenderBufferBuilder&)>  buildRenderBuffer;
    std::function<std::pair<std::optional<GPUFrameBuffer>,   std::vector<FrameBufferAllocatorMessage>>  (GPUFrameBufferBuilder&)>   buildFrameBuffer;
    std::function<std::pair<std::optional<GPUPipeline>,      std::vector<PipelineAllocatorMessage>>     (GPUPipelineBuilder&)>      buildPipeline;
    std::function<std::pair<std::optional<GPUShader>,        std::vector<ShaderCompilerMessage>>        (GPUShaderBuilder&)>        buildShader;
    std::function<std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>>          (GPUShaderProgramBuilder&)> buildShaderProgram;
    std::function<std::pair<std::optional<GPUSampler>,       std::vector<SamplerAllocatorMessage>>      (GPUSamplerBuilder&)>       buildSampler;
    std::function<std::pair<std::optional<GPUTexture>,       std::vector<TextureAllocatorMessage>>      (GPUTextureBuilder&)>       buildTexture;

    std::function<void(GPUBuffer&)>        destroyBuffer;
    std::function<void(GPUDescriptorSet&)> destroyDescriptorSet;
    std::function<void(GPURenderBuffer&)>  destroyRenderBuffer;
    std::function<void(GPUFrameBuffer&)>   destroyFrameBuffer;
    std::function<void(GPUPipeline&)>      destroyPipeline;
    std::function<void(GPUShader&)>        destroyShader;
    std::function<void(GPUShaderProgram&)> destroyShaderProgram;
    std::function<void(GPUSampler&)>       destroySampler;
    std::function<void(GPUTexture&)>       destroyTexture;
}

void Graphics::ChangeGraphicsBackend(GraphicsBackend backend) noexcept {
    currentBackend = backend;
    using enum GraphicsBackend;
    if (backend == None) {
        DOA_LOG_WARNING("Graphics backend set to None. No rendering will be performed.");

        bufferSubData      = static_cast<void(*)(GPUBuffer&, size_t, NonOwningPointerToConstRawData, size_t)>(Graphics::None::BufferSubData);
        getBufferSubData   = Graphics::None::GetBufferSubData;
        copyBufferSubData  = Graphics::None::CopyBufferSubData;
        clearBufferSubData = Graphics::None::ClearBufferSubData;

        blit             = Graphics::None::Blit;
        blitColor        = Graphics::None::BlitColor;
        blitDepth        = Graphics::None::BlitDepth;
        blitStencil      = Graphics::None::BlitStencil;
        blitDepthStencil = Graphics::None::BlitDepthStencil;

        render          = Graphics::None::Render;
        renderInstanced = Graphics::None::RenderInstanced;

        setRenderTarget          = static_cast<void(*)(const GPUFrameBuffer&)>                     (Graphics::OpenGL::SetRenderTarget);
        setRenderTargetPartial   = static_cast<void(*)(const GPUFrameBuffer&, std::span<unsigned>)>(Graphics::OpenGL::SetRenderTarget);
        clearRenderTargetColor   = Graphics::None::ClearRenderTargetColor;
        clearRenderTargetColors  = Graphics::None::ClearRenderTargetColors;
        clearRenderTargetDepth   = Graphics::None::ClearRenderTargetDepth;
        clearRenderTargetStencil = Graphics::None::ClearRenderTargetStencil;
        clearRenderTarget        = Graphics::None::ClearRenderTarget;

        bindPipeline = Graphics::None::BindPipeline;

        bindDescriptorSet = Graphics::None::BindDescriptorSet;

        buildBuffer        = static_cast<std::pair<std::optional<GPUBuffer>,        std::vector<BufferAllocatorMessage>>(*)       (GPUBufferBuilder&)>       (Graphics::None::Build);
        buildDescriptorSet = static_cast<std::pair<std::optional<GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>>(*)(GPUDescriptorSetBuilder&)>(Graphics::None::Build);
        buildRenderBuffer  = static_cast<std::pair<std::optional<GPURenderBuffer>,  std::vector<RenderBufferAllocatorMessage>>(*) (GPURenderBufferBuilder&)> (Graphics::None::Build);
        buildFrameBuffer   = static_cast<std::pair<std::optional<GPUFrameBuffer>,   std::vector<FrameBufferAllocatorMessage>>(*)  (GPUFrameBufferBuilder&)>  (Graphics::None::Build);
        buildPipeline      = static_cast<std::pair<std::optional<GPUPipeline>,      std::vector<PipelineAllocatorMessage>>(*)     (GPUPipelineBuilder&)>     (Graphics::None::Build);
        buildShader        = static_cast<std::pair<std::optional<GPUShader>,        std::vector<ShaderCompilerMessage>>(*)        (GPUShaderBuilder&)>       (Graphics::None::Build);
        buildShaderProgram = static_cast<std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>>(*)          (GPUShaderProgramBuilder&)>(Graphics::None::Build);
        buildSampler       = static_cast<std::pair<std::optional<GPUSampler>,       std::vector<SamplerAllocatorMessage>>(*)      (GPUSamplerBuilder&)>      (Graphics::None::Build);
        buildTexture       = static_cast<std::pair<std::optional<GPUTexture>,       std::vector<TextureAllocatorMessage>>(*)      (GPUTextureBuilder&)>      (Graphics::None::Build);

        destroyBuffer        = static_cast<void(*)(GPUBuffer&)>       (Graphics::None::Destruct);
        destroyDescriptorSet = static_cast<void(*)(GPUDescriptorSet&)>(Graphics::None::Destruct);
        destroyRenderBuffer  = static_cast<void(*)(GPURenderBuffer&)> (Graphics::None::Destruct);
        destroyFrameBuffer   = static_cast<void(*)(GPUFrameBuffer&)>  (Graphics::None::Destruct);
        destroyPipeline      = static_cast<void(*)(GPUPipeline&)>     (Graphics::None::Destruct);
        destroyShader        = static_cast<void(*)(GPUShader&)>       (Graphics::None::Destruct);
        destroyShaderProgram = static_cast<void(*)(GPUShaderProgram&)>(Graphics::None::Destruct);
        destroySampler       = static_cast<void(*)(GPUSampler&)>      (Graphics::None::Destruct);
        destroyTexture       = static_cast<void(*)(GPUTexture&)>      (Graphics::None::Destruct);
    } else if (backend == Software) {
        std::unreachable(); /* not implemented yet */
    }
#ifdef OPENGL_4_6_SUPPORT
    else if (backend == OpenGL4_6) {
        assert(Core::GetCore()->IsOpenGLInitialized());
        DOA_LOG_INFO("OpenGL version: %s", glGetString(GL_VERSION));
        DOA_LOG_INFO("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
        DOA_LOG_INFO("Vendor: %s", glGetString(GL_VENDOR));
        DOA_LOG_INFO("GPU: %s", glGetString(GL_RENDERER));

        bufferSubData      = static_cast<void(*)(GPUBuffer&, size_t, NonOwningPointerToConstRawData, size_t)>(Graphics::OpenGL::BufferSubData);
        getBufferSubData   = Graphics::OpenGL::GetBufferSubData;
        copyBufferSubData  = Graphics::OpenGL::CopyBufferSubData;
        clearBufferSubData = Graphics::OpenGL::ClearBufferSubData;

        blit             = Graphics::OpenGL::Blit;
        blitColor        = Graphics::OpenGL::BlitColor;
        blitDepth        = Graphics::OpenGL::BlitDepth;
        blitStencil      = Graphics::OpenGL::BlitStencil;
        blitDepthStencil = Graphics::OpenGL::BlitDepthStencil;

        render          = Graphics::OpenGL::Render;
        renderInstanced = Graphics::OpenGL::RenderInstanced;

        setRenderTarget          = static_cast<void(*)(const GPUFrameBuffer&)>                     (Graphics::OpenGL::SetRenderTarget);
        setRenderTargetPartial   = static_cast<void(*)(const GPUFrameBuffer&, std::span<unsigned>)>(Graphics::OpenGL::SetRenderTarget);
        clearRenderTargetColor   = Graphics::OpenGL::ClearRenderTargetColor;
        clearRenderTargetColors  = Graphics::OpenGL::ClearRenderTargetColors;
        clearRenderTargetDepth   = Graphics::OpenGL::ClearRenderTargetDepth;
        clearRenderTargetStencil = Graphics::OpenGL::ClearRenderTargetStencil;
        clearRenderTarget        = Graphics::OpenGL::ClearRenderTarget;

        bindPipeline = Graphics::OpenGL::BindPipeline;

        bindDescriptorSet = Graphics::OpenGL::BindDescriptorSet;

        buildBuffer        = static_cast<std::pair<std::optional<GPUBuffer>,        std::vector<BufferAllocatorMessage>>(*)       (GPUBufferBuilder&)>       (Graphics::OpenGL::Build);
        buildDescriptorSet = static_cast<std::pair<std::optional<GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>>(*)(GPUDescriptorSetBuilder&)>(Graphics::OpenGL::Build);
        buildRenderBuffer  = static_cast<std::pair<std::optional<GPURenderBuffer>,  std::vector<RenderBufferAllocatorMessage>>(*) (GPURenderBufferBuilder&)> (Graphics::OpenGL::Build);
        buildFrameBuffer   = static_cast<std::pair<std::optional<GPUFrameBuffer>,   std::vector<FrameBufferAllocatorMessage>>(*)  (GPUFrameBufferBuilder&)>  (Graphics::OpenGL::Build);
        buildPipeline      = static_cast<std::pair<std::optional<GPUPipeline>,      std::vector<PipelineAllocatorMessage>>(*)     (GPUPipelineBuilder&)>     (Graphics::OpenGL::Build);
        buildShader        = static_cast<std::pair<std::optional<GPUShader>,        std::vector<ShaderCompilerMessage>>(*)        (GPUShaderBuilder&)>       (Graphics::OpenGL::Build);
        buildShaderProgram = static_cast<std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>>(*)          (GPUShaderProgramBuilder&)>(Graphics::OpenGL::Build);
        buildSampler       = static_cast<std::pair<std::optional<GPUSampler>,       std::vector<SamplerAllocatorMessage>>(*)      (GPUSamplerBuilder&)>      (Graphics::OpenGL::Build);
        buildTexture       = static_cast<std::pair<std::optional<GPUTexture>,       std::vector<TextureAllocatorMessage>>(*)      (GPUTextureBuilder&)>      (Graphics::OpenGL::Build);

        destroyBuffer        = static_cast<void(*)(GPUBuffer&)>       (Graphics::OpenGL::Destruct);
        destroyDescriptorSet = static_cast<void(*)(GPUDescriptorSet&)>(Graphics::OpenGL::Destruct);
        destroyRenderBuffer  = static_cast<void(*)(GPURenderBuffer&)> (Graphics::OpenGL::Destruct);
        destroyFrameBuffer   = static_cast<void(*)(GPUFrameBuffer&)>  (Graphics::OpenGL::Destruct);
        destroyPipeline      = static_cast<void(*)(GPUPipeline&)>     (Graphics::OpenGL::Destruct);
        destroyShader        = static_cast<void(*)(GPUShader&)>       (Graphics::OpenGL::Destruct);
        destroyShaderProgram = static_cast<void(*)(GPUShaderProgram&)>(Graphics::OpenGL::Destruct);
        destroySampler       = static_cast<void(*)(GPUSampler&)>      (Graphics::OpenGL::Destruct);
        destroyTexture       = static_cast<void(*)(GPUTexture&)>      (Graphics::OpenGL::Destruct);
    }
#endif
#ifdef OPENGL_3_3_SUPPORT
    else if (backend == OpenGL3_3) {
        assert(Core::GetCore()->IsOpenGLInitialized());
        std::unreachable(); /* not implemented yet */
    }
#endif
#ifdef VULKAN_SUPPORT
    else if (backend == Vulkan) {
        assert(Core::GetCore()->IsVulkanInitialized());
        std::unreachable(); /* not implemented yet */
    }
#endif
#ifdef DIRECT3D_12_SUPPORT
    else if (backend == Direct3D12) {
        assert(Core::GetCore()->IsDirect3D12Initialized());
        std::unreachable(); /* not implemented yet */
    }
#endif
#ifdef DIRECT3D_11_SUPPORT
    else if (backend == Direct3D11) {
        assert(Core::GetCore()->IsDirect3D11Initialized());
        std::unreachable(); /* not implemented yet */
    }
#endif
}

void Graphics::BufferSubData(GPUBuffer& buffer, RawDataView dataView, size_t offsetBytes) noexcept {
    assert(bufferSubData && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    bufferSubData(buffer, dataView.size_bytes(), dataView.data(), offsetBytes);
}
void Graphics::BufferSubData(GPUBuffer& buffer, size_t sizeBytes, NonOwningPointerToConstRawData data, size_t offsetBytes) noexcept {
    assert(bufferSubData && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    assert(static_cast<bool>(buffer.Properties & BufferProperties::DynamicStorage));
    bufferSubData(buffer, sizeBytes, data, offsetBytes);
}
void Graphics::GetBufferSubData(const GPUBuffer& buffer, RawDataWriteableView dataView, size_t offsetBytes) noexcept {
    assert(getBufferSubData && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    getBufferSubData(buffer, dataView, offsetBytes);
}
void Graphics::CopyBufferSubData(const GPUBuffer& readBuffer, GPUBuffer& writeBuffer, size_t sizeBytesToCopy, size_t readOffsetBytes, size_t writeOffsetBytes) noexcept {
    assert(copyBufferSubData && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    copyBufferSubData(
        readBuffer,
        writeBuffer,
        readOffsetBytes,
        writeOffsetBytes,
        sizeBytesToCopy
    );
}
void Graphics::ClearBufferSubData(GPUBuffer& buffer, DataFormat format, size_t sizeBytesToClear, size_t offsetBytes) noexcept {
    assert(clearBufferSubData && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    clearBufferSubData(buffer, format, sizeBytesToClear, offsetBytes);
}

void Graphics::Blit(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    assert(blit && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    blit(source, destination);
}
void Graphics::BlitColor(const GPUFrameBuffer& source, GPUFrameBuffer& destination, unsigned srcAttachment, std::span<unsigned> dstAttachments) noexcept {
    assert(blitColor && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    blitColor(source, destination, srcAttachment, dstAttachments);
}
void Graphics::BlitDepth(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    assert(blitDepth && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    blitDepth(source, destination);
}
void Graphics::BlitStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    assert(blitStencil && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    blitStencil(source, destination);
}
void Graphics::BlitDepthStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    assert(blitDepthStencil && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    blitDepthStencil(source, destination);
}

void Graphics::Render(int count, int first) noexcept {
    assert(render && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    render(count, first);
}
void Graphics::RenderInstanced(int instanceCount, int count, int first) noexcept {
    assert(renderInstanced && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    renderInstanced(instanceCount, count, first);
}

void Graphics::SetRenderTarget(const GPUFrameBuffer& renderTarget) noexcept {
    assert(setRenderTarget && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    setRenderTarget(renderTarget);
}
void Graphics::SetRenderTarget(const GPUFrameBuffer& renderTarget, std::span<unsigned> targets) noexcept {
    assert(setRenderTargetPartial && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    setRenderTargetPartial(renderTarget, targets);
}
void Graphics::ClearRenderTargetColor(const GPUFrameBuffer& renderTarget, std::array<float, 4> color, unsigned colorBufferIndex) noexcept {
    assert(clearRenderTargetColor && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    clearRenderTargetColor(renderTarget, color, colorBufferIndex);
}
void Graphics::ClearRenderTargetColors(const GPUFrameBuffer& renderTarget, std::array<float, 4> color) noexcept {
    assert(clearRenderTargetColors && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    clearRenderTargetColors(renderTarget, color);
}
void Graphics::ClearRenderTargetDepth(const GPUFrameBuffer& renderTarget, float depth) noexcept {
    assert(clearRenderTargetDepth && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    clearRenderTargetDepth(renderTarget, depth);
}
void Graphics::ClearRenderTargetStencil(const GPUFrameBuffer& renderTarget, int stencil) noexcept {
    assert(clearRenderTargetStencil && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    clearRenderTargetStencil(renderTarget, stencil);
}
void Graphics::ClearRenderTarget(const GPUFrameBuffer& renderTarget, std::array<float, 4> color, float depth, int stencil) noexcept {
    assert(clearRenderTarget && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    clearRenderTarget(renderTarget, color, depth, stencil);
}

void Graphics::BindPipeline(const GPUPipeline& pipeline) noexcept {
    assert(bindPipeline && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    bindPipeline(pipeline);
}

void Graphics::BindDescriptorSet(const GPUDescriptorSet& descriptorSet) noexcept {
    assert(bindDescriptorSet && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    bindDescriptorSet(descriptorSet);
}

std::pair<std::optional<::GPUBuffer>, std::vector<BufferAllocatorMessage>> Graphics::Builders::Build(GPUBufferBuilder& builder) noexcept {
    assert(buildBuffer && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    return buildBuffer(builder);
}
std::pair<std::optional<::GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>> Graphics::Builders::Build(GPUDescriptorSetBuilder& builder) noexcept {
    assert(buildDescriptorSet && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    return buildDescriptorSet(builder);
}
std::pair<std::optional<::GPURenderBuffer>, std::vector<RenderBufferAllocatorMessage>> Graphics::Builders::Build(GPURenderBufferBuilder& builder) noexcept {
    assert(buildRenderBuffer && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    return buildRenderBuffer(builder);
}
std::pair<std::optional<::GPUFrameBuffer>, std::vector<FrameBufferAllocatorMessage>> Graphics::Builders::Build(GPUFrameBufferBuilder& builder) noexcept {
    assert(buildFrameBuffer && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    return buildFrameBuffer(builder);
}
std::pair<std::optional<::GPUPipeline>, std::vector<PipelineAllocatorMessage>> Graphics::Builders::Build(GPUPipelineBuilder& builder) noexcept {
    assert(buildPipeline && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    return buildPipeline(builder);
}
std::pair<std::optional<::GPUShader>, std::vector<ShaderCompilerMessage>> Graphics::Builders::Build(GPUShaderBuilder& builder) noexcept {
    assert(buildShader && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    return buildShader(builder);
}
std::pair<std::optional<::GPUShaderProgram>, std::vector<ShaderLinkerMessage>> Graphics::Builders::Build(GPUShaderProgramBuilder& builder) noexcept {
    assert(buildShaderProgram && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    return buildShaderProgram(builder);
}
std::pair<std::optional<::GPUSampler>, std::vector<SamplerAllocatorMessage>> Graphics::Builders::Build(GPUSamplerBuilder& builder) noexcept {
    assert(buildSampler && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    return buildSampler(builder);
}
std::pair<std::optional<::GPUTexture>, std::vector<TextureAllocatorMessage>> Graphics::Builders::Build(GPUTextureBuilder& builder) noexcept {
    assert(buildTexture && "Did you forget to call Graphics::ChangeGraphicsBackend()?");
    return buildTexture(builder);
}

void Graphics::Destructors::Destruct(GPUBuffer& buffer) noexcept {
    destroyBuffer(buffer);
}
void Graphics::Destructors::Destruct(GPUDescriptorSet& set) noexcept {
    destroyDescriptorSet(set);
}
void Graphics::Destructors::Destruct(GPURenderBuffer& renderbuffer) noexcept {
    destroyRenderBuffer(renderbuffer);
}
void Graphics::Destructors::Destruct(GPUFrameBuffer& framebuffer) noexcept {
    destroyFrameBuffer(framebuffer);
}
void Graphics::Destructors::Destruct(GPUPipeline& pipeline) noexcept {
    destroyPipeline(pipeline);
}
void Graphics::Destructors::Destruct(GPUShader& shader) noexcept {
    destroyShader(shader);
}
void Graphics::Destructors::Destruct(GPUShaderProgram& program) noexcept {
    destroyShaderProgram(program);
}
void Graphics::Destructors::Destruct(GPUSampler& sampler) noexcept {
    destroySampler(sampler);
}
void Graphics::Destructors::Destruct(GPUTexture& texture) noexcept {
    destroyTexture(texture);
}

std::ostream& operator<<(std::ostream& os, GraphicsBackend backend)       { return os << ToString(backend);  }
std::ostream& operator<<(std::ostream& os, BufferProperties property)     { return os << ToString(property); }
std::ostream& operator<<(std::ostream& os, ShaderType type)               { return os << ToString(type);     }
std::ostream& operator<<(std::ostream& os, TextureMinificationMode mode)  { return os << ToString(mode);     }
std::ostream& operator<<(std::ostream& os, TextureMagnificationMode mode) { return os << ToString(mode);     }
std::ostream& operator<<(std::ostream& os, TextureWrappingMode mode)      { return os << ToString(mode);     }
std::ostream& operator<<(std::ostream& os, TextureCompareMode mode)       { return os << ToString(mode);     }
std::ostream& operator<<(std::ostream& os, TextureCompareFunction func)   { return os << ToString(func);     }
std::ostream& operator<<(std::ostream& os, Multisample sample)            { return os << ToString(sample);   }
std::ostream& operator<<(std::ostream& os, DataFormat format)             { return os << ToString(format);   }
std::ostream& operator<<(std::ostream& os, TopologyType type)             { return os << ToString(type);     }
std::ostream& operator<<(std::ostream& os, DataType type)                 { return os << ToString(type);     }
std::ostream& operator<<(std::ostream& os, PolygonMode mode)              { return os << ToString(mode);     }
std::ostream& operator<<(std::ostream& os, CullMode mode)                 { return os << ToString(mode);     }
std::ostream& operator<<(std::ostream& os, DepthFunction func)            { return os << ToString(func);     }
std::ostream& operator<<(std::ostream& os, BlendFactor factor)            { return os << ToString(factor);   }
std::ostream& operator<<(std::ostream& os, InputRate rate)                { return os << ToString(rate);     }