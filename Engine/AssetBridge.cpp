#include <Engine/AssetBridge.hpp>

// GPUFrameBuffer
template<>
std::vector<FrameBufferAllocatorMessage> GPUFrameBuffers::Allocate(const Assets& assets, const UUID asset) noexcept {
    AssetHandle handle{ assets.FindAsset(asset) };
    assert(handle && handle->IsFrameBuffer());
    const FrameBuffer& frameBuffer{ handle->DataAs<FrameBuffer>() };

    GPUFrameBufferBuilder builder;
    builder.SetName(frameBuffer.Name);

    GPUTextureBuilder tBuilder;
    GPURenderBufferBuilder rbBuilder;
    size_t limit = std::min(frameBuffer.ColorAttachments.size(), static_cast<size_t>(MaxFrameBufferColorAttachments));
    for (size_t i = 0; i < limit; i++) {
        const FrameBufferAttachment& attachment = frameBuffer.ColorAttachments[i];
        if (attachment.HasReadbackSupport) {
            tBuilder.SetWidth(attachment.Resolution.Width).SetHeight(attachment.Resolution.Height).SetData(attachment.Format, {});
            tBuilder.SetSamples(attachment.Samples);

            auto [gpuTexture, messages] = tBuilder.Build();
            if (gpuTexture.has_value()) {
                builder.AttachColorTexture(std::move(gpuTexture.value()), static_cast<unsigned>(i));
            } else {
                DOA_LOG_ERROR("Texture attachment (for color) allocation failed for %s (UUID: %s). Aborting.", frameBuffer.Name.c_str(), asset.AsString().c_str());
                return messages;
            }
        } else {
            rbBuilder.SetLayout(attachment.Resolution.Width, attachment.Resolution.Height, attachment.Format);
            rbBuilder.SetSamples(attachment.Samples);

            auto [gpuRenderBuffer, messages] = rbBuilder.Build();
            if (gpuRenderBuffer.has_value()) {
                builder.AttachColorRenderBuffer(std::move(gpuRenderBuffer.value()), static_cast<unsigned>(i));
            } else {
                DOA_LOG_ERROR("RenderBuffer attachment (for color) allocation failed for %s (UUID: %s). Aborting.", frameBuffer.Name.c_str(), asset.AsString().c_str());
                return messages;
            }
        }
    }

    if (frameBuffer.DepthAttachment.has_value()) {
        const FrameBufferAttachment& attachment = frameBuffer.DepthAttachment.value();
        if (attachment.HasReadbackSupport) {
            tBuilder.SetWidth(attachment.Resolution.Width).SetHeight(attachment.Resolution.Height).SetData(attachment.Format, {});
            tBuilder.SetSamples(attachment.Samples);

            auto [gpuTexture, messages] = tBuilder.Build();
            if (gpuTexture.has_value()) {
                builder.AttachDepthTexture(std::move(gpuTexture.value()));
            } else {
                DOA_LOG_ERROR("Texture attachment (for depth) allocation failed for %s (UUID: %s). Aborting.", frameBuffer.Name.c_str(), asset.AsString().c_str());
                return messages;
            }
        } else {
            rbBuilder.SetLayout(attachment.Resolution.Width, attachment.Resolution.Height, attachment.Format);
            rbBuilder.SetSamples(attachment.Samples);

            auto [gpuRenderBuffer, messages] = rbBuilder.Build();
            if (gpuRenderBuffer.has_value()) {
                builder.AttachDepthRenderBuffer(std::move(gpuRenderBuffer.value()));
            } else {
                DOA_LOG_ERROR("RenderBuffer attachment (for depth) allocation failed for %s (UUID: %s). Aborting.", frameBuffer.Name.c_str(), asset.AsString().c_str());
                return messages;
            }
        }
    }

    if (frameBuffer.StencilAttachment.has_value()) {
        const FrameBufferAttachment& attachment = frameBuffer.StencilAttachment.value();
        if (attachment.HasReadbackSupport) {
            tBuilder.SetWidth(attachment.Resolution.Width).SetHeight(attachment.Resolution.Height).SetData(attachment.Format, {});
            tBuilder.SetSamples(attachment.Samples);

            auto [gpuTexture, messages] = tBuilder.Build();
            if (gpuTexture.has_value()) {
                builder.AttachStencilTexture(std::move(gpuTexture.value()));
            } else {
                DOA_LOG_ERROR("Texture attachment (for stencil) allocation failed for %s (UUID: %s). Aborting.", frameBuffer.Name.c_str(), asset.AsString().c_str());
                return messages;
            }
        } else {
            rbBuilder.SetLayout(attachment.Resolution.Width, attachment.Resolution.Height, attachment.Format);
            rbBuilder.SetSamples(attachment.Samples);

            auto [gpuRenderBuffer, messages] = rbBuilder.Build();
            if (gpuRenderBuffer.has_value()) {
                builder.AttachStencilRenderBuffer(std::move(gpuRenderBuffer.value()));
            } else {
                DOA_LOG_ERROR("RenderBuffer attachment (for stencil) allocation failed for %s (UUID: %s). Aborting.", frameBuffer.Name.c_str(), asset.AsString().c_str());
                return messages;
            }
        }
    }

    if (frameBuffer.DepthStencilAttachment.has_value()) {
        const FrameBufferAttachment& attachment = frameBuffer.DepthStencilAttachment.value();
        if (attachment.HasReadbackSupport) {
            tBuilder.SetWidth(attachment.Resolution.Width).SetHeight(attachment.Resolution.Height).SetData(attachment.Format, {});
            tBuilder.SetSamples(attachment.Samples);

            auto [gpuTexture, messages] = tBuilder.Build();
            if (gpuTexture.has_value()) {
                builder.AttachDepthStencilTexture(std::move(gpuTexture.value()));
            } else {
                DOA_LOG_ERROR("Texture attachment (for depth-stencil) allocation failed for %s (UUID: %s). Aborting.", frameBuffer.Name.c_str(), asset.AsString().c_str());
                return messages;
            }
        } else {
            rbBuilder.SetLayout(attachment.Resolution.Width, attachment.Resolution.Height, attachment.Format);
            rbBuilder.SetSamples(attachment.Samples);

            auto [gpuRenderBuffer, messages] = rbBuilder.Build();
            if (gpuRenderBuffer.has_value()) {
                builder.AttachDepthStencilRenderBuffer(std::move(gpuRenderBuffer.value()));
            } else {
                DOA_LOG_ERROR("RenderBuffer attachment (for depth-stencil) allocation failed for %s (UUID: %s). Aborting.", frameBuffer.Name.c_str(), asset.AsString().c_str());
                return messages;
            }
        }
    }

    auto [gpuFrameBuffer, messages] = builder.Build();
    if (gpuFrameBuffer.has_value()) {
        database[asset] = std::move(gpuFrameBuffer.value());
    } else {
        DOA_LOG_ERROR("FrameBuffer allocation failed for %s (UUID: %s). Aborting.", frameBuffer.Name.c_str(), asset.AsString().c_str());
    }
    return messages;
}

// GPUShader
template<>
std::vector<ShaderCompilerMessage> GPUShaders::Allocate(const Assets& assets, const UUID asset) noexcept {
    AssetHandle handle{ assets.FindAsset(asset) };
    assert(handle && handle->IsShader());
    const Shader& shader{ handle->DataAs<Shader>() };

    GPUShaderBuilder builder;
    builder.SetName(shader.Name).SetSourceCode(shader.SourceCode).SetType(shader.Type);

    auto [gpuShader, messages] = builder.Build();
    if (gpuShader.has_value()) {
        database[asset] = std::move(gpuShader.value());
    } else {
        DOA_LOG_ERROR("Shader allocation failed for %s (UUID: %s). Aborting.", shader.Name.c_str(), asset.AsString().c_str());
    }
    return messages;
}

// GPUShaderProgram
template<>
std::vector<ShaderLinkerMessage> GPUShaderPrograms::Allocate(const Assets& assets, const UUID asset) noexcept {
    AssetHandle handle{ assets.FindAsset(asset) };
    assert(handle && handle->IsShaderProgram());
    const ShaderProgram& program{ handle->DataAs<ShaderProgram>() };

    std::vector<ShaderLinkerMessage> preBuildMessages;
    GPUShader* vertShader = bridge.GetShaders().Query(program.VertexShader);
    GPUShader* tessEvalShader = bridge.GetShaders().Query(program.TessellationEvaluationShader);
    GPUShader* tessCtrlShader = bridge.GetShaders().Query(program.TessellationControlShader);
    GPUShader* geomShader = bridge.GetShaders().Query(program.GeometryShader);
    GPUShader* fragShader = bridge.GetShaders().Query(program.FragmentShader);
    GPUShader* compShader = bridge.GetShaders().Query(program.ComputeShader);

    if (program.HasComputeShader()) {
        if (!compShader) {
            preBuildMessages.emplace_back("Cannot build pipeline!");
            preBuildMessages.emplace_back("Compute shader couldn't compile. Check Compute shader for compiler errors.");
        }
    } else {
        if (program.HasVertexShader() && !vertShader) {
            preBuildMessages.emplace_back("Vertex shader couldn't compile. Check Vertex shader for compiler errors.");
        }
        if (program.HasTessellationEvaluationShader() && !tessEvalShader) {
            preBuildMessages.emplace_back("Tessellation Evaluation shader couldn't compile. Check Tessellation Evaluation shader for compiler errors.");
        }
        if (program.HasTessellationControlShader() && !tessCtrlShader) {
            preBuildMessages.emplace_back("Tessellation Control shader couldn't compile. Check Tessellation Control shader for compiler errors.");
        }
        if (program.HasGeometryShader() && !geomShader) {
            preBuildMessages.emplace_back("Geometry shader couldn't compile. Check Geometry shader for compiler errors.");
        }
        if (program.HasFragmentShader() && !fragShader) {
            preBuildMessages.emplace_back("Fragment shader couldn't compile. Check Fragment shader for compiler errors.");
        }

        // Prepend message if erred
        if (!preBuildMessages.empty()) {
            preBuildMessages.emplace(preBuildMessages.begin(), "Cannot build pipeline!");
        }
    }

    if (!preBuildMessages.empty()) {
        DOA_LOG_ERROR("Shader program allocation failed for %s (UUID: %s). Aborting.", program.Name.c_str(), asset.AsString().c_str());
        return preBuildMessages;
    }

    GPUShaderProgramBuilder builder;

    builder.SetName(program.Name);
    if (vertShader) {
        builder.SetVertexShader(*vertShader);
    }
    if (tessEvalShader) {
        builder.SetTessellationEvaluationShader(*tessEvalShader);
    }
    if (tessCtrlShader) {
        builder.SetTessellationControlShader(*tessCtrlShader);
    }
    if (geomShader) {
        builder.SetGeometryShader(*geomShader);
    }
    if (fragShader) {
        builder.SetFragmentShader(*fragShader);
    }
    if (compShader) {
        builder.SetComputeShader(*compShader);
    }

    auto [gpuShaderProgram, messages] = builder.Build();
    if (gpuShaderProgram.has_value()) {
        database[asset] = std::move(gpuShaderProgram.value());
    } else {
        DOA_LOG_ERROR("Shader program allocation failed for %s (UUID: %s). Aborting.", program.Name.c_str(), asset.AsString().c_str());
    }
    return messages;
}

// GPUSampler
template<>
std::vector<SamplerAllocatorMessage> GPUSamplers::Allocate(const Assets& assets, const UUID asset) noexcept {
    AssetHandle handle{ assets.FindAsset(asset) };
    assert(handle && handle->IsSampler());
    const Sampler& sampler{ handle->DataAs<Sampler>() };

    GPUSamplerBuilder builder;
    builder.SetName(sampler.Name)
        .SetMinificationFilter(sampler.MinFilter)
        .SetMagnificationFilter(sampler.MagFilter)
        .SetMinLOD(sampler.MinLOD)
        .SetMaxLOD(sampler.MaxLOD)
        .SetLODBias(sampler.LODBias)
        .SetWrapS(sampler.WrapS)
        .SetWrapT(sampler.WrapT)
        .SetWrapR(sampler.WrapR)
        .SetBorderColor(sampler.BorderColor.r, sampler.BorderColor.g, sampler.BorderColor.b, sampler.BorderColor.a)
        .SetCompareMode(sampler.CompareMode)
        .SetCompareFunction(sampler.CompareFunction)
        .SetMaxAnisotropy(sampler.MaxAnisotropy)
        .SetCubemapSeamless(sampler.CubemapSeamless);

    auto [gpuSampler, messages] = builder.Build();
    if (gpuSampler.has_value()) {
        database[asset] = std::move(gpuSampler.value());
    } else {
        DOA_LOG_ERROR("Sampler allocation failed for %s (UUID: %s). Aborting.", sampler.Name.c_str(), asset.AsString().c_str());
    }
    return messages;
}

// GPUTexture
template<>
std::vector<TextureAllocatorMessage> GPUTextures::Allocate(const Assets& assets, const UUID asset) noexcept {
    AssetHandle handle{ assets.FindAsset(asset) };
    assert(handle && handle->IsTexture());
    const Texture& texture{ handle->DataAs<Texture>() };

    GPUTextureBuilder builder;
    builder.SetName(texture.Name)
        .SetWidth(texture.Width)
        .SetHeight(texture.Height)
        .SetDepth(1)
        .SetData(texture.Format, texture.PixelData);

    auto [gpuTexture, messages] = builder.Build();
    if (gpuTexture.has_value()) {
        database[asset] = std::move(gpuTexture.value());
    } else {
        DOA_LOG_ERROR("Texture allocation failed for %s (UUID: %s). Aborting.", texture.Name.c_str(), asset.AsString().c_str());
    }
    return messages;
}
template<>
const GPUTexture& GPUTextures::Missing() const noexcept {
    static const Texture& missingTexture = Texture::Missing();

    static GPUTexture missing = GPUTextureBuilder()
        .SetName(missingTexture.Name)
        .SetWidth(missingTexture.Width)
        .SetHeight(missingTexture.Height)
        .SetData(missingTexture.Format, missingTexture.PixelData)
        .Build().first.value();
    return missing;
}

GPUSamplers& AssetGPUBridge::GetSamplers() noexcept                         { return gpuSamplers;       }
const GPUSamplers& AssetGPUBridge::GetSamplers() const noexcept             { return gpuSamplers;       }
GPUTextures& AssetGPUBridge::GetTextures() noexcept                         { return gpuTextures;       }
const GPUTextures& AssetGPUBridge::GetTextures() const noexcept             { return gpuTextures;       }
GPUShaders& AssetGPUBridge::GetShaders() noexcept                           { return gpuShaders;        }
const GPUShaders& AssetGPUBridge::GetShaders() const noexcept               { return gpuShaders;        }
GPUShaderPrograms& AssetGPUBridge::GetShaderPrograms() noexcept             { return gpuShaderPrograms; }
const GPUShaderPrograms& AssetGPUBridge::GetShaderPrograms() const noexcept { return gpuShaderPrograms; }
GPUFrameBuffers& AssetGPUBridge::GetFrameBuffers() noexcept                 { return gpuFrameBuffers;   }
const GPUFrameBuffers& AssetGPUBridge::GetFrameBuffers() const noexcept     { return gpuFrameBuffers;   }