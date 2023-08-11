#pragma once

#include <string>
#include <vector>
#include <optional>

#include <GL/glew.h>

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
    Type type;
    std::string name;
    std::string sourceCode;
    std::vector<Uniform> uniforms;

    std::string Serialize() const;
    static Shader Deserialize(const std::string_view data);

    static Shader Copy(const Shader& component);
};

struct ShaderProgram {

    std::string name;

                  Shader  vertexShader;
    std::optional<Shader> tesselationControlShader;
    std::optional<Shader> tesselationEvaluationShader;
    std::optional<Shader> geometryShader;
                  Shader  fragmentShader;

    std::vector<Shader::Uniform> uniforms;

    std::string Serialize() const;
    static ShaderProgram Deserialize(const std::string_view data);

    static ShaderProgram Copy(const ShaderProgram& component);
};
