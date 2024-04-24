#include <Engine/AssetBridge.hpp>

// GPUBuffer
template<>
std::vector<BufferAllocatorMessage> GPUBuffers::Allocate(const Assets& assets, const UUID asset) noexcept {
    return {};
}

// GPUFrameBuffer
template<>
std::vector<FrameBufferAllocatorMessage> GPUFrameBuffers::Allocate(const Assets& assets, const UUID asset) noexcept {
    return {};
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

    GPUShaderProgramBuilder builder;

    builder.SetName(program.Name);
    if (GPUShader* compShader = bridge.GetShaders().Query(program.ComputeShader)) {
        builder.SetComputeShader(*compShader);
    } else {
        GPUShader* vertShader = bridge.GetShaders().Query(program.VertexShader);
        GPUShader* fragShader = bridge.GetShaders().Query(program.FragmentShader);
        if (vertShader && fragShader) {
            builder.SetVertexShader(*vertShader).SetFragmentShader(*fragShader);
        }

        if (GPUShader* tessEvalShader = bridge.GetShaders().Query(program.TessellationEvaluationShader)) {
            builder.SetTessellationEvaluationShader(*tessEvalShader);
            if (GPUShader* tessCtrl = bridge.GetShaders().Query(program.TessellationControlShader)) {
                builder.SetTessellationControlShader(*tessCtrl);
            }
        }

        if (GPUShader* geomShader = bridge.GetShaders().Query(program.GeometryShader)) {
            builder.SetGeometryShader(*geomShader);
        }
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

// GPUVertexArray
template<>
std::vector<VertexArrayAllocatorMessage> GPUVertexArrays::Allocate(const Assets& assets, const UUID asset) noexcept {
    return {};
}

GPUBuffers& AssetGPUBridge::GetGPUBuffers() noexcept                        { return gpuBuffers;        }
const GPUBuffers& AssetGPUBridge::GetGPUBuffers() const noexcept            { return gpuBuffers;        }
GPUFrameBuffers& AssetGPUBridge::GetGPUFrameBuffers() noexcept              { return gpuFrameBuffers;   }
const GPUFrameBuffers& AssetGPUBridge::GetGPUFrameBuffers() const noexcept  { return gpuFrameBuffers;   }
GPUShaders& AssetGPUBridge::GetShaders() noexcept                           { return gpuShaders;        }
const GPUShaders& AssetGPUBridge::GetShaders() const noexcept               { return gpuShaders;        }
GPUShaderPrograms& AssetGPUBridge::GetShaderPrograms() noexcept             { return gpuShaderPrograms; }
const GPUShaderPrograms& AssetGPUBridge::GetShaderPrograms() const noexcept { return gpuShaderPrograms; }
GPUSamplers& AssetGPUBridge::GetSamplers() noexcept                         { return gpuSamplers;       }
const GPUSamplers& AssetGPUBridge::GetSamplers() const noexcept             { return gpuSamplers;       }
GPUTextures& AssetGPUBridge::GetTextures() noexcept                         { return gpuTextures;       }
const GPUTextures& AssetGPUBridge::GetTextures() const noexcept             { return gpuTextures;       }
GPUVertexArrays& AssetGPUBridge::GetGPUVertexArrays() noexcept              { return gpuVertexArrays;   }
const GPUVertexArrays& AssetGPUBridge::GetGPUVertexArrays() const noexcept  { return gpuVertexArrays;   }