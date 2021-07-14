#include "Shader.hpp"

#include <fstream>
#include <sstream>

#include <GL/glew.h>

#include "Log.hpp"
#include "Texture.hpp"

static std::unordered_map<std::string, std::shared_ptr<Shader>> SHADERS;

Shader::Shader(std::string_view name, ProgramID glProgramID) noexcept :
    _name(name),
    _glProgramID(glProgramID) {
    Uniformi("missing", 0);
    Uniformi("texture1", 1);
    Uniformi("texture2", 2);
    Uniformi("texture3", 3);
    Uniformi("texture4", 4);
    Uniformi("texture5", 5);
    Uniformi("texture6", 6);
    Uniformi("texture7", 7);
    Uniformi("texture8", 8);
    Uniformi("texture9", 9);
    Uniformi("texture10", 10);
    Uniformi("texture11", 11);
    Uniformi("texture12", 12);
    Uniformi("texture13", 13);
    Uniformi("texture14", 14);
    Uniformi("texture15", 15);
    Uniformi("texture16", 16);
}

Shader::~Shader() {
    glDeleteProgram(_glProgramID);
}

void Shader::Use() {
    glUseProgram(_glProgramID);
}

bool Shader::Uniformi(std::string_view name, int value) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glProgramUniform1i(_glProgramID, loc, value);
    return true;
}

bool Shader::Uniformiv(std::string_view name, int* values, int size) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glProgramUniform1iv(_glProgramID, loc, size, values);
    return true;
}

bool Shader::Uniform1f(std::string_view name, float value) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glProgramUniform1f(_glProgramID, loc, value);
    return true;
}

bool Shader::Uniform2f(std::string_view name, float* values) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glProgramUniform2fv(_glProgramID, loc, 2, values);
    return true;
}

bool Shader::Uniform3f(std::string_view name, float* values) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glProgramUniform3fv(_glProgramID, loc, 3, values);
    return true;
}

bool Shader::Uniform4f(std::string_view name, float* values) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glProgramUniform4fv(_glProgramID, loc, 4, values);
    return true;
}

bool Shader::UniformMat3(std::string_view name, float* values, bool transpose) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glProgramUniformMatrix3fv(_glProgramID, loc, 1, transpose, values);
    return true;
}

bool Shader::UniformMat4(std::string_view name, float* values, bool transpose) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glProgramUniformMatrix4fv(_glProgramID, loc, 1, transpose, values);
    return true;
}

UniformLocation Shader::UniformLoc(std::string_view name) {
    if (_uniforms.find(name.data()) != _uniforms.end()) {
        return _uniforms[name.data()];
    }
    return _uniforms[name.data()] = glGetUniformLocation(_glProgramID, name.data());
}

//-----------------------------------------------------------------

std::optional<std::weak_ptr<Shader>> CreateShader(std::string_view name, const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure ex) {
        DOA_LOG_ERROR("Shader %s coudln't be created. One or both of the source files couldn't be read!", name);
        return {};
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    ProgramID ID = glCreateProgram();

    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(v, 1, &vShaderCode, NULL);
    glShaderSource(f, 1, &fShaderCode, NULL);

    int success;
    char infoLog[512];

    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(v, 512, NULL, infoLog);
        DOA_LOG_ERROR(infoLog);
        return {};
    }

    glCompileShader(f);
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(v, 512, NULL, infoLog);
        DOA_LOG_ERROR(infoLog);
        return {};
    }

    glAttachShader(ID, v);
    glAttachShader(ID, f);

    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        DOA_LOG_ERROR(infoLog);
        return {};
    }

    glDetachShader(ID, v);
    glDetachShader(ID, f);

    glDeleteShader(v);
    glDeleteShader(f);

    auto rv = std::make_shared<Shader>(name, ID);
    SHADERS.emplace(name, rv);
    return rv;
}

std::optional<std::weak_ptr<Shader>> FindShader(std::string_view name) {
    auto it = SHADERS.find(name.data());
    if (it == SHADERS.end()) {
        DOA_LOG_WARNING("FindShader failed. There is no Shader named %s!", name);
        return {};
    }
    return it->second;
}
