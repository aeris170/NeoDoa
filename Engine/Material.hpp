#pragma once

#include <span>
#include <array>
#include <string>
#include <variant>
#include <type_traits>

#include <Engine/UUID.hpp>
#include <Engine/Shader.hpp>

struct AssetHandle;

using Uniform1f = std::array<float, 1>;
using Uniform2f = std::array<float, 2>;
using Uniform3f = std::array<float, 3>;
using Uniform4f = std::array<float, 4>;

using Uniform1i = std::array<int, 1>;
using Uniform2i = std::array<int, 2>;
using Uniform3i = std::array<int, 3>;
using Uniform4i = std::array<int, 4>;

using Uniform1ui = std::array<unsigned, 1>;
using Uniform2ui = std::array<unsigned, 2>;
using Uniform3ui = std::array<unsigned, 3>;
using Uniform4ui = std::array<unsigned, 4>;

using UniformMatrix2f = std::array<Uniform2f, 2>;
using UniformMatrix3f = std::array<Uniform3f, 3>;
using UniformMatrix4f = std::array<Uniform4f, 4>;
using UniformMatrix2x3f = std::array<Uniform2f, 3>;
using UniformMatrix3x2f = std::array<Uniform3f, 2>;
using UniformMatrix2x4f = std::array<Uniform2f, 4>;
using UniformMatrix4x2f = std::array<Uniform4f, 2>;
using UniformMatrix3x4f = std::array<Uniform3f, 4>;
using UniformMatrix4x3f = std::array<Uniform4f, 3>;

struct UniformValue {
    static constexpr int InvalidLocation{ -1 };

    int Location{ InvalidLocation };
    std::variant<
        Uniform1f,  Uniform2f,  Uniform3f,  Uniform4f,
        Uniform1i,  Uniform2i,  Uniform3i,  Uniform4i,
        Uniform1ui, Uniform2ui, Uniform3ui, Uniform4ui
    > Value;
};

struct UniformValueVector {
    static constexpr int InvalidLocation{ -1 };

    int Location{ InvalidLocation };
    std::variant<
        std::vector<Uniform1f>, std::vector<Uniform2f>, std::vector<Uniform3f>, std::vector<Uniform4f>,
        std::vector<Uniform1i>, std::vector<Uniform2i>, std::vector<Uniform3i>, std::vector<Uniform4i>,
        std::vector<Uniform1ui>, std::vector<Uniform2ui>, std::vector<Uniform3ui>, std::vector<Uniform4ui>,
        std::vector<UniformMatrix2f>, std::vector<UniformMatrix3f>, std::vector<UniformMatrix4f>,
        std::vector<UniformMatrix2x3f>, std::vector<UniformMatrix3x2f>,
        std::vector<UniformMatrix2x4f>, std::vector<UniformMatrix4x2f>,
        std::vector<UniformMatrix3x4f>, std::vector<UniformMatrix4x3f>
    > Values;
};

struct Material {

    using ValueType = std::variant<UniformValue, UniformValueVector>;
    using UniformValues = std::vector<ValueType>;

    struct Uniforms {
        void Clear() noexcept;

        const UniformValue& Get(int location) const noexcept;
        const UniformValueVector& Getv(int location) const noexcept;
        const UniformValues& GetAll() const noexcept;

        void Set(int location, Uniform1f value) noexcept;
        void Set(int location, Uniform2f value) noexcept;
        void Set(int location, Uniform3f value) noexcept;
        void Set(int location, Uniform4f value) noexcept;

        void Set(int location, Uniform1i value) noexcept;
        void Set(int location, Uniform2i value) noexcept;
        void Set(int location, Uniform3i value) noexcept;
        void Set(int location, Uniform4i value) noexcept;

        void Set(int location, Uniform1ui value) noexcept;
        void Set(int location, Uniform2ui value) noexcept;
        void Set(int location, Uniform3ui value) noexcept;
        void Set(int location, Uniform4ui value) noexcept;

        void Setv(int location, const std::span<Uniform1f> values) noexcept;
        void Setv(int location, const std::span<Uniform2f> values) noexcept;
        void Setv(int location, const std::span<Uniform3f> values) noexcept;
        void Setv(int location, const std::span<Uniform4f> values) noexcept;

        void Setv(int location, const std::span<Uniform1i> values) noexcept;
        void Setv(int location, const std::span<Uniform2i> values) noexcept;
        void Setv(int location, const std::span<Uniform3i> values) noexcept;
        void Setv(int location, const std::span<Uniform4i> values) noexcept;

        void Setv(int location, const std::span<Uniform1ui> values) noexcept;
        void Setv(int location, const std::span<Uniform2ui> values) noexcept;
        void Setv(int location, const std::span<Uniform3ui> values) noexcept;
        void Setv(int location, const std::span<Uniform4ui> values) noexcept;

        //TODO implement GPU transpose
        void Setv(int location, const std::span<UniformMatrix2f> values, bool transpose = false) noexcept;
        void Setv(int location, const std::span<UniformMatrix3f> values, bool transpose = false) noexcept;
        void Setv(int location, const std::span<UniformMatrix4f> values, bool transpose = false) noexcept;
        void Setv(int location, const std::span<UniformMatrix2x3f> values, bool transpose = false) noexcept;
        void Setv(int location, const std::span<UniformMatrix3x2f> values, bool transpose = false) noexcept;
        void Setv(int location, const std::span<UniformMatrix2x4f> values, bool transpose = false) noexcept;
        void Setv(int location, const std::span<UniformMatrix4x2f> values, bool transpose = false) noexcept;
        void Setv(int location, const std::span<UniformMatrix3x4f> values, bool transpose = false) noexcept;
        void Setv(int location, const std::span<UniformMatrix4x3f> values, bool transpose = false) noexcept;

    private:
        UniformValues values{};

        void EnsureCapacity(const size_t cap);

        friend struct Material;
    };

    std::string Name;
    UUID ShaderProgram{ UUID::Empty() };
    Uniforms VertexUniforms{};
    Uniforms TessellationControlUniforms{};
    Uniforms TessellationEvaluationUniforms{};
    Uniforms GeometryUniforms{};
    Uniforms FragmentUniforms{};

    void ClearAllUniforms() noexcept;

    std::string Serialize() const;
    static Material Deserialize(const std::string_view data);

    static Material Copy(const Material& material);
};