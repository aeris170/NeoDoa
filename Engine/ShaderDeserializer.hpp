#pragma once

#include <vector>
#include <string>
#include <string_view>

#include <Engine/Shader.hpp>
#include <Engine/GPUShader.hpp>

struct FNode;

// This struct is redundant, but is here to conform to the deserialization standard of NeoDoa.
struct ShaderDeserializationResult {
    bool erred{ false };
    std::vector<ShaderCompilerMessage> errors{};
    Shader deserializedShader;
};

ShaderDeserializationResult DeserializeVertexShader(const FNode& file);
ShaderDeserializationResult DeserializeVertexShader(const std::string_view data);

ShaderDeserializationResult DeserializeTessellationControlShader(const FNode& file);
ShaderDeserializationResult DeserializeTessellationControlShader(const std::string_view data);

ShaderDeserializationResult DeserializeTessellationEvaluationShader(const FNode& file);
ShaderDeserializationResult DeserializeTessellationEvaluationShader(const std::string_view data);

ShaderDeserializationResult DeserializeGeometryShader(const FNode& file);
ShaderDeserializationResult DeserializeGeometryShader(const std::string_view data);

ShaderDeserializationResult DeserializeFragmentShader(const FNode& file);
ShaderDeserializationResult DeserializeFragmentShader(const std::string_view data);

ShaderDeserializationResult DeserializeComputeShader(const FNode& file);
ShaderDeserializationResult DeserializeComputeShader(const std::string_view data);