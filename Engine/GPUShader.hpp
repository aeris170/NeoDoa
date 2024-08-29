#pragma once

#include <vector>
#include <string>
#include <utility>
#include <optional>
#include <string_view>

#include <Engine/Graphics.hpp>

// Shader
struct GPUShader {
    GLuint GLObjectID{};
    ShaderType Type{};
#ifdef DEBUG
    std::string Name{};
#endif

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPUShader);
};
struct GPUShaderBuilder {
    GPUShaderBuilder& SetType(ShaderType type) noexcept;
    GPUShaderBuilder& SetName(std::string_view name) noexcept;
    GPUShaderBuilder& SetSourceCode(std::string_view sourceCode) noexcept;
    [[nodiscard]] std::pair<std::optional<GPUShader>, std::vector<ShaderCompilerMessage>> Build() noexcept;

private:
    ShaderType type{};
#ifdef DEBUG
    std::string name{};
#endif
    std::string sourceCode{};

public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUShaderBuilder);

private:
#ifdef OPENGL_4_6_SUPPORT
    friend std::pair<std::optional<GPUShader>, std::vector<ShaderCompilerMessage>> Graphics::OpenGL::Build(GPUShaderBuilder&) noexcept;
#endif
};

// Shader Program
struct GPUShaderProgram {
    struct Uniform {
        int Location;
        std::string TypeName;
        std::string Name;
        int ArraySize;
        ShaderType ReferencedBy;
    };

    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{};
#endif

    std::vector<Uniform> Uniforms{};

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPUShaderProgram);

    int GetUniformLocation(std::string_view name) const noexcept;
};
struct GPUShaderProgramBuilder {
    GPUShaderProgramBuilder& SetName(const std::string_view name) noexcept;
    GPUShaderProgramBuilder& SetVertexShader(GPUShader& shader) noexcept;
    GPUShaderProgramBuilder& SetTessellationControlShader(GPUShader& shader) noexcept;
    GPUShaderProgramBuilder& SetTessellationEvaluationShader(GPUShader& shader) noexcept;
    GPUShaderProgramBuilder& SetGeometryShader(GPUShader& shader) noexcept;
    GPUShaderProgramBuilder& SetFragmentShader(GPUShader& shader) noexcept;
    GPUShaderProgramBuilder& SetComputeShader(GPUShader& shader) noexcept;
    [[nodiscard]] std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> Build() noexcept;

private:
#ifdef DEBUG
    std::string name{};
#endif
    GPUShader* vertShader{};
    GPUShader* tessCtrlShader{};
    GPUShader* tessEvalShader{};
    GPUShader* geomShader{};
    GPUShader* fragShader{};
    GPUShader* compShader{};
public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUShaderProgramBuilder);

private:
#ifdef OPENGL_4_6_SUPPORT
    friend std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> Graphics::OpenGL::Build(GPUShaderProgramBuilder&) noexcept;
#endif
};