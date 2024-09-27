#include <Engine/ShaderProgramDeserializer.hpp>

#include <format>

#include <Engine/Log.hpp>
#include <Engine/FileNode.hpp>

ShaderProgramDeserializationResult DeserializeShaderProgram(const FNode& file) {
    file.ReadContent();
    return DeserializeShaderProgram(file.DisposeContent());
}
ShaderProgramDeserializationResult DeserializeShaderProgram(std::string_view data) {
    ShaderProgramDeserializationResult rv;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.data());
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Error while parsing program file. Did you edit it? Don't.");
        DOA_LOG_ERROR("Couldn't deserialize shader program!\n\n%s", data.data());
        return rv;
    }

    ShaderProgramDeserializer::Deserialize(*doc.RootElement(), rv);
    return rv;
}

void ShaderProgramDeserializer::DefaultDeserialize(const tinyxml2::XMLElement& rootNode, ShaderProgramDeserializationResult& spdr) {
    ProgramConfig::Deserialize(*rootNode.FirstChildElement("config"), spdr);
    Shaders::Deserialize(*rootNode.FirstChildElement("shaders"), spdr);
}

void ShaderProgramDeserializer::ProgramConfig::DefaultDeserialize(const tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr) {
    DeserializeName(configNode, spdr);
}
void ShaderProgramDeserializer::ProgramConfig::DefaultDeserializeName(const tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.Name = configNode.Attribute("name");
}

void ShaderProgramDeserializer::Shaders::DefaultDeserialize(const tinyxml2::XMLElement& shadersNode, ShaderProgramDeserializationResult& spdr) {
    const tinyxml2::XMLElement* node;

    node = shadersNode.FirstChildElement("vertex-shader");
    if (!node) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "vertex"));
        return;
    }
    DeserializeVertexShader(*node, spdr);

    node = shadersNode.FirstChildElement("tess-ctrl-shader");
    if (!node) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "tess-ctrl"));
        return;
    }
    DeserializeTessellationControlShader(*node, spdr);

    node = shadersNode.FirstChildElement("tess-eval-shader");
    if (!node) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "tess-eval"));
        return;
    }
    DeserializeTessellationEvaluationShader(*node, spdr);

    node = shadersNode.FirstChildElement("geometry-shader");
    if (!node) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "geometry"));
        return;
    }
    DeserializeGeometryShader(*node, spdr);

    node = shadersNode.FirstChildElement("fragment-shader");
    if (!node) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "fragment"));
        return;
    }
    DeserializeFragmentShader(*node, spdr);

    node = shadersNode.FirstChildElement("compute-shader");
    if (!node) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "compute"));
        return;
    }
    DeserializeComputeShader(*node, spdr);
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeVertexShader(const tinyxml2::XMLElement& vertexShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.VertexShader = static_cast<UUID>(vertexShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeTessellationControlShader(const tinyxml2::XMLElement& tessCtrlShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.TessellationControlShader = static_cast<UUID>(tessCtrlShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeTessellationEvaluationShader(const tinyxml2::XMLElement& tessEvalShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.TessellationEvaluationShader = static_cast<UUID>(tessEvalShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeGeometryShader(const tinyxml2::XMLElement& geometryShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.GeometryShader = static_cast<UUID>(geometryShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeFragmentShader(const tinyxml2::XMLElement& fragmentShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.FragmentShader = static_cast<UUID>(fragmentShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeComputeShader(const tinyxml2::XMLElement& computeShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.ComputeShader = static_cast<UUID>(computeShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}