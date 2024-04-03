#pragma once

#include <vector>
#include <string>
#include <functional>
#include <string_view>

#include <tinyxml2.h>

#include <Engine/Shader.hpp>
#include <Engine/GPUShader.hpp>

struct FNode;

struct ShaderProgramDeserializationResult {
    bool erred{ false };
    std::vector<ShaderLinkerMessage> errors{};
    ShaderProgram deserializedShaderProgram;
};

ShaderProgramDeserializationResult DeserializeShaderProgram(const FNode& file);
ShaderProgramDeserializationResult DeserializeShaderProgram(std::string_view data);

/* Here is the general overview */
namespace ShaderProgramDeserializer {

    /* ------- Type Definitons ------- */
    using DeserializeFunction                                 = std::function<void(const tinyxml2::XMLElement& rootNode, ShaderProgramDeserializationResult& spdr)>;
    namespace ProgramConfig {
        using DeserializeFunction                             = std::function<void(const tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeNameFunction                         = std::function<void(const tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr)>;
    }
    namespace Shaders {
        using DeserializeFunction                             = std::function<void(const tinyxml2::XMLElement& shadersNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeVertexShaderFunction                 = std::function<void(const tinyxml2::XMLElement& vertexShaderNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeTessellationControlShaderFunction    = std::function<void(const tinyxml2::XMLElement& tessCtrlShaderNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeTessellationEvaluationShaderFunction = std::function<void(const tinyxml2::XMLElement& tessEvalShaderNode, ShaderProgramDeserializationResult& spdr) >;
        using DeserializeGeometryShaderFunction               = std::function<void(const tinyxml2::XMLElement& geometryShaderNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeFragmentShaderFunction               = std::function<void(const tinyxml2::XMLElement& fragmentShaderNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeComputeShaderFunction                = std::function<void(const tinyxml2::XMLElement& computeShaderNode, ShaderProgramDeserializationResult& spdr)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will deserialize by default. */
    void DefaultDeserialize(const tinyxml2::XMLElement& rootNode, ShaderProgramDeserializationResult& spdr);
    namespace ProgramConfig {
        void DefaultDeserialize(const tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeName(const tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr);
    }
    namespace Shaders {
        void DefaultDeserialize(const tinyxml2::XMLElement& shadersNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeVertexShader(const tinyxml2::XMLElement& vertexShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeTessellationControlShader(const tinyxml2::XMLElement& tessCtrlShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeTessellationEvaluationShader(const tinyxml2::XMLElement& tessEvalShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeGeometryShader(const tinyxml2::XMLElement& geometryShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeFragmentShader(const tinyxml2::XMLElement& fragmentShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeComputeShader(const tinyxml2::XMLElement& computeShaderNode, ShaderProgramDeserializationResult& spdr);
    }

    /* ----- Deserializer Functions ----- */
    inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                                         /* Feel free to assign this your own function, if you need custom serialization */
    namespace ProgramConfig {
        inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                                     /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeNameFunction DeserializeName{ DefaultDeserializeName };                                                                         /* Feel free to assign this your own function, if you need custom serialization */
    }
    namespace Shaders {
        inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                                     /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeVertexShaderFunction DeserializeVertexShader{ DefaultDeserializeVertexShader };                                                 /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeTessellationControlShaderFunction DeserializeTessellationControlShader{ DefaultDeserializeTessellationControlShader };          /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeTessellationEvaluationShaderFunction DeserializeTessellationEvaluationShader{ DefaultDeserializeTessellationEvaluationShader }; /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeGeometryShaderFunction DeserializeGeometryShader{ DefaultDeserializeGeometryShader };                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeFragmentShaderFunction DeserializeFragmentShader{ DefaultDeserializeFragmentShader };                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeComputeShaderFunction DeserializeComputeShader{ DefaultDeserializeComputeShader };                                              /* Feel free to assign this your own function, if you need custom serialization */
    }
}
