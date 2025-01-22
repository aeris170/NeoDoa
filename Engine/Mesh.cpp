#include <Engine/Mesh.hpp>

#include <Engine/MeshSerializer.hpp>
#include <Engine/MeshDeserializer.hpp>

std::string Mesh::Serialize() const noexcept { return SerializeMesh(*this); }
Mesh Mesh::Deserialize(const std::string_view data) noexcept { return DeserializeMesh(data).deserializedMesh; }

Mesh Mesh::Copy(const Mesh& mesh) noexcept { return mesh; }