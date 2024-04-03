#pragma once

#include <string>
#include <functional>
#include <string_view>

#include <tinyxml2.h>

#include <Engine/Shader.hpp>

/* This is the big bad bada-boom of serializers, this function will call appropriate serializers. See below for an overview! */
std::string SerializeShaderProgram(const ShaderProgram& program);

/* Here is the general overview */
namespace ShaderProgramSerializer {

    /* ------- Type Definitons ------- */
    using HeaderCommentFunction                          = std::function<void(tinyxml2::XMLPrinter& printer, const ShaderProgram& program)>;
    using SerializeFunction                              = std::function<void(tinyxml2::XMLPrinter& printer, const ShaderProgram& program)>;
    namespace ProgramConfig {
        using SerializeFunction                          = std::function<void(tinyxml2::XMLPrinter& printer, const ShaderProgram& program)>;
        using SerializeNameFunction                      = std::function<void(tinyxml2::XMLPrinter& printer, std::string_view name)>;
    }
    namespace Shaders {
        using SerializeFunction                             = std::function<void(tinyxml2::XMLPrinter& printer, const ShaderProgram& program)>;
        using SerializeVertexShaderFunction                 = std::function<void(tinyxml2::XMLPrinter& printer, const UUID& vertexShader)>;
        using SerializeTessellationControlShaderFunction    = std::function<void(tinyxml2::XMLPrinter& printer, const UUID& tessellationControlShader)>;
        using SerializeTessellationEvaluationShaderFunction = std::function<void(tinyxml2::XMLPrinter& printer, const UUID& tessellationEvaluationShader)>;
        using SerializeGeometryShaderFunction               = std::function<void(tinyxml2::XMLPrinter& printer, const UUID& geometryShader)>;
        using SerializeFragmentShaderFunction               = std::function<void(tinyxml2::XMLPrinter& printer, const UUID& fragmentShader)>;
        using SerializeComputeShaderFunction                = std::function<void(tinyxml2::XMLPrinter& printer, const UUID& computeShader)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will serialize by default. */
    void DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const ShaderProgram& program);
    void DefaultSerialize(tinyxml2::XMLPrinter& printer, const ShaderProgram& program);
    namespace ProgramConfig {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const ShaderProgram& program);
        void DefaultSerializeName(tinyxml2::XMLPrinter& printer, ::std::string_view name);
    }
    namespace Shaders {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const ShaderProgram& program);
        void DefaultSerializeVertexShader(tinyxml2::XMLPrinter& printer, const UUID& vertexShader);
        void DefaultSerializeTessellationControlShader(tinyxml2::XMLPrinter& printer, const UUID& tessellationControlShader);
        void DefaultSerializeTessellationEvaluationShader(tinyxml2::XMLPrinter& printer, const UUID& tessellationEvaluationShader);
        void DefaultSerializeGeometryShader(tinyxml2::XMLPrinter& printer, const UUID& geometryShader);
        void DefaultSerializeFragmentShader(tinyxml2::XMLPrinter& printer, const UUID& fragmentShader);
        void DefaultSerializeComputeShader(tinyxml2::XMLPrinter& printer, const UUID& computeShader);
    }

    /* ----- Serializer Functions ----- */
    inline HeaderCommentFunction HeaderComment{ DefaultHeaderComment };                                                                             /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeFunction Serialize{ DefaultSerialize };                                                                                         /* Feel free to assign this your own function, if you need custom serialization */
    namespace ProgramConfig {
        inline SerializeFunction Serialize{ DefaultSerialize };                                                                                     /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeNameFunction SerializeName{ DefaultSerializeName };                                                                         /* Feel free to assign this your own function, if you need custom serialization */
    }
    namespace Shaders {
        inline SerializeFunction Serialize{ DefaultSerialize };                                                                                     /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeVertexShaderFunction SerializeVertexShader{ DefaultSerializeVertexShader };                                                 /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeTessellationControlShaderFunction SerializeTessellationControlShader{ DefaultSerializeTessellationControlShader };          /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeTessellationEvaluationShaderFunction SerializeTessellationEvaluationShader{ DefaultSerializeTessellationEvaluationShader }; /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeGeometryShaderFunction SerializeGeometryShader{ DefaultSerializeGeometryShader };                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeFragmentShaderFunction SerializeFragmentShader{ DefaultSerializeFragmentShader };                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeComputeShaderFunction SerializeComputeShader{ DefaultSerializeComputeShader };                                              /* Feel free to assign this your own function, if you need custom serialization */
    }
}
