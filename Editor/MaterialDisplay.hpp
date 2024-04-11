#pragma once

#include <functional>

#include <Engine/Assets.hpp>
#include <Engine/Shader.hpp>
#include <Engine/Material.hpp>
#include <Engine/GPUShader.hpp>
#include <Engine/GPUTexture.hpp>

struct Observer;

struct MaterialDisplay {

    MaterialDisplay(Observer& observer) noexcept;

    void SetDisplayTarget(Assets& assets, const AssetHandle materialAssetHandle) noexcept;

    void RenderMessagesTable() noexcept;
    void RenderProgramCombo() noexcept;
    void RenderShaderUniforms() noexcept;

private:
    std::reference_wrapper<Observer> observer;
    Assets* assets{ nullptr };
    AssetHandle materialAsset{};
    const Texture* missingTexture{ &Texture::Missing() };

    int CountUniformsInGroup(const GPUShaderProgram& program, ShaderType group) noexcept;
    void RenderUniformGroup(Material::Uniforms& uniforms, const GPUShaderProgram& program, ShaderType group) noexcept;
    bool RenderSingleUniform(Material::Uniforms& uniforms, const UniformValue& value, const GPUShaderProgram::Uniform& uniform) noexcept;

    struct TextureView {
        void Render() noexcept;
        void Show(const Texture& texture, const GPUTexture& gpuTexture) noexcept;
        void Hide() noexcept;
    private:
        bool visible{ false };
        const Texture* texture{ nullptr };
        const GPUTexture* gpuTexture{ nullptr };
    } textureView;
};