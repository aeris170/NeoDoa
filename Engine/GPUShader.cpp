#include <Engine/GPUShader.hpp>

#include <iomanip>

#include <Engine/Log.hpp>

// Shader
GPUShader::~GPUShader() noexcept {
    Graphics::Destructors::Destruct(*this);
}
GPUShader::GPUShader(GPUShader&& other) noexcept {
    *this = std::move(other);
}
GPUShader& GPUShader::operator=(GPUShader&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
    Type = std::exchange(other.Type, {});
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    return *this;
}

GPUShaderBuilder& GPUShaderBuilder::SetType(ShaderType shaderType) noexcept {
    type = shaderType;
    return *this;
}
GPUShaderBuilder& GPUShaderBuilder::SetName(std::string_view shaderName) noexcept {
#ifdef DEBUG
    name = shaderName;
#endif
    return *this;
}
GPUShaderBuilder& GPUShaderBuilder::SetSourceCode(std::string_view code) noexcept {
    sourceCode = code;
    return *this;
}
std::pair<std::optional<GPUShader>, std::vector<ShaderCompilerMessage>> GPUShaderBuilder::Build() noexcept {
    return Graphics::Builders::Build(*this);
}

// Shader Program
GPUShaderProgram::~GPUShaderProgram() noexcept {
    Graphics::Destructors::Destruct(*this);
}
GPUShaderProgram::GPUShaderProgram(GPUShaderProgram&& other) noexcept {
    *this = std::move(other);
}
GPUShaderProgram& GPUShaderProgram::operator=(GPUShaderProgram&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    Uniforms = std::move(other.Uniforms);
    return *this;
}

int GPUShaderProgram::GetUniformLocation(std::string_view name) const noexcept {
    auto search = std::ranges::find_if(Uniforms, [name](const Uniform& uniform) { return uniform.Name == name; });
#ifdef DEBUG
    if (search == Uniforms.end()) {
        DOA_LOG_WARNING("Program %s does not contain uniform %s", std::quoted(Name), std::quoted(name));
        return -1;
    }
#endif
    return search->Location;
}

GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetName(const std::string_view programName) noexcept {
#ifdef DEBUG
    name = programName;
#endif
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetVertexShader(GPUShader& shader) noexcept {
    vertShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetTessellationControlShader(GPUShader& shader) noexcept {
    tessCtrlShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetTessellationEvaluationShader(GPUShader& shader) noexcept {
    tessEvalShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetGeometryShader(GPUShader& shader) noexcept {
    geomShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetFragmentShader(GPUShader& shader) noexcept {
    fragShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetComputeShader(GPUShader& shader) noexcept {
    compShader = &shader;
    return *this;
}

std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> GPUShaderProgramBuilder::Build() noexcept {
    return Graphics::Builders::Build(*this);
}