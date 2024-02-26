#include <Engine/Material.hpp>

#include <Engine/Assets.hpp>
#include <Engine/MaterialSerializer.hpp>
#include <Engine/MaterialDeserializer.hpp>

#pragma region Preprocessor Defines
#ifndef __NEODOA_MATERIAL_UNIFORMS_SET_IMPL
#define __NEODOA_MATERIAL_UNIFORMS_SET_IMPL \
    EnsureCapacity(location + 1); \
    values[location] = ValueType{ UniformValue{ \
        .Location = location, \
        .Value = value \
    } };
#endif // __NEODOA_MATERIAL_UNIFORMS_SET_IMPL

#ifndef __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL
#define __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL \
    EnsureCapacity(location + 1); \
    this->values[location] = ValueType{ UniformValueVector{ \
        .Location = location, \
        .Values = std::vector<decltype(values)::element_type>(values.begin(), values.end()) \
    } };
#endif // __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL
#pragma endregion

void Material::Uniforms::Clear() noexcept { values.clear(); }

const UniformValue& Material::Uniforms::Get(int location) const noexcept {
    assert(location > 0 && location < values.size());
    assert(std::holds_alternative<UniformValue>(values[location]));
    return std::get<UniformValue>(values[location]);
}
const UniformValueVector& Material::Uniforms::Getv(int location) const noexcept {
    assert(location > 0 && location < values.size());
    assert(std::holds_alternative<UniformValueVector>(values[location]));
    return std::get<UniformValueVector>(values[location]);
}
const Material::UniformValues& Material::Uniforms::GetAll() const noexcept { return values; }

void Material::Uniforms::Set(int location, Uniform1f value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, Uniform2f value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, Uniform3f value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, Uniform4f value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }

void Material::Uniforms::Set(int location, Uniform1i value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, Uniform2i value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, Uniform3i value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, Uniform4i value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }

void Material::Uniforms::Set(int location, Uniform1ui value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, Uniform2ui value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, Uniform3ui value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, Uniform4ui value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }

void Material::Uniforms::Setv(int location, const std::span<Uniform1f> values) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<Uniform2f> values) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<Uniform3f> values) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<Uniform4f> values) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }

void Material::Uniforms::Setv(int location, const std::span<Uniform1i> values) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<Uniform2i> values) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<Uniform3i> values) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<Uniform4i> values) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }

void Material::Uniforms::Setv(int location, const std::span<Uniform1ui> values) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<Uniform2ui> values) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<Uniform3ui> values) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<Uniform4ui> values) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }

void Material::Uniforms::Setv(int location, const std::span<UniformMatrix2f> values, bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<UniformMatrix3f> values, bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<UniformMatrix4f> values, bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<UniformMatrix2x3f> values, bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<UniformMatrix3x2f> values, bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<UniformMatrix2x4f> values, bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<UniformMatrix4x2f> values, bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<UniformMatrix3x4f> values, bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }
void Material::Uniforms::Setv(int location, const std::span<UniformMatrix4x3f> values, bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL }

void Material::Uniforms::EnsureCapacity(const size_t cap) {
    if (values.size() <= cap) {
        values.resize(cap);
    }
}

void Material::ClearAllUniforms() noexcept {
    VertexUniforms.Clear();
    TessellationControlUniforms.Clear();
    TessellationEvaluationUniforms.Clear();
    GeometryUniforms.Clear();
    FragmentUniforms.Clear();
}

std::string Material::Serialize() const {
    return SerializeMaterial(*this);
}
Material Material::Deserialize(const std::string_view data) {
    return DeserializeMaterial(data).deserializedMaterial;
}

Material Material::Copy(const Material& material) {
    return material.Deserialize(material.Serialize());
}