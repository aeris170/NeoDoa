#pragma once

#include <vector>
#include <utility>
#include <optional>

#include <Engine/Graphics.hpp>

// Shader
enum class ShaderCompilerMessageType {
    Info,
    Warning,
    Error
};
struct ShaderCompilerMessage {
    int LineNo;
    ShaderCompilerMessageType MessageType;
    std::string ShortMessage, FullMessage;
};

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
    static std::vector<std::string> SplitCompilerMessages(const std::string& messages) noexcept;
    static ShaderCompilerMessage ParseCompilerMessage(const std::string& message) noexcept;
    static void QueryShaderCompilerMessages(GLuint shader, std::vector<ShaderCompilerMessage>& messages) noexcept;

    ShaderType type{};
#ifdef DEBUG
    std::string name{};
#endif
    std::string sourceCode{};
public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUShaderBuilder);
};

// Shader Program
using ShaderLinkerMessage = std::string;

struct GPUShaderProgram {
    struct Uniform {
        int Location;
        std::string TypeName;
        std::string Name;
        int ArraySize;
        ShaderType ReferencedBy;
    };

    GLuint GLObjectID;
#ifdef DEBUG
    std::string Name;
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
    std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> BuildGraphicsPipeline() noexcept;
    std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> BuildComputePipeline() noexcept;

    static std::vector<std::string> SplitLinkerMessages(const std::string& messages) noexcept;
    static std::string_view SymbolicConstantToShaderUniformType(GLint symbolicConstant) noexcept;
    static bool LinkProgram(GLuint program, std::vector<ShaderLinkerMessage>& messages) noexcept;
    static std::vector<GPUShaderProgram::Uniform> ExtractActiveProgramUniforms(GLuint program, std::vector<ShaderLinkerMessage>& messages) noexcept;

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
};