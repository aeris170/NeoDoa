#pragma once

#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Utility/string_map.hpp>

#include <Engine/Vertex.hpp>

struct Avatar {
    struct Node {
        struct Mesh {
            std::string Name{};

            std::vector<Vertex> Vertices{};
            std::vector<unsigned int> Indices{};
        };

        std::string Name{};
        glm::mat4 LocalTransformation{ glm::mat4(1) };
        Node* Parent{ nullptr };

        unsigned ChildCount{};
        std::array<Node*, 128> Children{};

        unordered_string_map<Mesh> Meshes{};
    };
    struct Bone {
        int Id{};
        std::string Name{};
        glm::mat4 offsetTransform{ glm::mat4(1) };
    };

    Node* RootNode{ nullptr };
    unordered_string_map<Node> Nodes{};
    unordered_string_map<Bone> Bones{};
};