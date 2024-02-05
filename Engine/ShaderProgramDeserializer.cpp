#include <Engine/ShaderProgramDeserializer.hpp>

#include <regex>
#include <vector>
#include <format>
#include <string>

#include <Utility/Trim.hpp>

#include <Engine/Log.hpp>
#include <Engine/Core.hpp>
#include <Engine/FileNode.hpp>

static std::vector<std::string> SplitLinkerMessages(const std::string& messages) {
    static const std::regex regex{ "\n(?!\\s)" };

    std::vector<std::string> rv;
    std::copy(
        std::sregex_token_iterator{ messages.cbegin(), messages.cend(), regex, -1 },
        std::sregex_token_iterator{},
        std::back_inserter(rv)
    );

    return rv;
}

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
        DOA_LOG_ERROR("Couldn't deserialize shader program!\n\n%s", data);
        return rv;
    }

    ShaderProgramDeserializer::Deserialize(*doc.RootElement(), rv);
    return rv;
}


void ShaderProgramDeserializer::DefaultDeserialize(tinyxml2::XMLElement& rootNode, ShaderProgramDeserializationResult& spdr) {
    ProgramConfig::Deserialize(*rootNode.FirstChildElement("config"), spdr);
    Shaders::Deserialize(*rootNode.FirstChildElement("shaders"), spdr);
    Linking::Link(spdr);
}

void ShaderProgramDeserializer::ProgramConfig::DefaultDeserialize(tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr) {
    DeserializeName(configNode, spdr);
}
void ShaderProgramDeserializer::ProgramConfig::DefaultDeserializeName(tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.Name = configNode.Attribute("name");
}

void ShaderProgramDeserializer::Shaders::DefaultDeserialize(tinyxml2::XMLElement& shadersNode, ShaderProgramDeserializationResult& spdr) {
    tinyxml2::XMLElement* node;

    node = shadersNode.FirstChildElement("vertex-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "vertex"));
    }
    DeserializeVertexShader(*node, spdr);

    node = shadersNode.FirstChildElement("tess-ctrl-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "tess-ctrl"));
    }
    DeserializeTessellationControlShader(*node, spdr);

    node = shadersNode.FirstChildElement("tess-eval-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "tess-eval"));
    }
    DeserializeTessellationEvaluationShader(*node, spdr);

    node = shadersNode.FirstChildElement("geometry-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "geometry"));
    }
    DeserializeGeometryShader(*node, spdr);

    node = shadersNode.FirstChildElement("fragment-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "fragment"));
    }
    DeserializeFragmentShader(*node, spdr);
    //DeserializeVertexShader(shadersNode.FirstChildElement("compute-shader"), program);
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeVertexShader(tinyxml2::XMLElement& vertexShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.VertexShader = static_cast<UUID>(vertexShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeTessellationControlShader(tinyxml2::XMLElement& tessCtrlShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.TessellationControlShader = static_cast<UUID>(tessCtrlShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeTessellationEvaluationShader(tinyxml2::XMLElement& tessEvalShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.TessellationEvaluationShader = static_cast<UUID>(tessEvalShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeGeometryShader(tinyxml2::XMLElement& geometryShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.GeometryShader = static_cast<UUID>(geometryShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeFragmentShader(tinyxml2::XMLElement& fragmentShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.FragmentShader = static_cast<UUID>(fragmentShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeComputeShader(tinyxml2::XMLElement& computeShaderNode, ShaderProgramDeserializationResult& spdr) {
    //program.vertexShader = computeShaderNode.FindAttribute("id")->Unsigned64Attribute();
}

void ShaderProgramDeserializer::Linking::DefaultLink(ShaderProgramDeserializationResult& spdr) {
    if (spdr.erred) { return; } /* if we have errors, don't even try to link program. */
    OpenGLLink(spdr);
}
void ShaderProgramDeserializer::Linking::DefaultOpenGLLink(ShaderProgramDeserializationResult& spdr) {
    ShaderProgram& program = spdr.deserializedShaderProgram;
    if (!program.IsComplete()) {
        if (!program.HasVertexShader()) {
            spdr.erred = true;
            spdr.errors.emplace_back("Can't link program, program does not have a vertex shader!");
        }
        if (!program.HasFragmentShader()) {
            spdr.erred = true;
            spdr.errors.emplace_back("Can't link program, program does not have a fragment shader!");
        }
    }
    GLuint ID = glCreateProgram();
    std::array<GLuint, 5> attachedShaders{};

    const Assets& assets = *Core::GetCore()->Assets().get();

    // attach mandatory shaders - vertex and fragment
    AssetHandle vertexShaderAsset = assets.FindAsset(program.VertexShader);
    if (!vertexShaderAsset.HasValue()) {
        spdr.erred = true;
        spdr.errors.emplace_back("Can't link program, program references a non-existant vertex shader, has your shader successfully deserialized?");
    }
    AssetHandle fragmentShaderAsset = assets.FindAsset(program.FragmentShader);
    if (!fragmentShaderAsset.HasValue()) {
        spdr.erred = true;
        spdr.errors.emplace_back("Can't link program, program references a non-existant fragment shader, has your shader successfully deserialized?");
    }
    if (!vertexShaderAsset.HasValue() || !fragmentShaderAsset.HasValue()) {
        spdr.errors.emplace_back("Please fix errors before continuing.");
        return;
    }
    const Shader& vertexShader = vertexShaderAsset->DataAs<Shader>();
    glAttachShader(ID, vertexShader.ID);
    const Shader& fragmentShader = fragmentShaderAsset->DataAs<Shader>();
    glAttachShader(ID, fragmentShader.ID);
    attachedShaders[0] = vertexShader.ID;
    attachedShaders[4] = fragmentShader.ID;

    // attach optional shaders - tess_ctrl, tess_eval and geometry
    bool usingTessellation{ program.HasTessellationEvaluationShader() };
    if (usingTessellation && program.HasTessellationControlShader()) {
        AssetHandle tessCtrlShaderAsset = assets.FindAsset(program.TessellationControlShader);
        if (tessCtrlShaderAsset.HasValue()) {
            const Shader& tessCtrlShader = tessCtrlShaderAsset->DataAs<Shader>();
            glAttachShader(ID, tessCtrlShader.ID);
            attachedShaders[1] = tessCtrlShader.ID;
        }
    }
    if (usingTessellation) {
        AssetHandle tessEvalShaderAsset = assets.FindAsset(program.TessellationEvaluationShader);
        if (!tessEvalShaderAsset.HasValue()) {
            spdr.erred = true;
            spdr.errors.emplace_back("Can't link program, program uses tessellation but references a non-existant evaluation shader, has your shader successfully deserialized?");
            return;
        }
        const Shader& tessEvalShader = tessEvalShaderAsset->DataAs<Shader>();
        glAttachShader(ID, tessEvalShader.ID);
        attachedShaders[2] = tessEvalShader.ID;
    }
    if (program.HasGeometryShader()) {
        AssetHandle geometryShaderAsset = assets.FindAsset(program.GeometryShader);
        if (!geometryShaderAsset.HasValue()) {
            spdr.erred = true;
            spdr.errors.emplace_back("Can't link program, program references a non-existant geometry shader, has your shader successfully deserialized?");
            return;
        }
        const Shader& geometryShader = geometryShaderAsset->DataAs<Shader>();
        glAttachShader(ID, geometryShader.ID);
        attachedShaders[3] = geometryShader.ID;
    }

    int success;
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        spdr.erred = true;
        spdr.errors.emplace_back("Program linking failed!");
        glDeleteProgram(ID);
    } else {
        program.ID = ID;
    }

    GLint bufferLength;
    glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetProgramInfoLog(ID, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitLinkerMessages(logString);
        for (auto& log : logs) {
            spdr.errors.emplace_back(std::move(log));
        }
        delete[] logChars;
    }

    for (const auto shader : attachedShaders) {
        glDetachShader(ID, shader);
    }
}