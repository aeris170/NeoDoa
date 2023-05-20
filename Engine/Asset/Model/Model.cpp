#include "Model.hpp"

#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Log.hpp"
#include "Texture.hpp"

static std::unordered_map<std::string, std::shared_ptr<Model>> MODELS;

Model::Model(std::string_view name, std::vector<Mesh>&& meshes) noexcept :
    _name(name),
    _meshes(std::move(meshes)) {}

Model::~Model() noexcept {}

static std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string_view typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        auto t = Texture::CreateTexture(str.C_Str(), std::string("Images/").append(str.C_Str()).c_str());
        if (t != Texture::Empty()) { textures.emplace_back(std::move(t)); }
    }
    return textures;
}

static Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vs;
    std::vector<GLuint> is;
    std::vector<Texture> ts;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;
        v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        if (mesh->mColors[0]) {
            v.color = { mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a };
        }
        if (mesh->mTextureCoords[0]) {
            v.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        vs.push_back(v);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            is.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        /*
        ts.insert(ts.end(), diffuseMaps.begin(), diffuseMaps.end());
        if (diffuseMaps.size() > 0) {
            for (auto& v : vs) {
                v.texIndex = 0; // mesh->mMaterialIndex;
            }
        }*/

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        //ts.insert(ts.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(std::move(vs), std::move(is));
}

static std::vector<Mesh> processNode(aiNode* node, const aiScene* scene) {
    std::vector<Mesh> meshes;
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(std::move(processMesh(mesh, scene)));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        auto rv{ processNode(node->mChildren[i], scene) };
        meshes.insert(meshes.end(), std::make_move_iterator(rv.begin()), std::make_move_iterator(rv.end()));
    }
    return meshes;
}

std::weak_ptr<Model> CreateModelFromMesh(std::string_view name, std::vector<Mesh>&& meshes) {
    auto rv = std::make_shared<Model>(name, std::move(meshes));
    MODELS.emplace(name, rv);
    return rv;
}

std::optional<std::weak_ptr<Model>> CreateModelFromFileSystem(std::string_view modelPath) {
    Assimp::Importer importer;
    auto steps = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_PreTransformVertices;
    auto* scene = importer.ReadFile(modelPath.data(), steps);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        DOA_LOG_ERROR("Cannot load mesh, returned nullptr! %s", importer.GetErrorString());
        return {};
    }
    std::string name(modelPath);
    name = name.substr(name.find_last_of("/\\") + 1);
    auto rv = std::make_shared<Model>(name, processNode(scene->mRootNode, scene)); //std::move(processNode) prevents copy elision here...
    MODELS.emplace(name, rv);

    return rv;
}

std::optional<std::weak_ptr<Model>> FindModel(std::string_view name) {
    auto it = MODELS.find(name.data());
    if (it == MODELS.end()) {
        DOA_LOG_WARNING("There is no Model named %s!", name);
        return {};
    }
    return it->second;
}
