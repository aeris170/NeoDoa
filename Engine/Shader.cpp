#include "Shader.hpp"

#include <fstream>
#include <sstream>

#include <GL/glew.h>

#include "Log.hpp"
#include "Texture.hpp"

static std::unordered_map<std::string, std::shared_ptr<Shader>> SHADERS;

Shader::Shader(std::string_view name, ProgramID glProgramID) noexcept :
    _name(name),
    _glProgramID(glProgramID) {}

Shader::~Shader() {
    glDeleteProgram(_glProgramID);
}

void Shader::Use() {
    glUseProgram(_glProgramID);
}

bool Shader::Uniformi(std::string_view name, int value) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glUniform1i(loc, value);
    return true;
}

bool Shader::Uniformiv(std::string_view name, int* values, int size) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glUniform1iv(loc, size, values);
    return true;
}

bool Shader::Uniform1f(std::string_view name, float value) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glUniform1f(loc, value);
    return true;
}

bool Shader::Uniform2f(std::string_view name, float* values) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glUniform2fv(loc, 2, values);
    return true;
}

bool Shader::Uniform3f(std::string_view name, float* values) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glUniform3fv(loc, 3, values);
    return true;
}

bool Shader::Uniform4f(std::string_view name, float* values) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glUniform4fv(loc, 4, values);
    return true;
}

bool Shader::UniformMat3(std::string_view name, float* values, bool transpose) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glUniformMatrix3fv(loc, 1, transpose, values);
    return true;
}

bool Shader::UniformMat4(std::string_view name, float* values, bool transpose) {
    UniformLocation loc = UniformLoc(name);
    if (loc == -1) { return false; }
    glUniformMatrix4fv(loc, 1, transpose, values);
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
