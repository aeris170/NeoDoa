#pragma once

#include <string>
#include <vector>
#include <optional>

#include <GL/glew.h>

#include <Engine/UUID.hpp>

struct Shader {
    enum class ShaderType {
        Vertex,
        TessellationControl,
        TessellationEvaluation,
        Geometry,
        Fragment,
        Compute
    };

    struct Uniform {
        std::string TypeName;
        std::string Name;
    };

    GLuint ID;
    ShaderType Type;
    std::string Name;
    std::string SourceCode;
    std::vector<Uniform> Uniforms;

    std::string Serialize() const;
    static Shader Deserialize(const std::string_view data);

    static Shader Copy(const Shader& shader);
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

    std::string Serialize() const;
    static ShaderProgram Deserialize(const std::string_view data);

    static ShaderProgram Copy(const ShaderProgram& program);
};
