#ifdef OPENGL_4_6_SUPPORT
#include <Engine/GraphicsGL.hpp>

#include <regex>
#include <format>
#include <cassert>
#include <utility>

#include <Utility/Trim.hpp>
#include <Utility/TemplateUtilities.hpp>

#include <Engine/Region.hpp>
#include <Engine/Graphics.hpp>
#include <Engine/GPUBuffer.hpp>
#include <Engine/GPUShader.hpp>
#include <Engine/GPUTexture.hpp>
#include <Engine/Resolution.hpp>
#include <Engine/GPUPipeline.hpp>
#include <Engine/GPUFrameBuffer.hpp>
#include <Engine/GPUDescriptorSet.hpp>

#ifdef DEBUG
#include <debugbreak.h>
static void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);
#endif
static std::vector<std::string> SplitCompilerMessages(const std::string& messages) noexcept;
static ShaderCompilerMessage ParseCompilerMessage(const std::string& message) noexcept;
static void QueryShaderCompilerMessages(GLuint shader, std::vector<ShaderCompilerMessage>& messages) noexcept;
static std::vector<std::string> SplitLinkerMessages(const std::string& messages) noexcept;
static std::string_view SymbolicConstantToShaderUniformType(GLint symbolicConstant) noexcept;
static bool LinkProgram(GLuint program, std::vector<ShaderLinkerMessage>& messages) noexcept;
static std::vector<GPUShaderProgram::Uniform> ExtractActiveProgramUniforms(GLuint program, std::vector<ShaderLinkerMessage>& messages) noexcept;

namespace {

    std::optional<std::reference_wrapper<const GPUPipeline>> currentPipeline;

    Resolution GetAttachmentDimensions(const std::variant<GPUTexture, GPURenderBuffer>& attachment) noexcept {
        return std::visit(overloaded::lambda{
            [](const GPUTexture& t) -> Resolution {
                return { t.Width, t.Height };
            },
            [](const GPURenderBuffer& rb) -> Resolution {
                return { rb.Width, rb.Height };
            }
        }, attachment);
    }

    std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> BuildGraphicsPipeline(std::string& name, std::array<GPUShader*, 5> pipelineStages) noexcept {
        const GPUShader* vertShader = pipelineStages[0];
        const GPUShader* tessCtrlShader = pipelineStages[1];
        const GPUShader* tessEvalShader = pipelineStages[2];
        const GPUShader* geomShader = pipelineStages[3];
        const GPUShader* fragShader = pipelineStages[4];

        std::vector<ShaderLinkerMessage> messages{};
        if (!vertShader) {
            messages.emplace_back("Vertex shader cannot be null while building a graphics pipeline.");
            return { std::nullopt, std::move(messages) };
        }
        if (!fragShader) {
            messages.emplace_back("Fragment shader cannot be null while building a graphics pipeline.");
            return { std::nullopt, std::move(messages) };
        }

        GLuint program = glCreateProgram();

        // Attach pipeline stages
        glAttachShader(program, vertShader->GLObjectID);
        if (tessCtrlShader) {
            glAttachShader(program, tessCtrlShader->GLObjectID);
        }
        if (tessEvalShader) {
            glAttachShader(program, tessEvalShader->GLObjectID);
        }
        if (geomShader) {
            glAttachShader(program, geomShader->GLObjectID);
        }
        glAttachShader(program, fragShader->GLObjectID);

        // Link program
        bool success = LinkProgram(program, messages);

        std::optional<GPUShaderProgram> gpuShaderProgram{ std::nullopt };
        if (success) {
            gpuShaderProgram.emplace();
            gpuShaderProgram->GLObjectID = program;
#ifdef DEBUG
            gpuShaderProgram->Name = std::move(name);
#endif
            gpuShaderProgram->Uniforms = ExtractActiveProgramUniforms(program, messages);
        }

        // Detach pipeline stages
        glDetachShader(program, vertShader->GLObjectID);
        if (tessCtrlShader) {
            glDetachShader(program, tessCtrlShader->GLObjectID);
        }
        if (tessEvalShader) {
            glDetachShader(program, tessEvalShader->GLObjectID);
        }
        if (geomShader) {
            glDetachShader(program, geomShader->GLObjectID);
        }
        glDetachShader(program, fragShader->GLObjectID);

        return { std::move(gpuShaderProgram), std::move(messages) };
    }
    std::pair<std::optional<::GPUShaderProgram>, std::vector<ShaderLinkerMessage>> BuildComputePipeline(std::string& name, GPUShader* computeStage) noexcept {
        std::vector<ShaderLinkerMessage> messages{};
        if (!computeStage) {
            messages.emplace_back("Compute shader cannot be null while building a compute pipeline.");
            return { std::nullopt, std::move(messages) };
        }

        GLuint program = glCreateProgram();

        // Attach compute stage
        glAttachShader(program, computeStage->GLObjectID);

        // Link program
        bool success = LinkProgram(program, messages);

        std::optional<GPUShaderProgram> gpuShaderProgram{ std::nullopt };
        if (success) {
            gpuShaderProgram.emplace();
            gpuShaderProgram->GLObjectID = program;
#ifdef DEBUG
            gpuShaderProgram->Name = std::move(name);
#endif
            gpuShaderProgram->Uniforms = ExtractActiveProgramUniforms(program, messages);
        }

        // Detach compute stage
        glDetachShader(program, computeStage->GLObjectID);

        return { std::move(gpuShaderProgram), std::move(messages) };
    }
}

void Graphics::OpenGL::Initialize() noexcept {
    DOA_LOG_INFO("OpenGL version: %s", glGetString(GL_VERSION));
    DOA_LOG_INFO("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    DOA_LOG_INFO("Vendor: %s", glGetString(GL_VENDOR));
    DOA_LOG_INFO("GPU: %s", glGetString(GL_RENDERER));

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
}

void Graphics::OpenGL::BufferSubData(GPUBuffer& buffer, size_t sizeBytes, NonOwningPointerToConstRawData data, size_t offsetBytes) noexcept {
    assert(static_cast<bool>(buffer.Properties & BufferProperties::DynamicStorage));
    glNamedBufferSubData(buffer.GLObjectID, offsetBytes, sizeBytes, data);
}
void Graphics::OpenGL::GetBufferSubData(const GPUBuffer& buffer, RawDataWriteableView dataView, size_t offsetBytes) noexcept {
    glGetNamedBufferSubData(buffer.GLObjectID, offsetBytes, dataView.size_bytes(), dataView.data());
}
void Graphics::OpenGL::CopyBufferSubData(const GPUBuffer& readBuffer, GPUBuffer& writeBuffer, size_t sizeBytesToCopy, size_t readOffsetBytes, size_t writeOffsetBytes) noexcept {
    assert(readBuffer.GLObjectID > 0);
    assert(readBuffer.GLObjectID > 0);
    glCopyNamedBufferSubData(
        readBuffer.GLObjectID,
        writeBuffer.GLObjectID,
        readOffsetBytes,
        writeOffsetBytes,
        sizeBytesToCopy
    );
}
void Graphics::OpenGL::ClearBufferSubData(GPUBuffer& buffer, DataFormat format, size_t sizeBytesToClear, size_t offsetBytes) noexcept {
    glClearNamedBufferSubData(buffer.GLObjectID, ToGLSizedFormat(format), offsetBytes, sizeBytesToClear, ToGLBaseFormat(format), GL_UNSIGNED_BYTE, nullptr);
}

void Graphics::OpenGL::Blit(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    bool blitColor = !source.ColorAttachments.empty() && !destination.ColorAttachments.empty();
    bool blitDepth =
        (source.DepthAttachment.has_value() || source.DepthStencilAttachment.has_value()) &&
        (destination.DepthAttachment.has_value() || destination.DepthStencilAttachment.has_value());
    bool blitStencil =
        (source.StencilAttachment.has_value() || source.DepthStencilAttachment.has_value()) &&
        (destination.StencilAttachment.has_value() || destination.DepthStencilAttachment.has_value());

    unsigned srcX{}, srcY{}, dstX{}, dstY{};
    if (blitColor) {
        const auto& src { source.ColorAttachments[0].value() };
        std::visit(overloaded::lambda{
        [&srcX, &srcY](const GPUTexture& t) {
            srcX = t.Width;
            srcY = t.Height;
        },
        [&srcX, &srcY](const GPURenderBuffer& rb) {
            srcX = rb.Width;
            srcY = rb.Height;
        }}, src);

        const auto& dst{ destination.ColorAttachments[0].value() };
        std::visit(overloaded::lambda{
        [&dstX, &dstY](const GPUTexture& t) {
            dstX = t.Width;
            dstY = t.Height;
        },
        [&dstX, &dstY](const GPURenderBuffer& rb) {
            dstX = rb.Width;
            dstY = rb.Height;
        }}, dst);
    } else if (blitDepth) {
        const std::variant<GPUTexture, GPURenderBuffer>* src{};
        if (source.DepthAttachment.has_value()) {
            src = &source.DepthAttachment.value();
        } else {
            src = &source.DepthStencilAttachment.value();
        }
        std::visit(overloaded::lambda{
        [&srcX, &srcY](const GPUTexture& t) {
            srcX = t.Width;
            srcY = t.Height;
        },
        [&srcX, &srcY](const GPURenderBuffer& rb) {
            srcX = rb.Width;
            srcY = rb.Height;
        }}, *src);

        const std::variant<GPUTexture, GPURenderBuffer>* dst{};
        if (destination.DepthAttachment.has_value()) {
            dst = &destination.DepthAttachment.value();
        } else {
            dst = &destination.DepthStencilAttachment.value();
        }
        std::visit(overloaded::lambda{
        [&dstX, &dstY](const GPUTexture& t) {
            dstX = t.Width;
            dstY = t.Height;
        },
        [&dstX, &dstY](const GPURenderBuffer& rb) {
            dstX = rb.Width;
            dstY = rb.Height;
        }}, *dst);
    } else if (blitStencil) {
        const std::variant<GPUTexture, GPURenderBuffer>* src{};
        if (source.StencilAttachment.has_value()) {
            src = &source.StencilAttachment.value();
        } else {
            src = &source.DepthStencilAttachment.value();
        }
        std::visit(overloaded::lambda{
        [&srcX, &srcY](const GPUTexture& t) {
            srcX = t.Width;
            srcY = t.Height;
        },
        [&srcX, &srcY](const GPURenderBuffer& rb) {
            srcX = rb.Width;
            srcY = rb.Height;
        }}, *src);

        const std::variant<GPUTexture, GPURenderBuffer>* dst{};
        if (destination.StencilAttachment.has_value()) {
            dst = &destination.StencilAttachment.value();
        } else {
            dst = &destination.DepthStencilAttachment.value();
        }
        std::visit(overloaded::lambda{
        [&dstX, &dstY](const GPUTexture& t) {
            dstX = t.Width;
            dstY = t.Height;
        },
        [&dstX, &dstY](const GPURenderBuffer& rb) {
            dstX = rb.Width;
            dstY = rb.Height;
        }}, *dst);
    } else {
        return;
    }

    auto mask = (blitColor ? GL_COLOR_BUFFER_BIT : GLuint()) |
                (blitDepth ? GL_DEPTH_BUFFER_BIT : GLuint()) |
                (blitStencil ? GL_STENCIL_BUFFER_BIT : GLuint());

    glBlitNamedFramebuffer(
        source.GLObjectID,
        destination.GLObjectID,
        0, 0, srcX, srcY,
        0, 0, dstX, dstY,
        mask,
        GL_NEAREST
    );
}
void Graphics::OpenGL::BlitColor(const GPUFrameBuffer& source, GPUFrameBuffer& destination, unsigned srcAttachment, std::span<unsigned> dstAttachments) noexcept {
    assert(srcAttachment < source.ColorAttachments.size());
    assert(source.ColorAttachments[srcAttachment].has_value());

    assert(std::ranges::all_of(dstAttachments, [&destination](auto dstAttachment) { return dstAttachment < destination.ColorAttachments.size(); }));
    assert(std::ranges::all_of(dstAttachments, [&destination](auto dstAttachment) { return destination.ColorAttachments[dstAttachment].has_value(); }));
    assert(std::ranges::all_of(dstAttachments, [&destination, &dstAttachments](auto dstAttachment) {
        return GetAttachmentDimensions(destination.ColorAttachments[dstAttachments[0]].value()) == GetAttachmentDimensions(destination.ColorAttachments[dstAttachment].value());
    }));

    std::array<GLenum, MaxFrameBufferColorAttachments> drawBuffers{ GL_NONE };
    for (auto dstAttachment : dstAttachments) {
        drawBuffers[dstAttachment] = GL_COLOR_ATTACHMENT0 + dstAttachment;
    }

    // Set read/draw buffers
    glNamedFramebufferReadBuffer(source.GLObjectID, GL_COLOR_ATTACHMENT0 + srcAttachment);
    glNamedFramebufferDrawBuffers(destination.GLObjectID, drawBuffers.size(), drawBuffers.data());

    Resolution srcResolution{ GetAttachmentDimensions(source.ColorAttachments[srcAttachment].value()) };
    Resolution dstResolution{ GetAttachmentDimensions(destination.ColorAttachments[dstAttachments[0]].value()) };

    glBlitNamedFramebuffer(
        source.GLObjectID,
        destination.GLObjectID,
        0, 0, srcResolution.Width, srcResolution.Height,
        0, 0, dstResolution.Width, dstResolution.Height,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );
}
void Graphics::OpenGL::BlitDepth(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    const auto* srcAttachment =      source.DepthAttachment ?      &source.DepthAttachment : nullptr;
    const auto* dstAttachment = destination.DepthAttachment ? &destination.DepthAttachment : nullptr;

    assert(srcAttachment && srcAttachment->has_value());
    assert(dstAttachment && dstAttachment->has_value());

    Resolution srcResolution{ GetAttachmentDimensions(srcAttachment->value()) };
    Resolution dstResolution{ GetAttachmentDimensions(dstAttachment->value()) };

    glBlitNamedFramebuffer(
        source.GLObjectID,
        destination.GLObjectID,
        0, 0, srcResolution.Width, srcResolution.Height,
        0, 0, dstResolution.Width, dstResolution.Height,
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );
}
void Graphics::OpenGL::BlitStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    const auto* srcAttachment =      source.StencilAttachment ?      &source.StencilAttachment : nullptr;
    const auto* dstAttachment = destination.StencilAttachment ? &destination.StencilAttachment : nullptr;

    assert(srcAttachment && srcAttachment->has_value());
    assert(dstAttachment && dstAttachment->has_value());

    Resolution srcResolution{ GetAttachmentDimensions(srcAttachment->value()) };
    Resolution dstResolution{ GetAttachmentDimensions(dstAttachment->value()) };

    glBlitNamedFramebuffer(
        source.GLObjectID,
        destination.GLObjectID,
        0, 0, srcResolution.Width, srcResolution.Height,
        0, 0, dstResolution.Width, dstResolution.Height,
        GL_STENCIL_BUFFER_BIT,
        GL_NEAREST
    );
}
void Graphics::OpenGL::BlitDepthStencil(const GPUFrameBuffer& source, GPUFrameBuffer& destination) noexcept {
    const auto* srcAttachment =      source.DepthStencilAttachment ?      &source.DepthStencilAttachment : nullptr;
    const auto* dstAttachment = destination.DepthStencilAttachment ? &destination.DepthStencilAttachment : nullptr;

    assert(srcAttachment && srcAttachment->has_value());
    assert(dstAttachment && dstAttachment->has_value());

    Resolution srcResolution{ GetAttachmentDimensions(srcAttachment->value()) };
    Resolution dstResolution{ GetAttachmentDimensions(dstAttachment->value()) };

    glBlitNamedFramebuffer(
        source.GLObjectID,
        destination.GLObjectID,
        0, 0, srcResolution.Width, srcResolution.Height,
        0, 0, dstResolution.Width, dstResolution.Height,
        GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GL_NEAREST
    );
}

void Graphics::OpenGL::Render(int count, int first) noexcept {
    const GPUPipeline& pipeline = currentPipeline->get();
    if (pipeline.IndexBuffer) {
        glDrawElements(ToGLTopology(pipeline.Topology), count, ToGLDataType(pipeline.IndexType), nullptr);
    } else {
        glDrawArrays(ToGLTopology(pipeline.Topology), first, count);
    }
}
void Graphics::OpenGL::RenderInstanced(int instanceCount, int count, int first) noexcept {
    const GPUPipeline& pipeline = currentPipeline->get();
    if (pipeline.IndexBuffer) {
        glDrawElementsInstanced(ToGLTopology(pipeline.Topology), count, ToGLDataType(pipeline.IndexType), nullptr, instanceCount);
    } else {
        glDrawArraysInstanced(ToGLTopology(pipeline.Topology), first, count, instanceCount);
    }
}

void Graphics::OpenGL::SetRenderTarget(const GPUFrameBuffer& renderTarget) noexcept {
    if (renderTarget.GLObjectID != 0) {
        std::array<GLenum, MaxFrameBufferColorAttachments> drawBuffers{ GL_NONE };
        for (size_t i = 0; i < renderTarget.ColorAttachments.size(); ++i) {
            if (renderTarget.ColorAttachments[i].has_value()) {
                drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
            }
        }
        glNamedFramebufferDrawBuffers(renderTarget.GLObjectID, drawBuffers.size(), drawBuffers.data());
    }
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.GLObjectID);
}
void Graphics::OpenGL::SetRenderTarget(const GPUFrameBuffer& renderTarget, std::span<unsigned> targets) noexcept {
    std::array<GLenum, MaxFrameBufferColorAttachments> drawBuffers{ GL_NONE };
    if (targets.size() < drawBuffers.size()) {
        std::ranges::transform(targets, drawBuffers.begin(), [](auto target) { return GL_COLOR_ATTACHMENT0 + target; });
    } else {
        std::transform(targets.begin(), targets.begin() + drawBuffers.size(), drawBuffers.begin(), [](auto target) { return GL_COLOR_ATTACHMENT0 + target; });
    }
    glNamedFramebufferDrawBuffers(renderTarget.GLObjectID, drawBuffers.size(), drawBuffers.data());
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.GLObjectID);
}
void Graphics::OpenGL::ClearRenderTargetColor(const GPUFrameBuffer& renderTarget, std::array<float, 4> color, unsigned colorBufferIndex) noexcept {
    assert(renderTarget.ColorAttachments[colorBufferIndex]);
    glClearNamedFramebufferfv(renderTarget.GLObjectID, GL_COLOR, colorBufferIndex, color.data());
}
void Graphics::OpenGL::ClearRenderTargetColors(const GPUFrameBuffer& renderTarget, std::array<float, 4> color) noexcept {
    for (int i = 0; i < renderTarget.ColorAttachments.size(); i++) {
        if (renderTarget.ColorAttachments[i]) {
            glClearNamedFramebufferfv(renderTarget.GLObjectID, GL_COLOR, i, color.data());
        }
    }
}
void Graphics::OpenGL::ClearRenderTargetDepth(const GPUFrameBuffer& renderTarget, float depth) noexcept {
    assert(renderTarget.DepthAttachment || renderTarget.DepthStencilAttachment);
    glClearNamedFramebufferfv(renderTarget.GLObjectID, GL_DEPTH, 0, &depth);
}
void Graphics::OpenGL::ClearRenderTargetStencil(const GPUFrameBuffer& renderTarget, int stencil) noexcept {
    assert(renderTarget.StencilAttachment || renderTarget.DepthStencilAttachment);
    glClearNamedFramebufferiv(renderTarget.GLObjectID, GL_STENCIL, 0, &stencil);
}
void Graphics::OpenGL::ClearRenderTarget(const GPUFrameBuffer& renderTarget, std::array<float, 4> color, float depth, int stencil) noexcept {
    if (!renderTarget.ColorAttachments.empty()) {
        ClearRenderTargetColors(renderTarget, color);
    }
    if (renderTarget.DepthStencilAttachment || (renderTarget.DepthAttachment && renderTarget.StencilAttachment)) {
        glClearNamedFramebufferfi(renderTarget.GLObjectID, GL_DEPTH_STENCIL, 0, depth, stencil);
    } else {
        if (renderTarget.DepthAttachment) {
            ClearRenderTargetDepth(renderTarget, depth);
        }
        if (renderTarget.StencilAttachment) {
            ClearRenderTargetStencil(renderTarget, stencil);
        }
    }
}

void Graphics::OpenGL::BindPipeline(const GPUPipeline& pipeline) noexcept {
    currentPipeline.emplace(pipeline);

    glPolygonMode(GL_FRONT_AND_BACK, ToGLPolygonMode(pipeline.Polygon));
    pipeline.IsFaceCullingEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    glCullFace(ToGLCullMode(pipeline.Cull));
    glViewport(pipeline.Viewport.X, pipeline.Viewport.Y, pipeline.Viewport.Width, pipeline.Viewport.Height);

    pipeline.IsScissorEnabled ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
    glScissor(pipeline.Scissor.X, pipeline.Scissor.Y, pipeline.Scissor.Width, pipeline.Scissor.Height);

    pipeline.IsDepthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glDepthMask(pipeline.IsDepthWriteEnabled ? GL_TRUE : GL_FALSE);
    glDepthFunc(ToGLDepthFunction(pipeline.DepthFunc));
    pipeline.IsDepthClampEnabled ? glEnable(GL_DEPTH_CLAMP) : glDisable(GL_DEPTH_CLAMP);

    pipeline.IsMultisampleEnabled ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);

    pipeline.IsBlendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFuncSeparate(
        ToGLBlendFactor(pipeline.SourceFactor),
        ToGLBlendFactor(pipeline.DestinationFactor),
        ToGLBlendFactor(pipeline.SourceAlphaFactor),
        ToGLBlendFactor(pipeline.DestinationAlphaFactor)
    );

    assert(pipeline.ShaderProgram);
    glUseProgram(pipeline.ShaderProgram->get().GLObjectID);
    glBindVertexArray(pipeline.GLObjectID);
}

void Graphics::OpenGL::BindDescriptorSet(const GPUDescriptorSet& descriptorSet) noexcept {
    for (const DescriptorBinding& binding : descriptorSet.Bindings) {
        if (std::holds_alternative<std::monostate>(binding.Descriptor)) { continue; }

        std::visit(overloaded::lambda {
            [] (const std::monostate&) { /* empty */ },
            [&binding](const DescriptorBinding::UniformBuffer& uniformBuffer) {
                glBindBufferBase(GL_UNIFORM_BUFFER, binding.BindingSlot, uniformBuffer.Buffer.get().GLObjectID);
            },
            [&binding](const DescriptorBinding::StorageBuffer& storageBuffer) {
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding.BindingSlot, storageBuffer.Buffer.get().GLObjectID);
            },
            [&binding](const DescriptorBinding::CombinedImageSampler& combinedImageSampler) {
                glBindTextureUnit(binding.BindingSlot, combinedImageSampler.Texture.get().GLObjectID);
                glBindSampler(binding.BindingSlot, combinedImageSampler.Sampler.get().GLObjectID);
            },
        }, binding.Descriptor);
    }
}

std::pair<std::optional<::GPUBuffer>, std::vector<BufferAllocatorMessage>> Graphics::OpenGL::Build(GPUBufferBuilder& builder) noexcept {
    GLuint buffer;
    glCreateBuffers(1, &buffer);

    glNamedBufferStorage(buffer, builder.size, builder.data, ToGLBufferFlags(builder.properties));

    std::optional<GPUBuffer> gpuBuffer{ std::nullopt };
    gpuBuffer.emplace();
    gpuBuffer->GLObjectID = buffer;
#ifdef DEBUG
    gpuBuffer->Name = std::move(builder.name);
#endif
    gpuBuffer->Properties = builder.properties;
    gpuBuffer->SizeBytes = builder.size;

    return { std::move(gpuBuffer), {} };
}
std::pair<std::optional<::GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>> Graphics::OpenGL::Build(GPUDescriptorSetBuilder& builder) noexcept {
    std::optional<GPUDescriptorSet> gpuDescriptorSet{ std::nullopt };
    gpuDescriptorSet.emplace();

    gpuDescriptorSet->Bindings = std::move(builder.bindings); // this doesn't perform a move - yet (std::array.operator=(&&) copies)

    return { gpuDescriptorSet, {} };
}
std::pair<std::optional<::GPURenderBuffer>, std::vector<RenderBufferAllocatorMessage>> Graphics::OpenGL::Build(GPURenderBufferBuilder& builder) noexcept {
    GLuint renderBuffer;
    glCreateRenderbuffers(1, &renderBuffer);
    if (builder.samples == Multisample::None) {
        glNamedRenderbufferStorage(
            renderBuffer,
            ToGLSizedFormat(builder.format),
            builder.width, builder.height
        );
    } else {
        glNamedRenderbufferStorageMultisample(
            renderBuffer,
            static_cast<GLsizei>(builder.samples),
            ToGLSizedFormat(builder.format),
            builder.width, builder.height
        );
    }

    std::optional<GPURenderBuffer> gpuRenderBuffer{ std::nullopt };
    gpuRenderBuffer.emplace();
    gpuRenderBuffer->GLObjectID = renderBuffer;
#ifdef DEBUG
    gpuRenderBuffer->Name = std::move(builder.name);
#endif
    gpuRenderBuffer->Width = builder.width;
    gpuRenderBuffer->Height = builder.height;
    gpuRenderBuffer->Format = builder.format;
    gpuRenderBuffer->Samples = builder.samples;

    return { std::move(gpuRenderBuffer), {} };
}
std::pair<std::optional<::GPUFrameBuffer>, std::vector<FrameBufferAllocatorMessage>> Graphics::OpenGL::Build(GPUFrameBufferBuilder& builder) noexcept {
    GLuint frameBuffer;
    glCreateFramebuffers(1, &frameBuffer);

    std::array<GLenum, MaxFrameBufferColorAttachments> drawBuffers{ GL_NONE };
    for (int i = 0; i < builder.colorAttachments.size(); i++) {
        const auto& colorAttachment = builder.colorAttachments[i];

        if (builder.colorAttachments[i].has_value()) {
            drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
            std::visit(overloaded::lambda {
                [frameBuffer, &i](const GPUTexture& texture) {
                    glNamedFramebufferTexture(frameBuffer, GL_COLOR_ATTACHMENT0 + i, texture.GLObjectID, 0);
                },
                [frameBuffer, &i](const GPURenderBuffer& renderBuffer) {
                    glNamedFramebufferRenderbuffer(frameBuffer, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, renderBuffer.GLObjectID);
                }
            }, colorAttachment.value());
        }
    }
    glNamedFramebufferDrawBuffers(frameBuffer, drawBuffers.size(), drawBuffers.data());

    if (builder.depthStencilAttachment) {
        std::visit(overloaded::lambda{
            [frameBuffer](const GPUTexture& texture) {
                glNamedFramebufferTexture(frameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, texture.GLObjectID, 0);
            },
            [frameBuffer](const GPURenderBuffer& renderBuffer) {
                glNamedFramebufferRenderbuffer(frameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GLObjectID);
            }
        }, builder.depthStencilAttachment.value());
    } else {
        if (builder.depthAttachment) {
            std::visit(overloaded::lambda{
                [frameBuffer](const GPUTexture& texture) {
                    glNamedFramebufferTexture(frameBuffer, GL_DEPTH_ATTACHMENT, texture.GLObjectID, 0);
                },
                [frameBuffer](const GPURenderBuffer& renderBuffer) {
                    glNamedFramebufferRenderbuffer(frameBuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GLObjectID);
                }
            }, builder.depthAttachment.value());
        }
        if (builder.stencilAttachment) {
            std::visit(overloaded::lambda{
                [frameBuffer](const GPUTexture& texture) {
                    glNamedFramebufferTexture(frameBuffer, GL_STENCIL_ATTACHMENT, texture.GLObjectID, 0);
                },
                [frameBuffer](const GPURenderBuffer& renderBuffer) {
                    glNamedFramebufferRenderbuffer(frameBuffer, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GLObjectID);
                }
            }, builder.stencilAttachment.value());
        }
    }

    std::vector<FrameBufferAllocatorMessage> messages{};
    GLenum status = glCheckNamedFramebufferStatus(frameBuffer, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        messages.emplace_back("Framebuffer is not complete.");
        switch (status) {
        case GL_FRAMEBUFFER_UNDEFINED:
            messages.emplace_back("Framebuffer is undefined.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            messages.emplace_back("Framebuffer has incomplete attachment.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            messages.emplace_back("Framebuffer is missing attachment.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            messages.emplace_back("Framebuffer has incomplete draw buffer.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            messages.emplace_back("Framebuffer has incomplete read buffer.");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            messages.emplace_back("Framebuffer is unsupported.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            messages.emplace_back("Framebuffer has incomplete multisample.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            messages.emplace_back("Framebuffer has incomplete layer targets.");
            break;
        default:
            std::unreachable();
        }
    }

    std::optional<GPUFrameBuffer> gpuFrameBuffer{ std::nullopt };
    gpuFrameBuffer.emplace();
    gpuFrameBuffer->GLObjectID = frameBuffer;
#ifdef DEBUG
    gpuFrameBuffer->Name = std::move(builder.name);
#endif
    gpuFrameBuffer->ColorAttachments = std::move(builder.colorAttachments);
    gpuFrameBuffer->DepthAttachment = std::move(builder.depthAttachment);
    gpuFrameBuffer->StencilAttachment = std::move(builder.stencilAttachment);
    gpuFrameBuffer->DepthStencilAttachment = std::move(builder.depthStencilAttachment);

    return { std::move(gpuFrameBuffer), std::move(messages) };
}
std::pair<std::optional<::GPUPipeline>, std::vector<PipelineAllocatorMessage>> Graphics::OpenGL::Build(GPUPipelineBuilder& builder) noexcept {
    assert(builder.vertexBuffers.size() == builder.vertexLayouts.size()); // impossible

    if (!builder.shaderProgam) {
        return { std::nullopt, { "Cannot create pipeline without a shader program!" } };
    }

    GLuint vertexArray;
    glCreateVertexArrays(1, &vertexArray);

    unsigned attribIndex{};
    for (unsigned bindingIndx = 0; bindingIndx < builder.vertexBuffers.size(); bindingIndx++) {
        if (!builder.vertexBuffers[bindingIndx].has_value()) { continue; }

        const GPUBuffer& vertexBuffer{ builder.vertexBuffers[bindingIndx].value().get() };
        const GPUVertexAttribLayout& layout{ builder.vertexLayouts[bindingIndx] };
        glVertexArrayVertexBuffer(vertexArray, bindingIndx, vertexBuffer.GLObjectID, 0, layout.Stride);
        glVertexArrayBindingDivisor(vertexArray, bindingIndx, ToGLInputRate(layout.InputRate));
        for (unsigned i = 0; i < layout.Elements.size(); i++) {
            const GPUVertexAttribLayout::Element& elem{ layout.Elements[i] };
            if (elem.Count == 0) { continue; }

            glEnableVertexArrayAttrib(vertexArray, attribIndex);
            if (elem.Type == GL_INT ||
                elem.Type == GL_BYTE ||
                elem.Type == GL_SHORT ||
                elem.Type == GL_UNSIGNED_INT ||
                elem.Type == GL_UNSIGNED_BYTE ||
                elem.Type == GL_UNSIGNED_SHORT) {
                glVertexArrayAttribIFormat(vertexArray, attribIndex, elem.Count, elem.Type, layout.Offsets[i]);
            } else if (elem.Type == GL_DOUBLE) {
                glVertexArrayAttribLFormat(vertexArray, attribIndex, elem.Count, elem.Type, layout.Offsets[i]);
            } else {
                glVertexArrayAttribFormat(vertexArray, attribIndex, elem.Count, elem.Type, elem.IsNormalized, layout.Offsets[i]);
            }
            glVertexArrayAttribBinding(vertexArray, attribIndex, bindingIndx);
            attribIndex++;
        }
    }

    if (builder.indexBuffer) {
        glVertexArrayElementBuffer(vertexArray, builder.indexBuffer->GLObjectID);
    }

    std::optional<GPUPipeline> gpuPipeline{ std::nullopt };
    gpuPipeline.emplace();
    gpuPipeline->GLObjectID = vertexArray;
#ifdef DEBUG
    gpuPipeline->Name = std::move(builder.name);
#endif
    gpuPipeline->VertexBuffers = std::move(builder.vertexBuffers);
    gpuPipeline->VertexLayouts = std::move(builder.vertexLayouts);
    gpuPipeline->IndexBuffer = builder.indexBuffer;
    gpuPipeline->IndexType = builder.indexType;
    gpuPipeline->Topology = builder.topology;
    gpuPipeline->IsFaceCullingEnabled = builder.isFaceCullingEnabled;
    gpuPipeline->Cull = builder.cullMode;
    gpuPipeline->Polygon = builder.polygonMode;
    gpuPipeline->Viewport = builder.viewport;
    gpuPipeline->IsScissorEnabled = builder.isScissorEnabled;
    gpuPipeline->Scissor = builder.scissor;
    gpuPipeline->IsDepthTestEnabled = builder.isDepthTestEnabled;
    gpuPipeline->IsDepthWriteEnabled = builder.isDepthWriteEnabled;
    gpuPipeline->DepthFunc = builder.depthFunction;
    gpuPipeline->IsDepthClampEnabled = builder.isDepthClampEnabled;
    gpuPipeline->IsMultisampleEnabled = builder.isMultisampleEnabled;
    gpuPipeline->IsBlendEnabled = builder.isBlendEnabled;
    gpuPipeline->SourceFactor = builder.srcRGBFactor;
    gpuPipeline->DestinationFactor = builder.dstRGBFactor;
    gpuPipeline->SourceAlphaFactor = builder.srcAlphaFactor;
    gpuPipeline->DestinationAlphaFactor = builder.dstAlphaFactor;
    gpuPipeline->ShaderProgram = builder.shaderProgam;

    return { std::move(gpuPipeline), {} };
}
std::pair<std::optional<::GPUShader>, std::vector<ShaderCompilerMessage>> Graphics::OpenGL::Build(GPUShaderBuilder& builder) noexcept {
    std::vector<ShaderCompilerMessage> messages{};
    messages.emplace_back(0, ShaderCompilerMessage::Type::Info, std::format("Make sure this file contains GLSL {} Shader code!", ToString(builder.type)));

    GLuint shader{};
    switch (builder.type) {
        using enum ShaderType;
    case Vertex:                 shader = glCreateShader(GL_VERTEX_SHADER);          break;
    case TessellationControl:    shader = glCreateShader(GL_TESS_CONTROL_SHADER);    break;
    case TessellationEvaluation: shader = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
    case Geometry:               shader = glCreateShader(GL_GEOMETRY_SHADER);        break;
    case Fragment:               shader = glCreateShader(GL_FRAGMENT_SHADER);        break;
    case Compute:                shader = glCreateShader(GL_COMPUTE_SHADER);         break;
    }
    if (shader == decltype(shader){}) {
        // switch-case failed / user entered out of bounds type!
        messages.emplace_back(0, ShaderCompilerMessage::Type::Error, std::format("{} Shader compilation failed!", ToString(builder.type)));
        messages.emplace_back(0, ShaderCompilerMessage::Type::Error, "Couldn't make out shader type!");
        return { std::nullopt, messages };
    }

    const auto source = builder.sourceCode.data();
    glShaderSource(shader, 1, &source, nullptr);

    int success;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    QueryShaderCompilerMessages(shader, messages);

    if (!success) {
        messages.emplace_back(0, ShaderCompilerMessage::Type::Error, std::format("{} Shader compilation failed!", ToString(builder.type)));
        glDeleteShader(shader);
    }

    std::optional<GPUShader> gpuShader{ std::nullopt };
    if (success) {
        gpuShader.emplace();
        gpuShader->GLObjectID = shader;
        gpuShader->Type = builder.type;
#ifdef DEBUG
        gpuShader->Name = std::move(builder.name);
#endif
    }

    return { std::move(gpuShader), std::move(messages) };
}
std::pair<std::optional<::GPUShaderProgram>, std::vector<ShaderLinkerMessage>> Graphics::OpenGL::Build(GPUShaderProgramBuilder& builder) noexcept {
    if (builder.compShader) {
#ifdef DEBUG
        return BuildComputePipeline(builder.name, builder.compShader);
#elif NDEBUG
        std::string name = "";
        return BuildComputePipeline(name, builder.compShader);
#endif
    } else if (builder.vertShader && builder.fragShader) {
        std::array<GPUShader*, 5> stages {
            builder.vertShader,
            builder.tessCtrlShader,
            builder.tessEvalShader,
            builder.geomShader,
            builder.fragShader
        };
#ifdef DEBUG
        return BuildGraphicsPipeline(builder.name, stages);
#elif NDEBUG
        std::string name = "";
        return BuildGraphicsPipeline(name, stages);
#endif
    } else {
        return { std::nullopt, { "No pipeline can be built with shaders set.", "Make sure you set either both Vertex and Fragment shader or the Compute Shader alone." } };
    }
}
std::pair<std::optional<::GPUSampler>, std::vector<SamplerAllocatorMessage>> Graphics::OpenGL::Build(GPUSamplerBuilder& builder) noexcept {
    GLuint sampler;
    glGenSamplers(1, &sampler);

    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, ToGLTextureMinificationMode(builder.minFilter));
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, ToGLTextureMagnificationMode(builder.magFilter));
    glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, builder.minLOD);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, builder.maxLOD);
    glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, builder.LODBias);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, ToGLTextureWrappingMode(builder.wrapS));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, ToGLTextureWrappingMode(builder.wrapT));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, ToGLTextureWrappingMode(builder.wrapR));
    glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, builder.borderColor.data());
    glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, ToGLTextureCompareMode(builder.compareMode));
    glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, ToGLTextureCompareFunction(builder.compareFunction));
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, builder.maxAnisotropy);
    glSamplerParameteri(sampler, GL_TEXTURE_CUBE_MAP_SEAMLESS, builder.cubemapSeamless);

    std::optional<GPUSampler> gpuSampler{ std::nullopt };
    gpuSampler.emplace();
    gpuSampler->GLObjectID = sampler;
#ifdef DEBUG
    gpuSampler->Name = std::move(builder.name);
#endif

    return { std::move(gpuSampler), {} };
}
std::pair<std::optional<::GPUTexture>, std::vector<TextureAllocatorMessage>> Graphics::OpenGL::Build(GPUTextureBuilder& builder) noexcept {
    GLuint texture;
    if (builder.depth > 1) {
        assert(builder.samples == Multisample::None); // Multisampled 3D textures are not allowed.
        glCreateTextures(GL_TEXTURE_3D, 1, &texture);
        glTextureStorage3D(
            texture,
            static_cast<GLsizei>(1 + std::floor(std::log2(std::max({ builder.width, builder.height, builder.depth })))),
            ToGLSizedFormat(builder.format),
            builder.width, builder.height, builder.depth
        );
    } else if (builder.width > 1) {
        if (builder.samples == Multisample::None) {
            glCreateTextures(GL_TEXTURE_2D, 1, &texture);
            glTextureStorage2D(
                texture,
                static_cast<GLsizei>(1 + std::floor(std::log2(std::max(builder.width, builder.height)))),
                ToGLSizedFormat(builder.format),
                builder.width, builder.height
            );
        } else {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &texture);
            glTextureStorage2DMultisample(
                texture,
                static_cast<GLsizei>(builder.samples),
                ToGLSizedFormat(builder.format),
                builder.width, builder.height,
                GL_TRUE
            );
        }
    } else {
        assert(builder.samples == Multisample::None); // Multisampled 1D textures are not allowed.
        glCreateTextures(GL_TEXTURE_1D, 1, &texture);
        glTextureStorage1D(
            texture,
            static_cast<GLsizei>(1 + std::floor(std::log2(builder.width))),
            ToGLSizedFormat(builder.format),
            builder.width
        );
    }

    if (!builder.pixels.empty()) {
        if (builder.depth > 1) {
            glTextureSubImage3D(
                texture,
                0,
                0, 0, 0,
                builder.width, builder.height, builder.depth,
                ToGLBaseFormat(builder.format),
                GL_UNSIGNED_BYTE,
                builder.pixels.data()
            );
        } else if (builder.width > 1) {
            glTextureSubImage2D(
                texture,
                0,
                0, 0,
                builder.width, builder.height,
                ToGLBaseFormat(builder.format),
                GL_UNSIGNED_BYTE,
                builder.pixels.data()
            );
        } else {
            glTextureSubImage1D(
                texture,
                0,
                0,
                builder.width,
                ToGLBaseFormat(builder.format),
                GL_UNSIGNED_BYTE,
                builder.pixels.data()
            );
        }
        if (builder.samples == Multisample::None) {
            glGenerateTextureMipmap(texture); // Mipmaps are not allowed on multisampled textures.
        }
    }

    std::optional<GPUTexture> gpuTexture{ std::nullopt };
    gpuTexture.emplace();
    gpuTexture->GLObjectID = texture;
#ifdef DEBUG
    gpuTexture->Name = std::move(builder.name);
#endif
    gpuTexture->Width = builder.width;
    gpuTexture->Height = builder.height;
    gpuTexture->Depth = builder.depth;
    gpuTexture->Format = builder.format;

    return { std::move(gpuTexture), {} };
}

void Graphics::OpenGL::Destruct(GPUBuffer& buffer) noexcept {
    glDeleteBuffers(1, &buffer.GLObjectID);
}
void Graphics::OpenGL::Destruct(GPUDescriptorSet& set) noexcept {}
void Graphics::OpenGL::Destruct(GPURenderBuffer& renderbuffer) noexcept {
    glDeleteRenderbuffers(1, &renderbuffer.GLObjectID);
}
void Graphics::OpenGL::Destruct(GPUFrameBuffer& framebuffer) noexcept {
    glDeleteFramebuffers(1, &framebuffer.GLObjectID);
}
void Graphics::OpenGL::Destruct(GPUPipeline& pipeline) noexcept {
    glDeleteVertexArrays(1, &pipeline.GLObjectID);
}
void Graphics::OpenGL::Destruct(GPUShader& shader) noexcept {
    glDeleteShader(shader.GLObjectID);
}
void Graphics::OpenGL::Destruct(GPUShaderProgram& program) noexcept {
    glDeleteProgram(program.GLObjectID);
}
void Graphics::OpenGL::Destruct(GPUSampler& sampler) noexcept {
    glDeleteSamplers(1, &sampler.GLObjectID);
}
void Graphics::OpenGL::Destruct(GPUTexture& texture) noexcept {
    glDeleteTextures(1, &texture.GLObjectID);
}

#ifdef DEBUG
static void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param) {
    auto const src_str = [source]() {
        switch (source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        default: return "DEFAULT";
        }
    }();

    auto const type_str = [type]() {
        switch (type) {
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        case GL_DEBUG_TYPE_OTHER: return "OTHER";
        default: return "DEFAULT";
        }
    }();

    auto const severity_str = [severity]() {
        switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_LOW: return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        default: return "DEFAULT";
        }
    }();

    DOA_LOG_OPENGL("%s, %s, %s, %d: %s", src_str, type_str, severity_str, id, message);
    debug_break();
}
#endif
static std::vector<std::string> SplitCompilerMessages(const std::string& messages) noexcept {
    static const std::regex regex{ "\n(?!\\s)" };

    std::vector<std::string> rv;
    std::copy(
        std::sregex_token_iterator{ messages.cbegin(), messages.cend(), regex, -1 },
        std::sregex_token_iterator{},
        std::back_inserter(rv)
    );

    return rv;
}
static ShaderCompilerMessage ParseCompilerMessage(const std::string& message) noexcept {
    static const std::regex regex{ "0\\((\\d*)\\)\\s*:\\s*(\\w*)\\s*\\w*\\s*:\\s*(.*)" };

    ShaderCompilerMessage rv;

    std::smatch match;
    if (std::regex_search(message, match, regex)) {
        rv.FullMessage = trim_copy(message);
        rv.LineNo = std::stoi(match[1]);
        std::string type = match[2];
        if (type == "info") {
            rv.MessageType = ShaderCompilerMessage::Type::Info;
        } else if (type == "warning") {
            rv.MessageType = ShaderCompilerMessage::Type::Warning;
        } else if (type == "error") {
            rv.MessageType = ShaderCompilerMessage::Type::Error;
        } else {
            assert(false); // matches to something else, find and implement!
        }
        rv.ShortMessage = match[3];
    } else {
        assert(false); // investigate why regex didn't match!
    }
    return rv;
}
static void QueryShaderCompilerMessages(GLuint shader, std::vector<ShaderCompilerMessage>& messages) noexcept {
    GLint bufferLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetShaderInfoLog(shader, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitCompilerMessages(logString);
        for (const auto& log : logs) {
            messages.emplace_back(ParseCompilerMessage(log));
        }
        delete[] logChars;
    }
}
static std::vector<std::string> SplitLinkerMessages(const std::string& messages) noexcept {
    static const std::regex regex{ "\n(?!\\s)" };

    std::vector<std::string> rv;
    std::copy(
        std::sregex_token_iterator{ messages.cbegin(), messages.cend(), regex, -1 },
        std::sregex_token_iterator{},
        std::back_inserter(rv)
    );

    return rv;
}
static std::string_view SymbolicConstantToShaderUniformType(GLint symbolicConstant) noexcept {
    if (symbolicConstant == GL_FLOAT)           { return "float"; }
    else if (symbolicConstant == GL_FLOAT_VEC2) { return "vec2";  }
    else if (symbolicConstant == GL_FLOAT_VEC3) { return "vec3";  }
    else if (symbolicConstant == GL_FLOAT_VEC4) { return "vec4";  }

    else if (symbolicConstant == GL_DOUBLE)      { return "double"; }
    else if (symbolicConstant == GL_DOUBLE_VEC2) { return "dvec2";  }
    else if (symbolicConstant == GL_DOUBLE_VEC3) { return "dvec3";  }
    else if (symbolicConstant == GL_DOUBLE_VEC4) { return "dvec4";  }

    else if (symbolicConstant == GL_INT)      { return "int";   }
    else if (symbolicConstant == GL_INT_VEC2) { return "ivec2"; }
    else if (symbolicConstant == GL_INT_VEC3) { return "ivec3"; }
    else if (symbolicConstant == GL_INT_VEC4) { return "ivec4"; }

    else if (symbolicConstant == GL_UNSIGNED_INT)      { return "unsigned int"; }
    else if (symbolicConstant == GL_UNSIGNED_INT_VEC2) { return "uvec2";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_VEC3) { return "uvec3";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_VEC4) { return "uvec4";        }

    else if (symbolicConstant == GL_BOOL)      { return "bool";  }
    else if (symbolicConstant == GL_BOOL_VEC2) { return "bvec2"; }
    else if (symbolicConstant == GL_BOOL_VEC3) { return "bvec3"; }
    else if (symbolicConstant == GL_BOOL_VEC4) { return "bvec4"; }

    else if (symbolicConstant == GL_FLOAT_MAT2)   { return "mat2";   }
    else if (symbolicConstant == GL_FLOAT_MAT3)   { return "mat3";   }
    else if (symbolicConstant == GL_FLOAT_MAT4)   { return "mat4";   }
    else if (symbolicConstant == GL_FLOAT_MAT2x3) { return "mat2x3"; }
    else if (symbolicConstant == GL_FLOAT_MAT2x4) { return "mat2x4"; }
    else if (symbolicConstant == GL_FLOAT_MAT3x2) { return "mat3x2"; }
    else if (symbolicConstant == GL_FLOAT_MAT3x4) { return "mat3x4"; }
    else if (symbolicConstant == GL_FLOAT_MAT4x2) { return "mat4x2"; }
    else if (symbolicConstant == GL_FLOAT_MAT4x3) { return "mat4x3"; }

    else if (symbolicConstant == GL_DOUBLE_MAT2)   { return "dmat2";   }
    else if (symbolicConstant == GL_DOUBLE_MAT3)   { return "dmat3";   }
    else if (symbolicConstant == GL_DOUBLE_MAT4)   { return "dmat4";   }
    else if (symbolicConstant == GL_DOUBLE_MAT2x3) { return "dmat2x3"; }
    else if (symbolicConstant == GL_DOUBLE_MAT2x4) { return "dmat2x4"; }
    else if (symbolicConstant == GL_DOUBLE_MAT3x2) { return "dmat3x2"; }
    else if (symbolicConstant == GL_DOUBLE_MAT3x4) { return "dmat3x4"; }
    else if (symbolicConstant == GL_DOUBLE_MAT4x2) { return "dmat4x2"; }
    else if (symbolicConstant == GL_DOUBLE_MAT4x3) { return "dmat4x3"; }

    else if (symbolicConstant == GL_SAMPLER_1D)                   { return "sampler1D";            }
    else if (symbolicConstant == GL_SAMPLER_2D)                   { return "sampler2D";            }
    else if (symbolicConstant == GL_SAMPLER_3D)                   { return "sampler3D";            }
    else if (symbolicConstant == GL_SAMPLER_CUBE)                 { return "samplerCube";          }
    else if (symbolicConstant == GL_SAMPLER_1D_SHADOW)            { return "sampler1DShadow";      }
    else if (symbolicConstant == GL_SAMPLER_2D_SHADOW)            { return "sampler2DShadow";      }
    else if (symbolicConstant == GL_SAMPLER_1D_ARRAY)             { return "sampler1DArray";       }
    else if (symbolicConstant == GL_SAMPLER_2D_ARRAY)             { return "sampler2DArray";       }
    else if (symbolicConstant == GL_SAMPLER_1D_ARRAY_SHADOW)      { return "sampler1DArrayShadow"; }
    else if (symbolicConstant == GL_SAMPLER_2D_ARRAY_SHADOW)      { return "sampler2DArrayShadow"; }
    else if (symbolicConstant == GL_SAMPLER_2D_MULTISAMPLE)       { return "sampler2DMS";          }
    else if (symbolicConstant == GL_SAMPLER_2D_MULTISAMPLE_ARRAY) { return "sampler2DMSArray";     }
    else if (symbolicConstant == GL_SAMPLER_CUBE_SHADOW)          { return "samplerCubeShadow";    }
    else if (symbolicConstant == GL_SAMPLER_BUFFER)               { return "samplerBuffer";        }
    else if (symbolicConstant == GL_SAMPLER_2D_RECT)              { return "sampler2DRect";        }
    else if (symbolicConstant == GL_SAMPLER_2D_RECT_SHADOW)       { return "sampler2DRectShadow";  }

    else if (symbolicConstant == GL_INT_SAMPLER_1D)                   { return "isampler1D";        }
    else if (symbolicConstant == GL_INT_SAMPLER_2D)                   { return "isampler2D";        }
    else if (symbolicConstant == GL_INT_SAMPLER_3D)                   { return "isampler3D";        }
    else if (symbolicConstant == GL_INT_SAMPLER_CUBE)                 { return "isamplerCube";      }
    else if (symbolicConstant == GL_INT_SAMPLER_1D_ARRAY)             { return "isampler1DArray";   }
    else if (symbolicConstant == GL_INT_SAMPLER_2D_ARRAY)             { return "isampler2DArray";   }
    else if (symbolicConstant == GL_INT_SAMPLER_2D_MULTISAMPLE)       { return "isampler2DMS";      }
    else if (symbolicConstant == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) { return "isampler2DMSArray"; }
    else if (symbolicConstant == GL_INT_SAMPLER_BUFFER)               { return "isamplerBuffer";    }
    else if (symbolicConstant == GL_INT_SAMPLER_2D_RECT)              { return "isampler2DRect";    }

    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_1D)                   { return "usampler1D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D)                   { return "usampler2D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_3D)                   { return "usampler3D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_CUBE)                 { return "usamplerCube";      }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY)             { return "usampler2DArray";   }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY)             { return "usampler2DArray";   }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE)       { return "usampler2DMS";      }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) { return "usampler2DMSArray"; }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_BUFFER)               { return "usamplerBuffer";    }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D_RECT)              { return "usampler2DRect";    }

    else if (symbolicConstant == GL_IMAGE_1D)                   { return "image1D";        }
    else if (symbolicConstant == GL_IMAGE_2D)                   { return "image2D";        }
    else if (symbolicConstant == GL_IMAGE_3D)                   { return "image3D";        }
    else if (symbolicConstant == GL_IMAGE_2D_RECT)              { return "image2DRect";    }
    else if (symbolicConstant == GL_IMAGE_CUBE)                 { return "imageCube";      }
    else if (symbolicConstant == GL_IMAGE_BUFFER)               { return "imageBuffer";    }
    else if (symbolicConstant == GL_IMAGE_1D_ARRAY)             { return "image1DArray";   }
    else if (symbolicConstant == GL_IMAGE_2D_ARRAY)             { return "image2DArray";   }
    else if (symbolicConstant == GL_IMAGE_2D_MULTISAMPLE)       { return "image2DMS";      }
    else if (symbolicConstant == GL_IMAGE_2D_MULTISAMPLE_ARRAY) { return "image2DMSArray"; }

    else if (symbolicConstant == GL_INT_IMAGE_1D)                   { return "iimage1D";        }
    else if (symbolicConstant == GL_INT_IMAGE_2D)                   { return "iimage2D";        }
    else if (symbolicConstant == GL_INT_IMAGE_3D)                   { return "iimage3D";        }
    else if (symbolicConstant == GL_INT_IMAGE_2D_RECT)              { return "iimage2DRect";    }
    else if (symbolicConstant == GL_INT_IMAGE_CUBE)                 { return "iimageCube";      }
    else if (symbolicConstant == GL_INT_IMAGE_BUFFER)               { return "iimageBuffer";    }
    else if (symbolicConstant == GL_INT_IMAGE_1D_ARRAY)             { return "iimage1DArray";   }
    else if (symbolicConstant == GL_INT_IMAGE_2D_ARRAY)             { return "iimage2DArray";   }
    else if (symbolicConstant == GL_INT_IMAGE_2D_MULTISAMPLE)       { return "iimage2DMS";      }
    else if (symbolicConstant == GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY) { return "iimage2DMSArray"; }

    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_1D)                   { return "uimage1D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D)                   { return "uimage2D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_3D)                   { return "uimage3D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D_RECT)              { return "uimage2DRect";    }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_CUBE)                 { return "uimageCube";      }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_BUFFER)               { return "uimageBuffer";    }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_1D_ARRAY)             { return "uimage1DArray";   }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D_ARRAY)             { return "uimage2DArray";   }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE)       { return "uimage2DMS";      }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY) { return "uimage2DMSArray"; }

    else if (symbolicConstant == GL_UNSIGNED_INT_ATOMIC_COUNTER) { return "atomic_uint"; }

    else {
        DOA_LOG_WARNING("Detected unknown type of uniform. Symbolic constant: %d", symbolicConstant);
        std::unreachable();
    }
}
static bool LinkProgram(GLuint program, std::vector<ShaderLinkerMessage>& messages) noexcept {
    GLint success;

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        messages.emplace_back("Program linking failed!");

        GLint bufferLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufferLength);
        if (bufferLength > 1) {
            GLchar* logChars = new char[bufferLength + 1];
            glGetProgramInfoLog(program, bufferLength, NULL, logChars);
            std::string logString{ logChars };
            trim(logString);
            auto logs = SplitLinkerMessages(logString);
            for (auto& log : logs) {
                messages.emplace_back(std::move(log));
            }
            delete[] logChars;
        }

        glDeleteProgram(program);
    }

    return static_cast<bool>(success);
}
static std::vector<GPUShaderProgram::Uniform> ExtractActiveProgramUniforms(GLuint program, std::vector<ShaderLinkerMessage>& messages) noexcept {
    std::vector<GPUShaderProgram::Uniform> rv;

    // Extract active uniforms
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
    std::vector<GLchar> nameData(256);
    std::vector<GLenum> properties;
    properties.push_back(GL_NAME_LENGTH);                          // 0
    properties.push_back(GL_TYPE);                                 // 1
    properties.push_back(GL_ARRAY_SIZE);                           // 2
    properties.push_back(GL_REFERENCED_BY_VERTEX_SHADER);          // 3
    properties.push_back(GL_REFERENCED_BY_TESS_CONTROL_SHADER);    // 4
    properties.push_back(GL_REFERENCED_BY_TESS_EVALUATION_SHADER); // 5
    properties.push_back(GL_REFERENCED_BY_GEOMETRY_SHADER);        // 6
    properties.push_back(GL_REFERENCED_BY_FRAGMENT_SHADER);        // 7
    properties.push_back(GL_REFERENCED_BY_COMPUTE_SHADER);         // 8
    std::vector<GLint> values(properties.size());

    for (int i = 0; i < numActiveUniforms; i++) {
        GPUShaderProgram::Uniform uniform;
        glGetProgramResourceiv(program, GL_UNIFORM, i, properties.size(), properties.data(), values.size(), NULL, values.data());

        // Extract uniform name
        nameData.resize(values[0]); //The length of the name.
        glGetProgramResourceName(program, GL_UNIFORM, i, nameData.size(), NULL, nameData.data());
        uniform.Name = std::string(nameData.data(), nameData.size() - 1);

        // Extract uniform location
        uniform.Location = glGetUniformLocation(program, uniform.Name.c_str());

        // Extract uniform type name
        uniform.TypeName = SymbolicConstantToShaderUniformType(values[1]);

        // Extract uniform array size (eg. extracts 100 from "mat4 bones[100]")
        uniform.ArraySize = values[2];

        // Extract uniform source (which shader it comes from)
        if (values[3]) {
            uniform.ReferencedBy = ShaderType::Vertex;
        } else if (values[4]) {
            uniform.ReferencedBy = ShaderType::TessellationControl;
        } else if (values[5]) {
            uniform.ReferencedBy = ShaderType::TessellationEvaluation;
        } else if (values[6]) {
            uniform.ReferencedBy = ShaderType::Geometry;
        } else if (values[7]) {
            uniform.ReferencedBy = ShaderType::Fragment;
        } else if (values[8]) {
            uniform.ReferencedBy = ShaderType::Compute;
        } else {
            messages.emplace_back(std::format("Uniform {} not referenced by any shader.", uniform.Name));
            continue; // error! abort this uniform and continue with the next one.
        }

        int idx = 0;
        while (idx < uniform.ArraySize) {
            rv.emplace_back(uniform);

            uniform.Location++;
            uniform.Name = std::format("{}[{}]", uniform.Name.substr(0, uniform.Name.size() - 3), idx + 1);
            idx++;
        }
    }

    return rv;
}
#endif