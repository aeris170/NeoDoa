#include "Shader.hpp"

#include <string_view>

#include <Engine/ShaderDeserializer.hpp>

std::string Shader::Serialize() const { return sourceCode; }
Shader Shader::Deserialize(const std::string_view data) {
    return {}; // TODO return shader
}
