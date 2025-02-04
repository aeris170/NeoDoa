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
    using IndexList = std::vector<uint64_t>;

    std::string Name{};
    VertexList Vertices{};
    IndexList Indices{};

    std::string Serialize() const noexcept;
    static Mesh Deserialize(const std::string_view data) noexcept;

    static Mesh Copy(const Mesh& mesh) noexcept;
};