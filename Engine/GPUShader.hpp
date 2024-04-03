#pragma once

#include <vector>
#include <utility>
#include <optional>

#include <GL/glew.h>

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
    std::string Name{};

    GPUShader() noexcept = default;
    ~GPUShader() noexcept;
    GPUShader(const GPUShader&) = delete;
    GPUShader(GPUShader&& other) noexcept;
    GPUShader& operator=(const GPUShader&) = delete;
    GPUShader& operator=(GPUShader&& other) noexcept;
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
    std::string name{};
    std::string sourceCode{};
public:
    GPUShaderBuilder() = default;
    ~GPUShaderBuilder() = default;
    GPUShaderBuilder(const GPUShaderBuilder&) = delete;
    GPUShaderBuilder& operator=(const GPUShaderBuilder&) = delete;
    GPUShaderBuilder(GPUShaderBuilder&&) = delete;
    GPUShaderBuilder& operator=(GPUShaderBuilder&&) = delete;
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
    std::string Name;

    std::vector<Uniform> Uniforms{};

    GPUShaderProgram() noexcept = default;
    ~GPUShaderProgram() noexcept;
    GPUShaderProgram(const GPUShaderProgram&) = delete;
    GPUShaderProgram(GPUShaderProgram&& other) noexcept;
    GPUShaderProgram& operator=(const GPUShaderProgram&) = delete;
    GPUShaderProgram& operator=(GPUShaderProgram&& other) noexcept;

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

    std::string name{};
    GPUShader* vertShader{};
    GPUShader* tessCtrlShader{};
    GPUShader* tessEvalShader{};
    GPUShader* geomShader{};
    GPUShader* fragShader{};
    GPUShader* compShader{};
public:
    GPUShaderProgramBuilder() = default;
    ~GPUShaderProgramBuilder() = default;
    GPUShaderProgramBuilder(const GPUShaderProgramBuilder&) = delete;
    GPUShaderProgramBuilder& operator=(const GPUShaderProgramBuilder&) = delete;
    GPUShaderProgramBuilder(GPUShaderProgramBuilder&&) = delete;
    GPUShaderProgramBuilder& operator=(GPUShaderProgramBuilder&&) = delete;
};