#pragma once

#include <string>
#include <functional>
#include <string_view>

#include <imgui.h>

#include <Engine/Assets.hpp>
#include <Engine/Shader.hpp>

struct Observer;

struct ShaderProgramDisplay {

    ShaderProgramDisplay(Observer& observer) noexcept;

    void SetDisplayTarget(Assets& assets, const AssetHandle shaderProgramAssetHandle) noexcept;

    void RenderMessagesTable() noexcept;
    void RenderShaders() noexcept;

private:
    static constexpr ImVec4 NOT_SET_MANDATORY_COLOR{ 1, 0, 0, 1 };
    static constexpr ImVec4 NOT_SET_OPTIONAL_COLOR{ 1, 1, 0, 1 };
    static constexpr ImVec4 SET_COLOR{ 0, 1, 0, 1 };
    static constexpr std::string_view NOT_SET_MANDATORY_TEXT{ "NOT SET - MANDATORY!" };
    static constexpr std::string_view NOT_SET_OPTIONAL_TEXT{ "NOT SET - OPTIONAL!" };

    std::reference_wrapper<Observer> observer;
    Assets* assets{ nullptr };
    AssetHandle shaderProgramAsset{};
    std::string vertexShaderText;
    std::string tessCtrlShaderText;
    std::string tessEvalShaderText;
    std::string geometryShaderText;
    std::string fragmentShaderText;

    void Begin(std::string_view label) noexcept;
    void RenderVertexShader(ShaderProgram& program) noexcept;
    void RenderTessCtrlShader(ShaderProgram& program) noexcept;
    void RenderTessEvalShader(ShaderProgram& program) noexcept;
    void RenderGeometryShader(ShaderProgram& program) noexcept;
    void RenderFragmentShader(ShaderProgram& program) noexcept;
    void End() noexcept;
};