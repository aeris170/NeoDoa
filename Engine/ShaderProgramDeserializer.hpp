#pragma once

#include <functional>
#include <string_view>

#include <tinyxml2.h>

#include <Engine/Shader.hpp>

struct FNode;

struct ShaderProgramDeserializationResult {
    bool erred{ false };
    std::vector<std::string> errors{};
    ShaderProgram deserializedShaderProgram;
};

ShaderProgramDeserializationResult DeserializeShaderProgram(const FNode& file);
ShaderProgramDeserializationResult DeserializeShaderProgram(std::string_view data);

/* Here is the general overview */
namespace ShaderProgramDeserializer {

    /* ------- Type Definitons ------- */
    using DeserializeFunction                                 = std::function<void(tinyxml2::XMLElement& rootNode, ShaderProgramDeserializationResult& spdr)>;
    namespace ProgramConfig {
        using DeserializeFunction                             = std::function<void(tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeNameFunction                         = std::function<void(tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr)>;
    }
    namespace Shaders {
        using DeserializeFunction                             = std::function<void(tinyxml2::XMLElement& shadersNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeVertexShaderFunction                 = std::function<void(tinyxml2::XMLElement& vertexShaderNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeTessellationControlShaderFunction    = std::function<void(tinyxml2::XMLElement& tessCtrlShaderNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeTessellationEvaluationShaderFunction = std::function<void(tinyxml2::XMLElement& tessEvalShaderNode, ShaderProgramDeserializationResult& spdr) >;
        using DeserializeGeometryShaderFunction               = std::function<void(tinyxml2::XMLElement& geometryShaderNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeFragmentShaderFunction               = std::function<void(tinyxml2::XMLElement& fragmentShaderNode, ShaderProgramDeserializationResult& spdr)>;
        using DeserializeComputeShaderFunction                = std::function<void(tinyxml2::XMLElement& computeShaderNode, ShaderProgramDeserializationResult& spdr)>;
    }
    namespace Linking {
        using LinkFunction                                    = std::function<void(ShaderProgramDeserializationResult& spdr)>;
        using OpenGLLinkFunction                              = std::function<void(ShaderProgramDeserializationResult& spdr)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will deserialize by default. */
    void DefaultDeserialize(tinyxml2::XMLElement& rootNode, ShaderProgramDeserializationResult& spdr);
    namespace ProgramConfig {
        void DefaultDeserialize(tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeName(tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr);
    }
    namespace Shaders {
        void DefaultDeserialize(tinyxml2::XMLElement& shadersNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeVertexShader(tinyxml2::XMLElement& vertexShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeTessellationControlShader(tinyxml2::XMLElement& tessCtrlShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeTessellationEvaluationShader(tinyxml2::XMLElement& tessEvalShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeGeometryShader(tinyxml2::XMLElement& geometryShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeFragmentShader(tinyxml2::XMLElement& fragmentShaderNode, ShaderProgramDeserializationResult& spdr);
        void DefaultDeserializeComputeShader(tinyxml2::XMLElement& computeShaderNode, ShaderProgramDeserializationResult& spdr);
    }
    namespace Linking {
        void DefaultLink(ShaderProgramDeserializationResult& spdr);
        void DefaultOpenGLLink(ShaderProgramDeserializationResult& spdr);
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
    namespace Linking {
        inline LinkFunction Link{ DefaultLink };                                                                                                          /* Feel free to assign this your own function, if you need custom linking */
        inline OpenGLLinkFunction OpenGLLink{ DefaultOpenGLLink };                                                                                        /* Feel free to assign this your own function, if you need custom linking */
    }
}
