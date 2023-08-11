#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <string_view>
#include <unordered_map>

#include <assimp/scene.h>

#include <Engine/AABB.hpp>
#include <Engine/Avatar.hpp>
#include <Engine/VertexArray.hpp>

struct Model {
    struct RenderableMesh {
        VertexArray VAO{};
        const Avatar::Node::Mesh* const Mesh;
        struct AABB AABB;

        explicit RenderableMesh(const Avatar::Node::Mesh* const mesh) noexcept;
    };

    Model(std::string_view path) noexcept;
    ~Model() noexcept = default;
    Model(const Model& other) noexcept = default;
    Model(Model&& other) noexcept = default;
    Model& operator=(const Model& other) noexcept = default;
    Model& operator=(Model&& other) noexcept = default;

    const std::string& Name() const noexcept;
    const std::string& Path() const noexcept;

    const Avatar& GetAvatar() const noexcept;

    void CalculateAABB() const noexcept;
    const AABB& GetAABB() const noexcept;

private:

    std::string name{};
    std::string path{};

    Avatar avatar{};
    std::vector<RenderableMesh> meshes{};
    mutable AABB aabb{};

    static void processMaterials(Avatar& avatar, const aiScene* scene) noexcept;
    static void processNode(Avatar& avatar, aiNode* node, const aiScene* scene) noexcept;
    static Avatar::Node::Mesh& processMesh(Avatar::Node& node, aiMesh* mesh) noexcept;
    static void processBone(Avatar& avatar, Avatar::Node::Mesh& mesh, aiBone* bone) noexcept;
    static std::vector<RenderableMesh> realizeMeshes(const Avatar& avatar) noexcept;

private:
    using itr = decltype(meshes)::iterator;
    using const_itr = decltype(meshes)::const_iterator;
public:
    inline itr begin()              { return meshes.begin();  }
    inline itr end()                { return meshes.end();    }
    inline const_itr begin()  const { return meshes.begin();  }
    inline const_itr end()    const { return meshes.end();    }
    inline const_itr cbegin() const { return meshes.cbegin(); }
    inline const_itr cend()   const { return meshes.cend();   }
};
