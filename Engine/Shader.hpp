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

    GLuint ID;
    ShaderType Type;
    std::string Name;
    std::string SourceCode;

    std::string Serialize() const noexcept;
    static Shader Deserialize(const std::string_view data) noexcept;

    static Shader Copy(const Shader& shader) noexcept;
};

struct ShaderProgram {
    struct Uniform {
        int Location;
        std::string TypeName;
        std::string Name;
        int ArraySize;
        Shader::ShaderType ReferencedBy;
    };

    GLuint ID;
    std::string Name;

    UUID VertexShader                { UUID::Empty() };
    UUID TessellationControlShader   { UUID::Empty() };
    UUID TessellationEvaluationShader{ UUID::Empty() };
    UUID GeometryShader              { UUID::Empty() };
    UUID FragmentShader              { UUID::Empty() };

    std::vector<Uniform> Uniforms{};

    bool IsComplete() const noexcept;
    bool HasVertexShader() const noexcept;
    bool HasTessellationControlShader() const noexcept;
    bool HasTessellationEvaluationShader() const noexcept;
    bool HasGeometryShader() const noexcept;
    bool HasFragmentShader() const noexcept;

    int GetUniformLocation(std::string_view name) const noexcept;

    std::string Serialize() const noexcept;
    static ShaderProgram Deserialize(const std::string_view data) noexcept;

    static ShaderProgram Copy(const ShaderProgram& program) noexcept;
};
