#include <Engine/ShaderDeserializer.hpp>

#include <Engine/FileNode.hpp>

ShaderDeserializationResult DeserializeVertexShader(const FNode& file) {
    file.ReadContent();
    ShaderDeserializationResult rv = DeserializeVertexShader(file.DisposeContent());
    rv.deserializedShader.Name = file.Name();
    return rv;
}
ShaderDeserializationResult DeserializeVertexShader(const std::string_view data) {
    return { false, {}, { ShaderType::Vertex, "", data.data() }};
}

ShaderDeserializationResult DeserializeTessellationControlShader(const FNode& file) {
    file.ReadContent();
    ShaderDeserializationResult rv = DeserializeTessellationControlShader(file.DisposeContent());
    rv.deserializedShader.Name = file.Name();
    return rv;
}
ShaderDeserializationResult DeserializeTessellationControlShader(const std::string_view data) {
    return { false, {}, { ShaderType::TessellationControl, "", data.data() } };
}

ShaderDeserializationResult DeserializeTessellationEvaluationShader(const FNode& file) {
    file.ReadContent();
    ShaderDeserializationResult rv = DeserializeTessellationEvaluationShader(file.DisposeContent());
    rv.deserializedShader.Name = file.Name();
    return rv;
}
ShaderDeserializationResult DeserializeTessellationEvaluationShader(const std::string_view data) {
    return { false, {}, { ShaderType::TessellationEvaluation, "", data.data() } };
}

ShaderDeserializationResult DeserializeGeometryShader(const FNode& file) {
    file.ReadContent();
    ShaderDeserializationResult rv = DeserializeGeometryShader(file.DisposeContent());
    rv.deserializedShader.Name = file.Name();
    return rv;
}
ShaderDeserializationResult DeserializeGeometryShader(const std::string_view data) {
    return { false, {}, { ShaderType::Geometry, "", data.data() } };
}

ShaderDeserializationResult DeserializeFragmentShader(const FNode& file) {
    file.ReadContent();
    ShaderDeserializationResult rv = DeserializeFragmentShader(file.DisposeContent());
    rv.deserializedShader.Name = file.Name();
    return rv;
}
ShaderDeserializationResult DeserializeFragmentShader(const std::string_view data) {
    return { false, {}, { ShaderType::Fragment, "", data.data() } };
}

ShaderDeserializationResult DeserializeComputeShader(const FNode& file) {
    file.ReadContent();
    ShaderDeserializationResult rv = DeserializeComputeShader(file.DisposeContent());
    rv.deserializedShader.Name = file.Name();
    return rv;
}
ShaderDeserializationResult DeserializeComputeShader(const std::string_view data) {
    return { false, {}, { ShaderType::Compute, "", data.data() } };
}
