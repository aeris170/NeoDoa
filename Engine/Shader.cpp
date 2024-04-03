#include "Shader.hpp"

#include <string_view>

#include <Engine/Log.hpp>
#include <Engine/ShaderDeserializer.hpp>
#include <Engine/ShaderProgramSerializer.hpp>
#include <Engine/ShaderProgramDeserializer.hpp>

std::string Shader::Serialize() const noexcept { return SourceCode; }
Shader Shader::Deserialize(const std::string_view data) noexcept {
    auto result = DeserializeVertexShader(data);
    if (!result.erred) { return result.deserializedShader; }

    result = DeserializeFragmentShader(data);
    if (!result.erred) { return result.deserializedShader; }

    result = DeserializeTessellationControlShader(data);
    if (!result.erred) { return result.deserializedShader; }

    result = DeserializeTessellationEvaluationShader(data);
    if (!result.erred) { return result.deserializedShader; }

    result = DeserializeGeometryShader(data);
    if (!result.erred) { return result.deserializedShader; }

    result = DeserializeComputeShader(data);
    if (!result.erred) { return result.deserializedShader; }

    return {}; // TODO return shader
}

Shader Shader::Copy(const Shader& shader) noexcept {
    return shader.Deserialize(shader.Serialize());
}

bool ShaderProgram::IsComplete() const  noexcept {
    if (!HasComputeShader()) {
        return HasVertexShader() && HasFragmentShader() ;
    } else {
        return !HasVertexShader() &&
            !HasTessellationControlShader() &&
            !HasTessellationEvaluationShader() &&
            !HasGeometryShader() &&
            !HasFragmentShader() &&
            HasComputeShader();
    }
}
bool ShaderProgram::HasVertexShader() const noexcept                 { return VertexShader                 != UUID::Empty(); }
bool ShaderProgram::HasTessellationControlShader() const noexcept    { return TessellationControlShader    != UUID::Empty(); }
bool ShaderProgram::HasTessellationEvaluationShader() const noexcept { return TessellationEvaluationShader != UUID::Empty(); }
bool ShaderProgram::HasGeometryShader() const noexcept               { return GeometryShader               != UUID::Empty(); }
bool ShaderProgram::HasFragmentShader() const noexcept               { return FragmentShader               != UUID::Empty(); }
bool ShaderProgram::HasComputeShader() const noexcept                { return ComputeShader                != UUID::Empty(); }

std::string ShaderProgram::Serialize() const noexcept { return SerializeShaderProgram(*this); }
ShaderProgram ShaderProgram::Deserialize(std::string_view data) noexcept { return DeserializeShaderProgram(data).deserializedShaderProgram; }

ShaderProgram ShaderProgram::Copy(const ShaderProgram& program) noexcept { return program.Deserialize(program.Serialize()); }