#include "Shader.hpp"

#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Log.hpp"
#include "Texture.hpp"

std::weak_ptr<Shader> Shader::CreateFromFile(std::string_view name, const char* vShaderPath, const char* fShaderPath) {
    std::string vShaderCode, fShaderCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vShaderPath);
        fShaderFile.open(fShaderPath);

        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vShaderCode = vShaderStream.str();
        fShaderCode = fShaderStream.str();
    } catch (std::ifstream::failure ex) {
        DOA_LOG_ERROR("Shader %s coudln't be created. One or both of the source files couldn't be read!", name);
        return std::weak_ptr<Shader>();
    }

    return CreateFromMemory(name, vShaderCode.c_str(), fShaderCode.c_str());
}
std::weak_ptr<Shader> Shader::CreateFromMemory(std::string_view name, const char* vShaderCode, const char* fShaderCode) {
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
        return std::weak_ptr<Shader>();
    }

    glCompileShader(f);
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(v, 512, NULL, infoLog);
        DOA_LOG_ERROR(infoLog);
        return std::weak_ptr<Shader>();
    }

    glAttachShader(ID, v);
    glAttachShader(ID, f);

    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        DOA_LOG_ERROR(infoLog);
        return std::weak_ptr<Shader>();
    }

    glDetachShader(ID, v);
    glDetachShader(ID, f);

    glDeleteShader(v);
    glDeleteShader(f);

    auto rv = std::shared_ptr<Shader>(new Shader(name, ID));
    SHADERS.emplace(name, rv);
    return rv;
}
std::weak_ptr<Shader> Shader::ReloadFromFile(std::weak_ptr<Shader> shader, const char* vShaderPath, const char* fShaderPath) {
    std::string vShaderCode, fShaderCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vShaderPath);
        fShaderFile.open(fShaderPath);

        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vShaderCode = vShaderStream.str();
        fShaderCode = fShaderStream.str();
    } catch (std::ifstream::failure ex) {
        DOA_LOG_ERROR("Shader %s couldn't be reloaded. One or both of the source files couldn't be read!", shader.lock()->_name);
        return std::weak_ptr<Shader>();
    }

    return ReloadFromMemory(shader, vShaderCode.c_str(), fShaderCode.c_str());
}
std::weak_ptr<Shader> Shader::ReloadFromMemory(std::weak_ptr<Shader> shader, const char* vShaderCode, const char* fShaderCode) {
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
        return shader;
    }

    glCompileShader(f);
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(v, 512, NULL, infoLog);
        DOA_LOG_ERROR(infoLog);
        return shader;
    }

    glAttachShader(ID, v);
    glAttachShader(ID, f);

    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        DOA_LOG_ERROR(infoLog);
        return shader;
    }

    glDetachShader(ID, v);
    glDetachShader(ID, f);

    glDeleteShader(v);
    glDeleteShader(f);

    auto sha = shader.lock();
    sha->DeAlloc();
    sha->_glProgramID = ID;
    return sha;
}

std::weak_ptr<Shader> Shader::Find(std::string_view name) {
    auto it = SHADERS.find(name.data());
    if (it == SHADERS.end()) {
        DOA_LOG_WARNING("Shader::Find failed. There is no Shader named %s!", name);
        return std::weak_ptr<Shader>();
    }
    return it->second;
}

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

Shader::~Shader() { DeAlloc(); }

void Shader::Use() { glUseProgram(_glProgramID); }

bool Shader::Uniformi(std::string_view name, const int value) {
    UniformLocation loc = UniformLoc(*this, name);
    if (loc == -1) { return false; }
    glProgramUniform1i(_glProgramID, loc, value);
    return true;
}

bool Shader::Uniformiv(std::string_view name, const int* values, int size) {
    UniformLocation loc = UniformLoc(*this, name);
    if (loc == -1) { return false; }
    glProgramUniform1iv(_glProgramID, loc, size, values);
    return true;
}

bool Shader::Uniform1f(std::string_view name, const float* values, int count) {
    UniformLocation loc = UniformLoc(*this, name);
    if (loc == -1) { return false; }
    glProgramUniform1fv(_glProgramID, loc, count, values);
    return true;
}

bool Shader::Uniform2f(std::string_view name, const float* values, int count) {
    UniformLocation loc = UniformLoc(*this, name);
    if (loc == -1) { return false; }
    glProgramUniform2fv(_glProgramID, loc, count, values);
    return true;
}

bool Shader::Uniform3f(std::string_view name, const float* values, int count) {
    UniformLocation loc = UniformLoc(*this, name);
    if (loc == -1) { return false; }
    glProgramUniform3fv(_glProgramID, loc, count, values);
    return true;
}

bool Shader::Uniform4f(std::string_view name, const float* values, int count) {
    UniformLocation loc = UniformLoc(*this, name);
    if (loc == -1) { return false; }
    glProgramUniform4fv(_glProgramID, loc, count, values);
    return true;
}

bool Shader::UniformMat3(std::string_view name, const float* values, bool transpose) {
    UniformLocation loc = UniformLoc(*this, name);
    if (loc == -1) { return false; }
    glProgramUniformMatrix3fv(_glProgramID, loc, 1, transpose, values);
    return true;
}

bool Shader::UniformMat4(std::string_view name, const float* values, bool transpose) {
    UniformLocation loc = UniformLoc(*this, name);
    if (loc == -1) { return false; }
    glProgramUniformMatrix4fv(_glProgramID, loc, 1, transpose, values);
    return true;
}

UniformLocation Shader::UniformLoc(Shader& shader, std::string_view name) {
    if (shader._uniforms.find(name.data()) != shader._uniforms.end()) {
        return shader._uniforms[name.data()];
    }
    return shader._uniforms[name.data()] = glGetUniformLocation(shader._glProgramID, name.data());
}

void Shader::DeAlloc() noexcept { glDeleteProgram(_glProgramID); }

void ShaderUniformTable::Set1b(std::string_view name, bool value) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<bool, 1>{ value },
            GetType<bool>(),
            { false, 1, 1, false }
        }
    );
}
void ShaderUniformTable::Set1bv(std::string_view name, bool* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 1, new bool[count * 1]),
            GetType<bool*>(),
            { true, 1, count, false }
        }
    );
}
void ShaderUniformTable::Set2b(std::string_view name, bool value1, bool value2) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<bool, 2>{ value1, value2 },
            GetType<bool>(),
            { false, 2, 1, false }
        }
    );
}
void ShaderUniformTable::Set2bv(std::string_view name, bool* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 2, new bool[count * 2]),
            GetType<bool*>(),
            { true, 2, count, false }
        }
    );
}
void ShaderUniformTable::Set3b(std::string_view name, bool value1, bool value2, bool value3) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<bool, 3>{ value1, value2, value3 },
            GetType<bool>(),
            { false, 3, 1, false }
        }
    );
}
void ShaderUniformTable::Set3bv(std::string_view name, bool* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 3, new bool[count * 3]),
            GetType<bool*>(),
            { true, 3, count, false }
        }
    );
}
void ShaderUniformTable::Set4b(std::string_view name, bool value1, bool value2, bool value3, bool value4) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<bool, 4>{ value1, value2, value3, value4 },
            GetType<bool>(),
            { false, 4, 1, false }
        }
    );
}
void ShaderUniformTable::Set4bv(std::string_view name, bool* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 4, new bool[count * 4]),
            GetType<bool*>(),
            { true, 4, count, false }
        }
    );
}
void ShaderUniformTable::Set1i(std::string_view name, int value) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<int, 1>{ value },
            GetType<int>(),
            { false, 1, 1, false }
        }
    );
}
void ShaderUniformTable::Set1iv(std::string_view name, int* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 1, new int[count * 1]),
            GetType<int*>(),
            { true, 1, count, false }
        }
    );
}
void ShaderUniformTable::Set2i(std::string_view name, int value1, int value2) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<int, 2>{ value1, value2 },
            GetType<int>(),
            { false, 2, 1, false }
        }
    );
}
void ShaderUniformTable::Set2iv(std::string_view name, int* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 2, new int[count * 2]),
            GetType<int*>(),
            { true, 2, count, false }
        }
    );
}
void ShaderUniformTable::Set3i(std::string_view name, int value1, int value2, int value3) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<int, 3>{ value1, value2, value3 },
            GetType<int>(),
            { false, 3, 1, false }
        }
    );
}
void ShaderUniformTable::Set3iv(std::string_view name, int* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 3, new int[count * 3]),
            GetType<int*>(),
            { true, 3, count, false }
        }
    );
}
void ShaderUniformTable::Set4i(std::string_view name, int value1, int value2, int value3, int value4) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<int, 4>{ value1, value2, value3, value4 },
            GetType<int>(),
            { false, 4, 1, false }
        }
    );
}
void ShaderUniformTable::Set4iv(std::string_view name, int* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 4, new int[count * 4]),
            GetType<int*>(),
            { true, 4, count, false }
        }
    );
}
void ShaderUniformTable::Set1f(std::string_view name, float value) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<float, 1>{ value },
            GetType<float>(),
            { false, 1, 1, false }
        }
    );
}
void ShaderUniformTable::Set1fv(std::string_view name, float* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 1, new float[count * 1]),
            GetType<float*>(),
            { true, 1, count, false }
        }
    );
}
void ShaderUniformTable::Set2f(std::string_view name, float value1, float value2) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<float, 2>{ value1, value2 },
            GetType<float>(),
            { false, 2, 1, false }
        }
    );
}
void ShaderUniformTable::Set2fv(std::string_view name, float* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 2, new float[count * 2]),
            GetType<float*>(),
            { true, 2, count, false }
        }
    );
}
void ShaderUniformTable::Set3f(std::string_view name, float value1, float value2, float value3) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<float, 3>{ value1, value2, value3 },
            GetType<float>(),
            { false, 3, 1, false }
        }
    );
}
void ShaderUniformTable::Set3fv(std::string_view name, float* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 3, new float[count * 3]),
            GetType<float*>(),
            { true, 3, count, false }
        }
    );
}
void ShaderUniformTable::Set4f(std::string_view name, float value1, float value2, float value3, float value4) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<float, 4>{ value1, value2, value3, value4 },
            GetType<float>(),
            { false, 4, 1, false }
        }
    );
}
void ShaderUniformTable::Set4fv(std::string_view name, float* values, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 4, new float[count * 4]),
            GetType<float*>(),
            { true, 4, count, false }
        }
    );
}
void ShaderUniformTable::SetMat3(std::string_view name, float* values, bool transpose) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<float, 9> {
                values[0], values[1], values[2],
                values[3], values[4], values[5],
                values[6], values[7], values[8],
            },
            GetType<float>(),
            { false, 9, 1, transpose }
        }
    );
}
void ShaderUniformTable::SetMat3v(std::string_view name, float* values, bool transpose, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 9, new float[count * 9]),
            GetType<float*>(),
            { true, 9, count, transpose }
        }
    );
}
void ShaderUniformTable::SetMat4(std::string_view name, float* values, bool transpose) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::array<float, 16> {
                values[0], values[1], values[2], values[3],
                values[4], values[5], values[6], values[7],
                values[8], values[9], values[10], values[11],
                values[12], values[13], values[14], values[15]
            },
            GetType<float>(),
            { false, 16, 1, transpose }
        }
    );
}
void ShaderUniformTable::SetMat4v(std::string_view name, float* values, bool transpose, size_t count) {
    uniforms.emplace(
        std::string(name),
        ShaderUniform{
            std::copy(values, values + count * 16, new float[count * 16]),
            GetType<float*>(),
            { true, 16, count, transpose }
        }
    );
}

ShaderUniform::~ShaderUniform() noexcept {
    if (auxData.dynAlloc) {
        if (std::get<0>(IsBoolArray())) {
            delete[] std::any_cast<bool*>(data);
        } else if (std::get<0>(IsIntArray())) {
            delete[] std::any_cast<int*>(data);
        } else if (std::get<0>(IsFloatArray())) {
            delete[] std::any_cast<float*>(data);
        } else if (std::get<0>(IsVec2Array())) {
            delete[] std::any_cast<glm::vec2*>(data);
        } else if (std::get<0>(IsVec3Array())) {
            delete[] std::any_cast<glm::vec3*>(data);
        } else if (std::get<0>(IsVec4Array())) {
            delete[] std::any_cast<glm::vec4*>(data);
        } else if (std::get<0>(IsMat3Array())) {
            delete[] std::any_cast<glm::mat3*>(data);
        } else if (std::get<0>(IsMat4Array())) {
            delete[] std::any_cast<glm::mat4*>(data);
        }
    }
}

std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsBool()       const { return { type == GetType<bool>()      , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsBoolArray()  const { return { type == GetType<bool*>()     , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsInt()        const { return { type == GetType<int>()       , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsIntArray()   const { return { type == GetType<int*>()      , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsFloat()      const { return { type == GetType<float>()     , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsFloatArray() const { return { type == GetType<float*>()    , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsVec2()       const { return { type == GetType<glm::vec2>() , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsVec2Array()  const { return { type == GetType<glm::vec2*>(), auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsVec3()       const { return { type == GetType<glm::vec3>() , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsVec3Array()  const { return { type == GetType<glm::vec3*>(), auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsVec4()       const { return { type == GetType<glm::vec4>() , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsVec4Array()  const { return { type == GetType<glm::vec4*>(), auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsMat3()       const { return { type == GetType<glm::mat3>() , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsMat3Array()  const { return { type == GetType<glm::mat3*>(), auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsMat4()       const { return { type == GetType<glm::mat4>() , auxData }; }
std::tuple<bool, ShaderUniform::AuxData> ShaderUniform::IsMat4Array()  const { return { type == GetType<glm::mat4*>(), auxData }; }
