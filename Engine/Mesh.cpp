#include <Engine/Mesh.hpp>

#include <Engine/MeshSerializer.hpp>
#include <Engine/MeshDeserializer.hpp>

void Mesh::CalculateAABBProperties() noexcept {
    Min = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    Max = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };
    Origin = {};
    Centeroid = {};

    if (Vertices.has_value()) {
        auto&& Vertices = this->Vertices.value();
        for (const auto& vertex : Vertices) {
            Min = glm::min(Min, vertex.Position);
            Max = glm::max(Max, vertex.Position);

            Centeroid += vertex.Position;
        }
        Origin = (Min + Max) / 2.0f;
        Centeroid /= Vertices.size();
    }
}

std::string Mesh::Serialize() const noexcept { return SerializeMesh(*this); }
Mesh Mesh::Deserialize(const std::string_view data) noexcept { return DeserializeMesh(data).deserializedMesh; }

Mesh Mesh::Copy(const Mesh& mesh) noexcept { return mesh; }