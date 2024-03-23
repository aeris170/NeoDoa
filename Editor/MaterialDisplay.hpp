#pragma once

#include <functional>

#include <Engine/Assets.hpp>
#include <Engine/Shader.hpp>
#include <Engine/Material.hpp>

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

    int CountUniformsInGroup(const ShaderProgram& program, Shader::ShaderType group) noexcept;
    void RenderUniformGroup(Material::Uniforms& uniforms, const ShaderProgram& program, Shader::ShaderType group) noexcept;
    bool RenderSingleUniform(Material::Uniforms& uniforms, const UniformValue& value, const ShaderProgram::Uniform& uniform) noexcept;

    struct TextureView {
        void Render() noexcept;
        void Show(const Texture& texture) noexcept;
        void Hide() noexcept;
    private:
        bool visible{ false };
        const Texture* texture{ nullptr };
    } textureView;
};