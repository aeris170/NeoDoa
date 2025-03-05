#pragma once

#include <string>
#include <vector>
#include <string_view>

#include <glm/glm.hpp>

struct Mesh {
    struct Vertex {

        //static constexpr int MAX_BONE_PER_VERTEX{ 4 };

        glm::vec3 Position{};
        glm::vec3 Normal{};
        glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
        glm::vec2 TexCoords{};

        //std::array<int, MAX_BONE_PER_VERTEX> BoneIDs;
        //std::array<float, MAX_BONE_PER_VERTEX> BoneWeights;
    };
    using VertexList = std::vector<Vertex>;
    using IndexList = std::vector<uint32_t>;

    std::string Name{};
    VertexList Vertices{};
    IndexList Indices{};

    glm::vec3 Min{}; // Element-wise min position. May not correspond to a real vertex in the Mesh.
    glm::vec3 Max{}; // Element-wise max position. May not correspond to a real vertex in the Mesh.
    glm::vec3 Origin{}; // Center of bounding box.
    glm::vec3 Centeroid{}; // Average of all vertex positions.

    // Calculates Min, Max, Origin and Centeroid
    void CalculateAABBProperties() noexcept;

    std::string Serialize() const noexcept;
    static Mesh Deserialize(const std::string_view data) noexcept;

    static Mesh Copy(const Mesh& mesh) noexcept;
};