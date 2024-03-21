#pragma once

#include <span>
#include <array>
#include <string>
#include <variant>
#include <string_view>
#include <type_traits>

#include <glm/glm.hpp>

#include <Engine/UUID.hpp>
#include <Engine/Shader.hpp>

struct AssetHandle;

using Uniform1f = glm::vec1;
using Uniform2f = glm::vec2;
using Uniform3f = glm::vec3;
using Uniform4f = glm::vec4;

using Uniform1i = glm::ivec1;
using Uniform2i = glm::ivec2;
using Uniform3i = glm::ivec3;
using Uniform4i = glm::ivec4;

using Uniform1ui = glm::uvec1;
using Uniform2ui = glm::uvec2;
using Uniform3ui = glm::uvec3;
using Uniform4ui = glm::uvec4;

using UniformMatrix2f = glm::mat2;
using UniformMatrix3f = glm::mat3;
using UniformMatrix4f = glm::mat4;
using UniformMatrix2x3f = glm::mat2x3;
using UniformMatrix3x2f = glm::mat3x2;
using UniformMatrix2x4f = glm::mat2x4;
using UniformMatrix4x2f = glm::mat4x2;
using UniformMatrix3x4f = glm::mat3x4;
using UniformMatrix4x3f = glm::mat4x3;

struct UniformValue {
    static constexpr int InvalidLocation{ -1 };

    int Location{ InvalidLocation };
    std::string Name;
    std::variant<
        Uniform1f, Uniform2f, Uniform3f, Uniform4f,
        Uniform1i, Uniform2i, Uniform3i, Uniform4i,
        Uniform1ui, Uniform2ui, Uniform3ui, Uniform4ui,
        UniformMatrix2f, UniformMatrix3f, UniformMatrix4f,
        UniformMatrix2x3f, UniformMatrix3x2f,
        UniformMatrix2x4f, UniformMatrix4x2f,
        UniformMatrix3x4f, UniformMatrix4x3f
    > Value;
};

struct Material {

    using UniformValues = std::vector<UniformValue>;

    struct Uniforms {
        void Clear() noexcept;

        const UniformValue& Get(int location) const noexcept;
        const UniformValue* TryGet(int location) const noexcept;
        const UniformValues& GetAll() const noexcept;

        void Set(int location, std::string_view name, Uniform1f value) noexcept;
        void Set(int location, std::string_view name, Uniform2f value) noexcept;
        void Set(int location, std::string_view name, Uniform3f value) noexcept;
        void Set(int location, std::string_view name, Uniform4f value) noexcept;

        void Set(int location, std::string_view name, Uniform1i value) noexcept;
        void Set(int location, std::string_view name, Uniform2i value) noexcept;
        void Set(int location, std::string_view name, Uniform3i value) noexcept;
        void Set(int location, std::string_view name, Uniform4i value) noexcept;

        void Set(int location, std::string_view name, Uniform1ui value) noexcept;
        void Set(int location, std::string_view name, Uniform2ui value) noexcept;
        void Set(int location, std::string_view name, Uniform3ui value) noexcept;
        void Set(int location, std::string_view name, Uniform4ui value) noexcept;

        //TODO implement GPU transpose
        void Set(int location, std::string_view name, UniformMatrix2f value, bool transpose = false) noexcept;
        void Set(int location, std::string_view name, UniformMatrix3f value, bool transpose = false) noexcept;
        void Set(int location, std::string_view name, UniformMatrix4f value, bool transpose = false) noexcept;
        void Set(int location, std::string_view name, UniformMatrix2x3f value, bool transpose = false) noexcept;
        void Set(int location, std::string_view name, UniformMatrix3x2f value, bool transpose = false) noexcept;
        void Set(int location, std::string_view name, UniformMatrix2x4f value, bool transpose = false) noexcept;
        void Set(int location, std::string_view name, UniformMatrix4x2f value, bool transpose = false) noexcept;
        void Set(int location, std::string_view name, UniformMatrix3x4f value, bool transpose = false) noexcept;
        void Set(int location, std::string_view name, UniformMatrix4x3f value, bool transpose = false) noexcept;

    private:
        UniformValues values{};

        void EnsureCapacity(const size_t cap) noexcept;

        friend struct Material;
    };

    std::string Name;
    UUID ShaderProgram{ UUID::Empty() };
    Uniforms VertexUniforms{};
    Uniforms TessellationControlUniforms{};
    Uniforms TessellationEvaluationUniforms{};
    Uniforms GeometryUniforms{};
    Uniforms FragmentUniforms{};

    bool HasShaderProgram() const noexcept;

    void ClearAllUniforms() noexcept;

    std::string Serialize() const noexcept;
    static Material Deserialize(const std::string_view data) noexcept;

    static Material Copy(const Material& material) noexcept;
};