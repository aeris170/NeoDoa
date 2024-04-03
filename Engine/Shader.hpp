#pragma once

#include <string>
#include <vector>
#include <optional>

#include <GL/glew.h>

#include <Engine/UUID.hpp>
#include <Engine/Graphics.hpp>

struct Shader {
    ShaderType Type;
    std::string Name;
    std::string SourceCode;

    std::string Serialize() const noexcept;
    static Shader Deserialize(const std::string_view data) noexcept;

    static Shader Copy(const Shader& shader) noexcept;
};

struct ShaderProgram {
    std::string Name;

    UUID VertexShader                { UUID::Empty() };
    UUID TessellationControlShader   { UUID::Empty() };
    UUID TessellationEvaluationShader{ UUID::Empty() };
    UUID GeometryShader              { UUID::Empty() };
    UUID FragmentShader              { UUID::Empty() };
    UUID ComputeShader               { UUID::Empty() };

    bool IsComplete() const noexcept;
    bool HasVertexShader() const noexcept;
    bool HasTessellationControlShader() const noexcept;
    bool HasTessellationEvaluationShader() const noexcept;
    bool HasGeometryShader() const noexcept;
    bool HasFragmentShader() const noexcept;
    bool HasComputeShader() const noexcept;

    std::string Serialize() const noexcept;
    static ShaderProgram Deserialize(const std::string_view data) noexcept;

    static ShaderProgram Copy(const ShaderProgram& program) noexcept;
};