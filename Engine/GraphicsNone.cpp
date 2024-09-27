#include <Engine/GraphicsNone.hpp>

#include <Engine/Graphics.hpp>
#include <Engine/GPUBuffer.hpp>
#include <Engine/GPUShader.hpp>
#include <Engine/GPUTexture.hpp>
#include <Engine/Resolution.hpp>
#include <Engine/GPUPipeline.hpp>
#include <Engine/GPUFrameBuffer.hpp>
#include <Engine/GPUDescriptorSet.hpp>

#ifdef _MSC_VER  // Check if compiling with Visual C++
#pragma warning(disable : 4100) // disable warning C4100: 'parameter': unreferenced formal parameter
#elif defined(__GNUC__) || defined(__clang__)  // Check if compiling with GCC or Clang
#pragma GCC diagnostic ignored "-Wunused-parameter" // ignore -Wunused-parameter warning
#endif

void Graphics::None::BufferSubData(GPUBuffer& buffer, size_t sizeBytes, NonOwningPointerToConstRawData data, size_t offsetBytes) noexcept {}
void Graphics::None::GetBufferSubData(const GPUBuffer& buffer, RawDataWriteableView dataView, size_t offsetBytes) noexcept {}
void Graphics::None::CopyBufferSubData(const GPUBuffer& readBuffer, GPUBuffer& writeBuffer, size_t sizeBytesToCopy, size_t readOffsetBytes, size_t writeOffsetBytes) noexcept {}
void Graphics::None::ClearBufferSubData(GPUBuffer& buffer, DataFormat format, size_t sizeBytesToClear, size_t offsetBytes) noexcept {}

void Graphics::None::Blit(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {}
void Graphics::None::BlitColor(const GPUFrameBuffer& source, GPUFrameBuffer& destination, unsigned srcAttachment, std::span<unsigned> dstAttachments) noexcept {}
void Graphics::None::BlitDepth(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {}
void Graphics::None::BlitStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {}
void Graphics::None::BlitDepthStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {}

void Graphics::None::Render(int count, int first) noexcept {}
void Graphics::None::RenderInstanced(int instanceCount, int count, int first) noexcept {}

void Graphics::None::SetRenderTarget(const GPUFrameBuffer& renderTarget) noexcept {}
void Graphics::None::SetRenderTarget(const GPUFrameBuffer& renderTarget, std::span<unsigned> targets) noexcept {}
void Graphics::None::ClearRenderTargetColor(const GPUFrameBuffer& renderTarget, std::array<float, 4> color, unsigned colorBufferIndex) noexcept {}
void Graphics::None::ClearRenderTargetColors(const GPUFrameBuffer& renderTarget, std::array<float, 4> color) noexcept {}
void Graphics::None::ClearRenderTargetDepth(const GPUFrameBuffer& renderTarget, float depth) noexcept {}
void Graphics::None::ClearRenderTargetStencil(const GPUFrameBuffer& renderTarget, int stencil) noexcept {}
void Graphics::None::ClearRenderTarget(const GPUFrameBuffer& renderTarget, std::array<float, 4> color, float depth, int stencil) noexcept {}

void Graphics::None::BindPipeline(const GPUPipeline& pipeline) noexcept {}

void Graphics::None::BindDescriptorSet(const GPUDescriptorSet& descriptorSet) noexcept {}

std::pair<std::optional<GPUBuffer>, std::vector<BufferAllocatorMessage>> Graphics::None::Build(GPUBufferBuilder& builder) noexcept {
    return { {{}}, { "You're using no-op graphics backend.", "This object will not function as desired." } };
}
std::pair<std::optional<GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>> Graphics::None::Build(GPUDescriptorSetBuilder& builder) noexcept {
    return { {{}}, { "You're using no-op graphics backend.", "This object will not function as desired." } };
}
std::pair<std::optional<GPURenderBuffer>, std::vector<RenderBufferAllocatorMessage>> Graphics::None::Build(GPURenderBufferBuilder& builder) noexcept {
    return { {{}}, { "You're using no-op graphics backend.", "This object will not function as desired." } };
}
std::pair<std::optional<GPUFrameBuffer>, std::vector<FrameBufferAllocatorMessage>> Graphics::None::Build(GPUFrameBufferBuilder& builder) noexcept {
    return { {{}}, { "You're using no-op graphics backend.", "This object will not function as desired." } };
}
std::pair<std::optional<GPUPipeline>, std::vector<PipelineAllocatorMessage>> Graphics::None::Build(GPUPipelineBuilder& builder) noexcept {
    return { {{}}, { "You're using no-op graphics backend.", "This object will not function as desired." } };
}
std::pair<std::optional<GPUShader>, std::vector<ShaderCompilerMessage>> Graphics::None::Build(GPUShaderBuilder& builder) noexcept {
    return { {{}}, {{ 0, ShaderCompilerMessage::Type::Info, "You're using no-op graphics backend.", }, { 0, ShaderCompilerMessage::Type::Info, "This object will not function as desired.", }}};
}
std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> Graphics::None::Build(GPUShaderProgramBuilder& builder) noexcept {
    return { {{}}, { "You're using no-op graphics backend.", "This object will not function as desired." } };
}
std::pair<std::optional<GPUSampler>, std::vector<SamplerAllocatorMessage>> Graphics::None::Build(GPUSamplerBuilder& builder) noexcept {
    return { {{}}, { "You're using no-op graphics backend.", "This object will not function as desired." } };
}
std::pair<std::optional<GPUTexture>, std::vector<TextureAllocatorMessage>> Graphics::None::Build(GPUTextureBuilder& builder) noexcept {
    return { {{}}, { "You're using no-op graphics backend.", "This object will not function as desired." } };
}

void Graphics::None::Destruct(GPUBuffer& buffer) noexcept {}
void Graphics::None::Destruct(GPUDescriptorSet& set) noexcept {}
void Graphics::None::Destruct(GPURenderBuffer& renderbuffer) noexcept {}
void Graphics::None::Destruct(GPUFrameBuffer& framebuffer) noexcept {}
void Graphics::None::Destruct(GPUPipeline& pipeline) noexcept {}
void Graphics::None::Destruct(GPUShader& shader) noexcept {}
void Graphics::None::Destruct(GPUShaderProgram& program) noexcept {}
void Graphics::None::Destruct(GPUSampler& sampler) noexcept {}
void Graphics::None::Destruct(GPUTexture& texture) noexcept {}

#ifdef _MSC_VER
#pragma warning(default : 4100) // re-enable warning C4100
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic warning "-Wunused-parameter" // restore -Wunused-parameter warning
#endif