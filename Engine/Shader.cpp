#include "Shader.hpp"

#include <string_view>

#include <Engine/ShaderDeserializer.hpp>
#include <Engine/ShaderProgramSerializer.hpp>
#include <Engine/ShaderProgramDeserializer.hpp>

std::string Shader::Serialize() const { return SourceCode; }
Shader Shader::Deserialize(const std::string_view data) {
    return {}; // TODO return shader
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