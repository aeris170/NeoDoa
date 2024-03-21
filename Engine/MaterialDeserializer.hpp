#pragma once

#include <string>
#include <functional>
#include <string_view>

#include <tinyxml2.h>

#include <Engine/Material.hpp>

struct FNode;

struct MaterialDeserializationResult {
    bool erred{ false };
    std::vector<std::string> errors{};
    Material deserializedMaterial{ "" };
};

/* This is the big bad bada-boom of deserializers, this function will call appropriate deserializers. See below for an overview! */
MaterialDeserializationResult DeserializeMaterial(const FNode& file) noexcept;
MaterialDeserializationResult DeserializeMaterial(const std::string_view data) noexcept;

/* Here is the general overview */
namespace MaterialDeserializer {

    /* ------- Type Definitons ------- */
    using DeserializeFunction = std::function<void(tinyxml2::XMLElement& rootElem, MaterialDeserializationResult& mdr)>;
    namespace Program {
        using DeserializeFunction   = std::function<void(tinyxml2::XMLElement& programElem, MaterialDeserializationResult& mdr)>;
        using DeserializeIDFunction = std::function<void(tinyxml2::XMLElement& programElem, MaterialDeserializationResult& mdr)>;
    }
    namespace Uniforms {
        using DeserializeFunction                               = std::function<void(tinyxml2::XMLElement& uniformElem, MaterialDeserializationResult& mdr)>;
        using DeserializeVertexUniformsFunction                 = std::function<void(tinyxml2::XMLElement& vertexUniformsElem, MaterialDeserializationResult& mdr)>;
        using DeserializeTessellationControlUniformsFunction    = std::function<void(tinyxml2::XMLElement& tessellationControlUniformsElem, MaterialDeserializationResult& mdr)>;
        using DeserializeTessellationEvaluationUniformsFunction = std::function<void(tinyxml2::XMLElement& tessellationEvaluationUniformsElem, MaterialDeserializationResult& mdr)>;
        using DeserializeGeometryUniformsFunction               = std::function<void(tinyxml2::XMLElement& geometryUniformsElem, MaterialDeserializationResult& mdr)>;
        using DeserializeFragmentUniformsFunction               = std::function<void(tinyxml2::XMLElement& fragmentUniformsElem, MaterialDeserializationResult& mdr)>;
        using DeserializeUniformFunction                        = std::function<void(tinyxml2::XMLElement& uniformElem, MaterialDeserializationResult& mdr, Material::Uniforms& uniformsToFill)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will serialize by default. */
    void DefaultDeserialize(tinyxml2::XMLElement& rootElem, MaterialDeserializationResult& mdr) noexcept;
    namespace Program {
        void DefaultDeserialize(tinyxml2::XMLElement& programElem, MaterialDeserializationResult& mdr) noexcept;
        void DefaultDeserializeID(tinyxml2::XMLElement& programElem, MaterialDeserializationResult& mdr) noexcept;
    }
    namespace Uniforms {
        void DefaultDeserialize(tinyxml2::XMLElement& uniformElem, MaterialDeserializationResult& mdr) noexcept;
        void DefaultDeserializeVertexUniforms(tinyxml2::XMLElement& vertexUniformsElem, MaterialDeserializationResult& mdr) noexcept;
        void DefaultDeserializeTessellationControlUniforms(tinyxml2::XMLElement& tessellationControlUniformsElem, MaterialDeserializationResult& mdr) noexcept;
        void DefaultDeserializeTessellationEvaluationUniforms(tinyxml2::XMLElement& tessellationEvaluationUniformsElem, MaterialDeserializationResult& mdr) noexcept;
        void DefaultDeserializeGeometryUniforms(tinyxml2::XMLElement& geometryUniformsElem, MaterialDeserializationResult& mdr) noexcept;
        void DefaultDeserializeFragmentUniforms(tinyxml2::XMLElement& fragmentUniformsElem, MaterialDeserializationResult& mdr) noexcept;
        void DefaultDeserializeUniform(tinyxml2::XMLElement& uniformElem, MaterialDeserializationResult& mdr, Material::Uniforms& uniformsToFill) noexcept;
    }

    /* ----- Deserializer Functions ----- */
    inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                                               /* Feel free to assign this your own function, if you need custom serialization */
    namespace Program {
        inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeIDFunction DeserializeID{ DefaultDeserializeID };                                                                                     /* Feel free to assign this your own function, if you need custom serialization */
    }
    namespace Uniforms {
        inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeVertexUniformsFunction DeserializeVertexUniforms{ DefaultDeserializeVertexUniforms };                                                 /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeTessellationControlUniformsFunction DeserializeTessellationControlUniforms{ DefaultDeserializeTessellationControlUniforms };          /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeTessellationEvaluationUniformsFunction DeserializeTessellationEvaluationUniforms{ DefaultDeserializeTessellationEvaluationUniforms }; /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeGeometryUniformsFunction DeserializeGeometryUniforms{ DefaultDeserializeGeometryUniforms };                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeFragmentUniformsFunction DeserializeFragmentUniforms{ DefaultDeserializeFragmentUniforms };                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeUniformFunction DeserializeUniform{ DefaultDeserializeUniform };                                                                      /* Feel free to assign this your own function, if you need custom serialization */
    }
}

/* Some Helper functions to trivially deserialize (not-so) trivial data types */
namespace MaterialDeserializer::Helpers {
    enum class UniformType {
        Uniform1f,
        Uniform2f,
        Uniform3f,
        Uniform4f,

        Uniform1i,
        Uniform2i,
        Uniform3i,
        Uniform4i,

        Uniform1ui,
        Uniform2ui,
        Uniform3ui,
        Uniform4ui,

        UniformMatrix2f,
        UniformMatrix3f,
        UniformMatrix4f,
        UniformMatrix2x3f,
        UniformMatrix3x2f,
        UniformMatrix2x4f,
        UniformMatrix4x2f,
        UniformMatrix3x4f,
        UniformMatrix4x3f,

        InvalidEnum
    };
    UniformType ExtractUniformType(const std::string_view typeString) noexcept;

    Uniform1f DeserializeUniform1f(const tinyxml2::XMLElement& elem) noexcept;
    Uniform2f DeserializeUniform2f(const tinyxml2::XMLElement& elem) noexcept;
    Uniform3f DeserializeUniform3f(const tinyxml2::XMLElement& elem) noexcept;
    Uniform4f DeserializeUniform4f(const tinyxml2::XMLElement& elem) noexcept;

    Uniform1i DeserializeUniform1i(const tinyxml2::XMLElement& elem) noexcept;
    Uniform2i DeserializeUniform2i(const tinyxml2::XMLElement& elem) noexcept;
    Uniform3i DeserializeUniform3i(const tinyxml2::XMLElement& elem) noexcept;
    Uniform4i DeserializeUniform4i(const tinyxml2::XMLElement& elem) noexcept;

    Uniform1ui DeserializeUniform1ui(const tinyxml2::XMLElement& elem) noexcept;
    Uniform2ui DeserializeUniform2ui(const tinyxml2::XMLElement& elem) noexcept;
    Uniform3ui DeserializeUniform3ui(const tinyxml2::XMLElement& elem) noexcept;
    Uniform4ui DeserializeUniform4ui(const tinyxml2::XMLElement& elem) noexcept;

    UniformMatrix2f DeserializeUniformMatrix2f(const tinyxml2::XMLElement& elem) noexcept;
    UniformMatrix3f DeserializeUniformMatrix3f(const tinyxml2::XMLElement& elem) noexcept;
    UniformMatrix4f DeserializeUniformMatrix4f(const tinyxml2::XMLElement& elem) noexcept;
    UniformMatrix2x3f DeserializeUniformMatrix2x3f(const tinyxml2::XMLElement& elem) noexcept;
    UniformMatrix3x2f DeserializeUniformMatrix3x2f(const tinyxml2::XMLElement& elem) noexcept;
    UniformMatrix2x4f DeserializeUniformMatrix2x4f(const tinyxml2::XMLElement& elem) noexcept;
    UniformMatrix4x2f DeserializeUniformMatrix4x2f(const tinyxml2::XMLElement& elem) noexcept;
    UniformMatrix3x4f DeserializeUniformMatrix3x4f(const tinyxml2::XMLElement& elem) noexcept;
    UniformMatrix4x3f DeserializeUniformMatrix4x3f(const tinyxml2::XMLElement& elem) noexcept;
}