#include <Engine/Material.hpp>

#include <Engine/Log.hpp>
#include <Engine/Assets.hpp>
#include <Engine/MaterialSerializer.hpp>
#include <Engine/MaterialDeserializer.hpp>

#pragma region Preprocessor Defines
#ifndef __NEODOA_MATERIAL_UNIFORMS_SET_IMPL
#define __NEODOA_MATERIAL_UNIFORMS_SET_IMPL \
    if (location < 0) { \
        DOA_LOG_WARNING("Cannot set uniform at location %d", location); \
        return; \
    } \
    EnsureCapacity(location + 1); \
    this->values[location] = UniformValue{ \
        .Location = location, \
        .Name = std::string(name), \
        .Value = value \
    };
#endif // __NEODOA_MATERIAL_UNIFORMS_SETV_IMPL
#pragma endregion

void Material::Uniforms::Clear() noexcept { values.clear(); }

const UniformValue& Material::Uniforms::Get(int location) const noexcept {
    assert(location >= 0 && location < values.size());
    return values[location];
}
const UniformValue* Material::Uniforms::TryGet(int location) const noexcept {
    if (location < 0 || location >= values.size()) { return nullptr; }
    return &values[location];
}
const Material::UniformValues& Material::Uniforms::GetAll() const noexcept { return values; }

void Material::Uniforms::Set(int location, std::string_view name, Uniform1f value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, Uniform2f value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, Uniform3f value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, Uniform4f value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }

void Material::Uniforms::Set(int location, std::string_view name, Uniform1i value) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, Uniform2i value) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, Uniform3i value) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, Uniform4i value) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }

void Material::Uniforms::Set(int location, std::string_view name, Uniform1ui value) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, Uniform2ui value) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, Uniform3ui value) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, Uniform4ui value) noexcept{ __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }

void Material::Uniforms::Set(int location, std::string_view name, UniformMatrix2f value, [[maybe_unused]] bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, UniformMatrix3f value, [[maybe_unused]] bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, UniformMatrix4f value, [[maybe_unused]] bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, UniformMatrix2x3f value, [[maybe_unused]] bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, UniformMatrix3x2f value, [[maybe_unused]] bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, UniformMatrix2x4f value, [[maybe_unused]] bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, UniformMatrix4x2f value, [[maybe_unused]] bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, UniformMatrix3x4f value, [[maybe_unused]] bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }
void Material::Uniforms::Set(int location, std::string_view name, UniformMatrix4x3f value, [[maybe_unused]] bool transpose) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }

void Material::Uniforms::Set(int location, std::string_view name, UniformSampler2D value) noexcept { __NEODOA_MATERIAL_UNIFORMS_SET_IMPL }

void Material::Uniforms::EnsureCapacity(const size_t cap) noexcept {
    if (values.size() <= cap) {
        values.resize(cap);
    }
}

bool Material::HasShaderProgram() const noexcept { return ShaderProgram != UUID::Empty(); }

void Material::ClearAllUniforms() noexcept {
    VertexUniforms.Clear();
    TessellationControlUniforms.Clear();
    TessellationEvaluationUniforms.Clear();
    GeometryUniforms.Clear();
    FragmentUniforms.Clear();
}

std::string Material::Serialize() const noexcept {
    return SerializeMaterial(*this);
}
Material Material::Deserialize(const std::string_view data) noexcept {
    return DeserializeMaterial(data).deserializedMaterial;
}

Material Material::Copy(const Material& material) noexcept {
    return material.Deserialize(material.Serialize());
}