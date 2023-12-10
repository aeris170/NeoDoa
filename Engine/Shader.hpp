#pragma once

#include <string>
#include <vector>
#include <optional>

#include <GL/glew.h>

#include <Engine/UUID.hpp>

struct Shader {
    enum class Type {
        VERTEX,
        TESS_CTRL,
        TESS_EVAL,
        GEOMETRY,
        FRAGMENT,
        COMPUTE
    };

    struct Uniform {
        std::string typeName;
        std::string name;
    };

    GLuint ID;
    Type Type;
    std::string Name;
    std::string SourceCode;
    std::vector<Uniform> Uniforms;

    std::string Serialize() const;
    static Shader Deserialize(const std::string_view data);

    static Shader Copy(const Shader& component);
};

struct ShaderProgram {

    GLuint ID;
    std::string Name;

    UUID VertexShader                { UUID::Empty() };
    UUID TessellationControlShader   { UUID::Empty() };
    UUID TessellationEvaluationShader{ UUID::Empty() };
    UUID GeometryShader              { UUID::Empty() };
    UUID FragmentShader              { UUID::Empty() };

    bool IsComplete() const;
    bool HasVertexShader() const;
    bool HasTessellationControlShader() const;
    bool HasTessellationEvaluationShader() const;
    bool HasGeometryShader() const;
    bool HasFragmentShader() const;

    std::vector<Shader::Uniform> Uniforms;

    std::string Serialize() const;
    static ShaderProgram Deserialize(const std::string_view data);

    static ShaderProgram Copy(const ShaderProgram& component);
};
