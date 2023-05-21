#pragma once

#include <any>
#include <unordered_map>

#include "TypeSystem.hpp"
#include "TypedefsAndConstants.hpp"

struct ShaderAttrib;
struct ShaderUniform;
struct ShaderUniformTable;

struct Shader : std::enable_shared_from_this<Shader> {
    static std::weak_ptr<Shader> CreateFromFile(std::string_view name, const char* vShaderPath, const char* fShaderPath);
    static std::weak_ptr<Shader> CreateFromMemory(std::string_view name, const char* vShaderCode, const char* fShaderCode);
    static std::weak_ptr<Shader> ReloadFromFile(std::weak_ptr<Shader> shader, const char* vShaderPath, const char* fShaderPath);
    static std::weak_ptr<Shader> ReloadFromMemory(std::weak_ptr<Shader> shader, const char* vShaderCode, const char* fShaderCode);
    static std::weak_ptr<Shader> Find(std::string_view name);

    ~Shader() noexcept;

    void Use();

    bool Uniformi(std::string_view name, const int value);
    bool Uniformiv(std::string_view name, const int* values, int size);
    bool Uniform1f(std::string_view name, const float* values, int count = 1);
    bool Uniform2f(std::string_view name, const float* values, int count = 1);
    bool Uniform3f(std::string_view name, const float* values, int count = 1);
    bool Uniform4f(std::string_view name, const  float* values, int count = 1);
    bool UniformMat3(std::string_view name, const float* values, bool transpose = false);
    bool UniformMat4(std::string_view name, const float* values, bool transpose = false);

private:
    inline static std::unordered_map<std::string, std::shared_ptr<Shader>> SHADERS;
    static UniformLocation UniformLoc(Shader& shader, std::string_view name);

    std::string _name;
    ProgramID _glProgramID{ 0 };
    std::unordered_map<std::string, UniformLocation> _uniforms;

    Shader(std::string_view name, ProgramID glProgramID) noexcept;
    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;

    void DeAlloc() noexcept;
};

struct ShaderUniformTable {
    void Set1b(std::string_view name, bool value);
    void Set1bv(std::string_view name, bool* values, size_t count = 1);
    void Set2b(std::string_view name, bool value1, bool value2);
    void Set2bv(std::string_view name, bool* values, size_t count = 1);
    void Set3b(std::string_view name, bool value1, bool value2, bool value3);
    void Set3bv(std::string_view name, bool* values, size_t count = 1);
    void Set4b(std::string_view name, bool value1, bool value2, bool value3, bool value4);
    void Set4bv(std::string_view name, bool* values, size_t count = 1);
    void Set1i(std::string_view name, int value);
    void Set1iv(std::string_view name, int* values, size_t count = 1);
    void Set2i(std::string_view name, int value1, int value2);
    void Set2iv(std::string_view name, int* values, size_t count = 1);
    void Set3i(std::string_view name, int value1, int value2, int value3);
    void Set3iv(std::string_view name, int* values, size_t count = 1);
    void Set4i(std::string_view name, int value1, int value2, int value3, int value4);
    void Set4iv(std::string_view name, int* values, size_t count = 1);
    void Set1f(std::string_view name, float value);
    void Set1fv(std::string_view name, float* values, size_t count = 1);
    void Set2f(std::string_view name, float value1, float value2);
    void Set2fv(std::string_view name, float* values, size_t count = 1);
    void Set3f(std::string_view name, float value1, float value2, float value3);
    void Set3fv(std::string_view name, float* values, size_t count = 1);
    void Set4f(std::string_view name, float value1, float value2, float value3, float value4);
    void Set4fv(std::string_view name, float* values, size_t count = 1);
    void SetMat3(std::string_view name, float* values, bool transpose = false);
    void SetMat3v(std::string_view name, float* values, bool transpose = false, size_t count = 1);
    void SetMat4(std::string_view name, float* values, bool transpose = false);
    void SetMat4v(std::string_view name, float* values, bool transpose = false, size_t count = 1);

private:
    entt::dense_map<std::string, ShaderUniform> uniforms;
};

struct ShaderUniform {
    std::any data;
    Type type;
    struct AuxData {
        bool dynAlloc; // true if pointer type, false if vector type
        size_t stride; // number of components per element
        size_t count; // total number of elements
        bool transpose; // only applicable to matrix types
    } auxData;

    ~ShaderUniform() noexcept;

    std::tuple<bool, AuxData> IsBool() const;
    std::tuple<bool, AuxData> IsBoolArray() const;
    std::tuple<bool, AuxData> IsInt() const;
    std::tuple<bool, AuxData> IsIntArray() const;
    std::tuple<bool, AuxData> IsFloat() const;
    std::tuple<bool, AuxData> IsFloatArray() const;
    std::tuple<bool, AuxData> IsVec2() const;
    std::tuple<bool, AuxData> IsVec2Array() const;
    std::tuple<bool, AuxData> IsVec3() const;
    std::tuple<bool, AuxData> IsVec3Array() const;
    std::tuple<bool, AuxData> IsVec4() const;
    std::tuple<bool, AuxData> IsVec4Array() const;
    std::tuple<bool, AuxData> IsMat3() const;
    std::tuple<bool, AuxData> IsMat3Array() const;
    std::tuple<bool, AuxData> IsMat4() const;
    std::tuple<bool, AuxData> IsMat4Array() const;
};