#pragma once

#include <string>
#include <functional>

#include <tinyxml2.h>

#include <Engine/UUID.hpp>
#include <Engine/Material.hpp>

/* This is the big bad bada-boom of serializers, this function will call appropriate serializers. See below for an overview! */
std::string SerializeMaterial(const Material& material) noexcept;

/* Here is the general overview */
namespace MaterialSerializer {

    /* ------- Type Definitons ------- */
    using HeaderCommentFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Material& material)>;
    using SerializeFunction     = std::function<void(tinyxml2::XMLPrinter& printer, const Material& material)>;
    namespace Program {
        using SerializeFunction   = std::function<void(tinyxml2::XMLPrinter& printer, const Material& material)>;
        using SerializeIDFunction = std::function<void(tinyxml2::XMLPrinter& printer, const UUID id)>;
    }
    namespace Uniforms {
        using SerializeFunction                               = std::function<void(tinyxml2::XMLPrinter& printer, const Material& material)>;
        using SerializeVertexUniformsFunction                 = std::function<void(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms)>;
        using SerializeTessellationControlUniformsFunction    = std::function<void(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms)>;
        using SerializeTessellationEvaluationUniformsFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms)>;
        using SerializeGeometryUniformsFunction               = std::function<void(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms)>;
        using SerializeFragmentUniformsFunction               = std::function<void(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms)>;
        using SerializeUniformFunction                        = std::function<void(tinyxml2::XMLPrinter& printer, const UniformValue& uniformValue)>;
        using SerializeUniformVectorFunction                  = std::function<void(tinyxml2::XMLPrinter& printer, const UniformValueVector& uniformVector)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will serialize by default. */
    void DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const Material& material) noexcept;
    void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Material& material) noexcept;
    namespace Program {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Material& material) noexcept;
        void DefaultSerializeID(tinyxml2::XMLPrinter& printer, const UUID id) noexcept;
    }
    namespace Uniforms {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Material& material) noexcept;
        void DefaultSerializeVertexUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept;
        void DefaultSerializeTessellationControlUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept;
        void DefaultSerializeTessellationEvaluationUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept;
        void DefaultSerializeGeometryUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept;
        void DefaultSerializeFragmentUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept;
        void DefaultSerializeUniform(tinyxml2::XMLPrinter& printer, const UniformValue& uniform) noexcept;
        void DefaultSerializeUniformVector(tinyxml2::XMLPrinter& printer, const UniformValueVector& uniformVector) noexcept;
    }

    /* ----- Serializer Functions ----- */
    inline HeaderCommentFunction HeaderComment{ DefaultHeaderComment };                                                                                   /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeFunction Serialize{ DefaultSerialize };                                                                                               /* Feel free to assign this your own function, if you need custom serialization */
    namespace Program {
        inline SerializeFunction Serialize{ DefaultSerialize };                                                                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeIDFunction SerializeID{ DefaultSerializeID };                                                                                     /* Feel free to assign this your own function, if you need custom serialization */
    }
    namespace Uniforms {
        inline SerializeFunction Serialize{ DefaultSerialize };                                                                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeVertexUniformsFunction SerializeVertexUniforms{ DefaultSerializeVertexUniforms };                                                 /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeTessellationControlUniformsFunction SerializeTessellationControlUniforms{ DefaultSerializeTessellationControlUniforms };          /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeTessellationEvaluationUniformsFunction SerializeTessellationEvaluationUniforms{ DefaultSerializeTessellationEvaluationUniforms }; /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeGeometryUniformsFunction SerializeGeometryUniforms{ DefaultSerializeGeometryUniforms };                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeFragmentUniformsFunction SerializeFragmentUniforms{ DefaultSerializeFragmentUniforms };                                           /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeUniformFunction SerializeUniform{ DefaultSerializeUniform };                                                                      /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeUniformVectorFunction SerializeUniformVector{ DefaultSerializeUniformVector };                                                    /* Feel free to assign this your own function, if you need custom serialization */
    }
}

/* Some Helper functions to trivially serialize (not-so) trivial data types */
namespace MaterialSerializer::Helpers {
    std::string_view ExtractUniformTypeString(const UniformValue& uniform) noexcept;
    std::string_view ExtractUniformTypeString(const UniformValueVector& uniformVector) noexcept;

    void SerializeUniform1f(tinyxml2::XMLPrinter& printer, const Uniform1f& uniformValue) noexcept;
    void SerializeUniform2f(tinyxml2::XMLPrinter& printer, const Uniform2f& uniformValue) noexcept;
    void SerializeUniform3f(tinyxml2::XMLPrinter& printer, const Uniform3f& uniformValue) noexcept;
    void SerializeUniform4f(tinyxml2::XMLPrinter& printer, const Uniform4f& uniformValue) noexcept;

    void SerializeUniform1i(tinyxml2::XMLPrinter& printer, const Uniform1i& uniformValue) noexcept;
    void SerializeUniform2i(tinyxml2::XMLPrinter& printer, const Uniform2i& uniformValue) noexcept;
    void SerializeUniform3i(tinyxml2::XMLPrinter& printer, const Uniform3i& uniformValue) noexcept;
    void SerializeUniform4i(tinyxml2::XMLPrinter& printer, const Uniform4i& uniformValue) noexcept;

    void SerializeUniform1ui(tinyxml2::XMLPrinter& printer, const Uniform1ui& uniformValue) noexcept;
    void SerializeUniform2ui(tinyxml2::XMLPrinter& printer, const Uniform2ui& uniformValue) noexcept;
    void SerializeUniform3ui(tinyxml2::XMLPrinter& printer, const Uniform3ui& uniformValue) noexcept;
    void SerializeUniform4ui(tinyxml2::XMLPrinter& printer, const Uniform4ui& uniformValue) noexcept;

    void SerializeUniformMatrix2f(tinyxml2::XMLPrinter& printer, const UniformMatrix2f& uniformValue) noexcept;
    void SerializeUniformMatrix3f(tinyxml2::XMLPrinter& printer, const UniformMatrix3f& uniformValue) noexcept;
    void SerializeUniformMatrix4f(tinyxml2::XMLPrinter& printer, const UniformMatrix4f& uniformValue) noexcept;
    void SerializeUniformMatrix2x3f(tinyxml2::XMLPrinter& printer, const UniformMatrix2x3f& uniformValue) noexcept;
    void SerializeUniformMatrix3x2f(tinyxml2::XMLPrinter& printer, const UniformMatrix3x2f& uniformValue) noexcept;
    void SerializeUniformMatrix2x4f(tinyxml2::XMLPrinter& printer, const UniformMatrix2x4f& uniformValue) noexcept;
    void SerializeUniformMatrix4x2f(tinyxml2::XMLPrinter& printer, const UniformMatrix4x2f& uniformValue) noexcept;
    void SerializeUniformMatrix3x4f(tinyxml2::XMLPrinter& printer, const UniformMatrix3x4f& uniformValue) noexcept;
    void SerializeUniformMatrix4x3f(tinyxml2::XMLPrinter& printer, const UniformMatrix4x3f& uniformValue) noexcept;
}