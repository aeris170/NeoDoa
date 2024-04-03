#include <Engine/AssetBridge.hpp>

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

GPUShaders& AssetGPUBridge::GetShaders() noexcept { return gpuShaders; }
const GPUShaders&  AssetGPUBridge::GetShaders() const noexcept { return gpuShaders; }
GPUShaderPrograms& AssetGPUBridge::GetShaderPrograms() noexcept { return gpuShaderPrograms; }
const GPUShaderPrograms& AssetGPUBridge::GetShaderPrograms() const noexcept { return gpuShaderPrograms; }