#include <Engine/Model.hpp>

#include <memory>
#include <iostream>
#include <algorithm>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include <Engine/Vertex.hpp>
#include <Engine/Buffer.hpp>
#include <Engine/VertexArray.hpp>
#include <Engine/AssimpGLMConvert.hpp>

Model::Model(std::string_view path) noexcept {
    std::cout << "Creating model from disk: " << path << std::endl;

#if defined(_DEBUG)
    Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE, aiDefaultLogStream_STDOUT);
#else
    Assimp::DefaultLogger::create("", Assimp::Logger::NORMAL, aiDefaultLogStream_STDOUT);
#endif
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FlipWindingOrder | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals);
    Assimp::DefaultLogger::kill();

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Couldn't read scene file: " << importer.GetErrorString() << std::endl;
        return;
    }
    this->name = path.substr(path.find_last_of("/\\") + 1);
    this->path = path;

    processNode(avatar, scene->mRootNode, scene);
    meshes = realizeMeshes(avatar);
    CalculateAABB();
}

const std::string& Model::Name() const noexcept { return name; }
const std::string& Model::Path() const noexcept { return path; }

const Avatar& Model::GetAvatar() const noexcept { return avatar; }

void Model::CalculateAABB() const noexcept {
    aabb = meshes[0].AABB;

    for (const auto& mesh : meshes) {
        const auto& bb = mesh.AABB;

        aabb.Min = glm::min(aabb.Min, bb.Min);
        aabb.Max = glm::max(aabb.Max, bb.Max);
    }
}
const AABB& Model::GetAABB() const noexcept { return aabb; }

//-----------------------------------------------------------------
void Model::processNode(struct Avatar& avatar, aiNode* node, const aiScene* scene) noexcept {
    Avatar::Node* parent{ nullptr };
    if (node->mParent) {
        parent = &avatar.Nodes[node->mParent->mName.C_Str()];
    }
    auto [iter, _] = avatar.Nodes.try_emplace(
        std::string(node->mName.C_Str()),
        std::string(node->mName.C_Str()),
        aiMat4ToGLMMat4(node->mTransformation),
        nullptr, //parent,
        0,
        std::array<Avatar::Node*, 128>{}
    );
    Avatar::Node& current = iter->second;
    if (parent) {
        parent->Children[parent->ChildCount++] = &current;
    }

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Avatar::Node::Mesh& processedMesh = processMesh(current, mesh);
        if (mesh->mNumBones == 0) { /* boneless animation */
            int boneId = static_cast<int>(avatar.Bones.size());
            avatar.Bones.try_emplace(
                current.Name,
                boneId,
                current.Name,
                glm::mat4(1)
            );
            for (auto& vertex : processedMesh.Vertices) {
                vertex.BoneIDs[0] = boneId;
                vertex.BoneWeights[0] = 1.0f;
            }
        } else { /* skeletal animation */
            for (unsigned int j = 0; j < mesh->mNumBones; j++) {
                processBone(avatar, processedMesh, mesh->mBones[j]);
            }
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(avatar, node->mChildren[i], scene);
    }

    if (node == scene->mRootNode) {
        avatar.RootNode = &current;
    }
}
Avatar::Node::Mesh& Model::processMesh(Avatar::Node& node, aiMesh* mesh) noexcept {
    auto [iter, _] = node.Meshes.try_emplace(
        mesh->mName.C_Str(),
        mesh->mName.C_Str(),
        std::vector<Vertex>{},
        std::vector<unsigned int>{}
    );
    Avatar::Node::Mesh& current = iter->second;
    current.Vertices.resize(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex& vertex = current.Vertices[i];

        vertex.Position = aiVecToGLMVec(mesh->mVertices[i]);
        vertex.Normal = aiVecToGLMVec(mesh->mNormals[i]);
        if (mesh->mColors[0]) {
            vertex.Color = aiColorToGLMVec(mesh->mColors[0][i]);
        } else {
            vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        }
        if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
            vertex.TexCoords = aiVecToGLMVec(mesh->mTextureCoords[0][i]);
        } else {
            vertex.TexCoords = { 0.0f, 0.0f };
        }

        vertex.BoneIDs.fill(-1);
        vertex.BoneWeights.fill(0.0f);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            current.Indices.emplace_back(face.mIndices[j]);
        }
    }

    return current;
}
void Model::processBone(struct Avatar& avatar, Avatar::Node::Mesh& mesh, aiBone* bone) noexcept {
    auto SetVertexBoneData = [](Vertex& vertex, int boneID, float weight) {
        for (int i = 0; i < Vertex::MAX_BONE_PER_VERTEX; ++i) {
            if (vertex.BoneIDs[i] < 0) {
                vertex.BoneIDs[i] = boneID;
                vertex.BoneWeights[i] = weight;
                break;
            }
        }
    };

    auto [iter, _] = avatar.Bones.try_emplace(
        bone->mName.C_Str(),
        avatar.Bones.size(),
        bone->mName.C_Str(),
        aiMat4ToGLMMat4(bone->mOffsetMatrix)
    );
    Avatar::Bone& current = iter->second;

    auto weights = bone->mWeights;
    auto numWeights = bone->mNumWeights;

    for (unsigned int j = 0; j < numWeights; j++) {
        auto vertexID = weights[j].mVertexId;
        auto weight = weights[j].mWeight;
        assert(vertexID <= mesh.Vertices.size()); /* this also practically should never happen! re-export your model */
        SetVertexBoneData(mesh.Vertices[vertexID], current.Id, weight);
    }
}
std::vector<Model::RenderableMesh> Model::realizeMeshes(const Avatar& avatar) noexcept {
    std::vector<Model::RenderableMesh> rv{};
    for (const auto& [_, node] : avatar.Nodes) {
        for (const auto& [__, mesh] : node.Meshes) {
            rv.emplace_back(&mesh);
        }
    }
    return rv;
}

Model::RenderableMesh::RenderableMesh(const Avatar::Node::Mesh* const mesh) noexcept :
    Mesh(mesh) {
    Buffer vbo(Buffer::Accept(mesh->Vertices));
    VertexAttribLayout layout;
    layout.Define<float>(3); // Position
    layout.Define<float>(3); // Normal
    layout.Define<float>(4); // Color
    layout.Define<float>(2); // UV
    layout.Define<int>(4);   // BoneIDs
    layout.Define<float>(4); // BoneWeights
    VAO.BindVertexAttribBuffer(std::move(vbo), std::move(layout));

    Buffer ebo(Buffer::Accept(mesh->Indices));
    VAO.BindElementBuffer(std::move(ebo));
    // ---------------------------------------------------------------
    AABB.Min = glm::vec3(std::numeric_limits<float>::max());
    AABB.Max = -glm::vec3(std::numeric_limits<float>::max());

    for (const auto& vertex : mesh->Vertices) {
        const auto& position = vertex.Position;

        AABB.Min.x = std::min(AABB.Min.x, position.x);
        AABB.Min.y = std::min(AABB.Min.y, position.y);
        AABB.Min.z = std::min(AABB.Min.z, position.z);

        AABB.Max.x = std::max(AABB.Max.x, position.x);
        AABB.Max.y = std::max(AABB.Max.y, position.y);
        AABB.Max.z = std::max(AABB.Max.z, position.z);
    }
}