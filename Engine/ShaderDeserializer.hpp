#pragma once

#include <string>

#include <Engine/Shader.hpp>

struct FNode;

enum class ShaderCompilerMessageType {
    INFO,
    WARNING,
    ERROR,
    ComponentCompilerMessageType_COUNT
};
struct ShaderCompilerMessage {
    int lineNo;
    ShaderCompilerMessageType messageType;
    std::string shortMessage, fullMessage;
};
struct ShaderDeserializationResult {
    bool erred{ false };
    std::vector<ShaderCompilerMessage> messages{};
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