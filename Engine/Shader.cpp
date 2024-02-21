#include "Shader.hpp"

#include <string_view>

#include <Engine/ShaderDeserializer.hpp>
#include <Engine/ShaderProgramSerializer.hpp>
#include <Engine/ShaderProgramDeserializer.hpp>

std::string Shader::Serialize() const { return SourceCode; }
Shader Shader::Deserialize(const std::string_view data) {
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

Shader Shader::Copy(const Shader& shader) {
    return shader.Deserialize(shader.Serialize());
}

bool ShaderProgram::IsComplete() const                      { return HasVertexShader() && HasFragmentShader();      }
bool ShaderProgram::HasVertexShader() const                 { return VertexShader                 != UUID::Empty(); }
bool ShaderProgram::HasTessellationControlShader() const    { return TessellationControlShader    != UUID::Empty(); }
bool ShaderProgram::HasTessellationEvaluationShader() const { return TessellationEvaluationShader != UUID::Empty(); }
bool ShaderProgram::HasGeometryShader() const               { return GeometryShader               != UUID::Empty(); }
bool ShaderProgram::HasFragmentShader() const               { return FragmentShader               != UUID::Empty(); }

std::string ShaderProgram::Serialize() const { return SerializeShaderProgram(*this); }
ShaderProgram ShaderProgram::Deserialize(std::string_view data) { return DeserializeShaderProgram(data).deserializedShaderProgram; }

ShaderProgram ShaderProgram::Copy(const ShaderProgram& program) { return program.Deserialize(program.Serialize()); }