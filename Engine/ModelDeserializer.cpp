#include <Engine/ModelDeserializer.hpp>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include <Engine/Log.hpp>
#include <Engine/Texture.hpp>
#include <Engine/FileNode.hpp>
#include <Engine/DataTypes.hpp>

static constexpr std::array<aiTextureType, AI_TEXTURE_TYPE_MAX> textureTypes {
            aiTextureType_DIFFUSE,
            aiTextureType_SPECULAR,
            aiTextureType_AMBIENT,
            aiTextureType_EMISSIVE,
            aiTextureType_HEIGHT,
            aiTextureType_NORMALS,
            aiTextureType_SHININESS,
            aiTextureType_OPACITY,
            aiTextureType_DISPLACEMENT,
            aiTextureType_LIGHTMAP,
            aiTextureType_REFLECTION,
            aiTextureType_BASE_COLOR,
            aiTextureType_NORMAL_CAMERA,
            aiTextureType_EMISSION_COLOR,
            aiTextureType_METALNESS,
            aiTextureType_DIFFUSE_ROUGHNESS,
            aiTextureType_AMBIENT_OCCLUSION,
            aiTextureType_UNKNOWN,
            aiTextureType_SHEEN,
            aiTextureType_CLEARCOAT,
            aiTextureType_TRANSMISSION,
            aiTextureType_MAYA_BASE,
            aiTextureType_MAYA_SPECULAR,
            aiTextureType_MAYA_SPECULAR_COLOR,
            aiTextureType_MAYA_SPECULAR_ROUGHNESS
};
// if this assert fails, assimp modified aiTextureType enum.
// modify above array accordingly.
static_assert(textureTypes.back() != 0 && textureTypes.back() == AI_TEXTURE_TYPE_MAX);

// Custom log stream to print to console
class ConsoleLogStream : public Assimp::LogStream {
public:
    void write(const char* message) override {
        DOA_LOG_FATAL("[Assimp] %s", message);
    }
};


Tree<Model::Node> processNodeTree(const aiScene& scene);
std::pair<std::vector<Mesh>, std::vector<size_t>> processMeshes(const aiScene& scene);
std::vector<TextureDeserializationResult> processTextures(const aiScene& scene);
std::vector<Model::Material> processMaterials(const aiScene& scene);

ModelDeserializationResult DeserializeModel(const FNode& file) noexcept {
    file.ReadContent();
    auto rv = DeserializeModel(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedModel.Name = file.Name();
    }
    return rv;
}
ModelDeserializationResult DeserializeModel(const std::string_view data) noexcept {
    ModelDeserializationResult rv;

    auto* a = Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
    Assimp::DefaultLogger::get()->attachStream(new ConsoleLogStream, Assimp::Logger::VERBOSE);

    Assimp::Importer importer;

    auto flags = aiProcess_Triangulate |
        aiProcess_FlipUVs;
    const aiScene* scene = importer.ReadFileFromMemory(data.data(), data.size(), flags, "fbx");
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't deserialize model!");
        rv.errors.emplace_back("This should normally never happen unless model data is ill-formed.");
        rv.errors.emplace_back(importer.GetErrorString());
        DOA_LOG_ERROR("Couldn't deserialize model!\n\n%s", data);
    } else {
        rv.deserializedModel.Nodes = processNodeTree(*scene);
        std::tie(rv.deserializedMeshes, rv.meshMaterialIndices) = processMeshes(*scene);
        rv.deserializedTextures = processTextures(*scene);
        rv.deserializedModel.Materials = processMaterials(*scene);
    }

    Assimp::DefaultLogger::kill();

    return rv;
}

Tree<Model::Node> processNodeTree(const aiScene& scene) {
    static auto ProcessNodeRecursive = [](Tree<Model::Node>& tree, size_t parentIndex, const aiNode& node, auto&& selfReference) -> void {
        std::vector<size_t> meshIndices{};

        meshIndices.reserve(node.mNumMeshes);
        for (size_t i = 0; i < node.mNumMeshes; i++) {
            meshIndices.push_back(node.mMeshes[i]);
        }

        std::string_view nodeName = node.mName.C_Str();
        if (nodeName.empty()) {
            nodeName = "Root";
        }

        size_t selfIndex = tree.EmplaceNode(parentIndex, nodeName.data(), std::move(meshIndices));
        for (size_t i = 0; i < node.mNumChildren; i++) {
            selfReference(tree, selfIndex, *node.mChildren[i], selfReference);
        }
    };

    Tree<Model::Node> rv{};
    for (size_t i = 0; i < scene.mRootNode->mNumChildren; i++) {
        ProcessNodeRecursive(rv, rv.Root, *scene.mRootNode->mChildren[i], ProcessNodeRecursive);
    }
    return rv;
}

std::pair<std::vector<Mesh>, std::vector<size_t>> processMeshes(const aiScene& scene) {
    std::pair<std::vector<Mesh>, std::vector<size_t>> rv{};
    auto& [meshes, materialIndices] = rv;

    meshes.resize(scene.mNumMeshes);
    materialIndices.resize(scene.mNumMeshes);
    for (size_t i = 0; i < scene.mNumMeshes; i++) {
        Mesh& m = meshes[i];
        const aiMesh& mesh = *scene.mMeshes[i];

        m.Vertices.resize(mesh.mNumVertices);
        for (size_t j = 0; j < mesh.mNumVertices; j++) {
            if (mesh.HasPositions()) {
                auto& position = m.Vertices[j].Position;
                position.x = mesh.mVertices[j].x;
                position.y = mesh.mVertices[j].y;
                position.z = mesh.mVertices[j].z;
            }
            if (mesh.HasNormals()) {
                auto& normal = m.Vertices[j].Normal;
                normal.x = mesh.mNormals[j].x;
                normal.y = mesh.mNormals[j].y;
                normal.z = mesh.mNormals[j].z;
            }
            if (mesh.HasTextureCoords(0)) {
                auto& texCoords = m.Vertices[j].TexCoords;
                texCoords.x = mesh.mTextureCoords[0][j].x;
                texCoords.y = mesh.mTextureCoords[0][j].y;
            }
            if (mesh.HasVertexColors(0)) {
                auto& color = m.Vertices[j].Color;
                color.r = mesh.mTextureCoords[0][j].x;
                color.g = mesh.mTextureCoords[0][j].y;
                color.b = mesh.mTextureCoords[0][j].z;
                color.a = 1;
            } else {
                auto& color = m.Vertices[j].Color;
                color = { 1.0f, 1.0f, 1.0f, 1.0f };
            }
        }

        m.Indices.resize(mesh.mNumFaces * 3); // all faces are triangles
        for (size_t j = 0; j < mesh.mNumFaces; j++) {
            aiFace face = mesh.mFaces[j];
            assert(face.mNumIndices == 3);

            const size_t base = j * 3;
            m.Indices[base + 0] = face.mIndices[0];
            m.Indices[base + 1] = face.mIndices[1];
            m.Indices[base + 2] = face.mIndices[2];
        }

        materialIndices[i] = mesh.mMaterialIndex;

        m.CalculateAABBProperties();
    }

    return rv;
}

std::vector<Model::Material> processMaterials(const aiScene& scene) {
    std::vector<Model::Material> rv{};

    rv.resize(scene.mNumMaterials);
    for (size_t i = 0; i < scene.mNumMaterials; i++) {
        Model::Material& m = rv[i];
        aiMaterial& mat = *scene.mMaterials[i];

        {
            aiString matName;
            if (mat.Get(AI_MATKEY_NAME, matName) == aiReturn_SUCCESS && matName.length > 0) {
                m.Name = matName.C_Str();
            }
        }
        {
            aiColor3D diffuseColor;
            if (mat.Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == aiReturn_SUCCESS) {
                m.DiffuseColor = {
                    diffuseColor.r,
                    diffuseColor.g,
                    diffuseColor.b,
                    1.0f
                };
            }
        }
        {
            aiColor3D specularColor;
            if (mat.Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == aiReturn_SUCCESS) {
                m.SpecularColor = {
                    specularColor.r,
                    specularColor.g,
                    specularColor.b,
                    1.0f
                };
            }
        }
        {
            aiColor3D ambientColor;
            if (mat.Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS) {
                m.AmbientColor = {
                    ambientColor.r,
                    ambientColor.g,
                    ambientColor.b,
                    1.0f
                };
            }
        }
        {
            aiColor3D emissiveColor;
            if (mat.Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == aiReturn_SUCCESS) {
                m.EmissiveColor = {
                    emissiveColor.r,
                    emissiveColor.g,
                    emissiveColor.b,
                    1.0f
                };
            }
        }
        {
            aiColor3D transparentColor;
            if (mat.Get(AI_MATKEY_COLOR_TRANSPARENT, transparentColor) == aiReturn_SUCCESS) {
                m.TransparentColor = {
                    transparentColor.r,
                    transparentColor.g,
                    transparentColor.b,
                    1.0f
                };
            }
        }
        {
            aiColor3D reflectiveColor;
            if (mat.Get(AI_MATKEY_COLOR_REFLECTIVE, reflectiveColor) == aiReturn_SUCCESS) {
                m.ReflectiveColor = {
                    reflectiveColor.r,
                    reflectiveColor.g,
                    reflectiveColor.b,
                    1.0f
                };
            }
        }
        {
            float reflectivity;
            if (mat.Get(AI_MATKEY_REFLECTIVITY, reflectivity) == aiReturn_SUCCESS) {
                m.Reflectivity = reflectivity;
            }
        }
        {
            int wireframe;
            if (mat.Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe) == aiReturn_SUCCESS) {
                m.EnableWireframe = wireframe != 0;
            }
        }
        {
            int twoSided;
            if (mat.Get(AI_MATKEY_TWOSIDED, twoSided) == aiReturn_SUCCESS) {
                m.TwoSided = twoSided != 0;
            }
        }
        {
            aiShadingMode shadingModel;
            if (mat.Get(AI_MATKEY_SHADING_MODEL, shadingModel) == aiReturn_SUCCESS) {
                switch(shadingModel) {
                    case aiShadingMode_Flat:
                        m.ShadingMode = Model::Material::ShadingMode::Flat;
                        break;
                    case aiShadingMode_Gouraud:
                        m.ShadingMode = Model::Material::ShadingMode::Gouraud;
                        break;
                    case aiShadingMode_Phong:
                        m.ShadingMode = Model::Material::ShadingMode::Phong;
                        break;
                    case aiShadingMode_Blinn:
                        m.ShadingMode = Model::Material::ShadingMode::Blinn;
                        break;
                    case aiShadingMode_Toon:
                        m.ShadingMode = Model::Material::ShadingMode::Toon;
                        break;
                    case aiShadingMode_OrenNayar:
                        m.ShadingMode = Model::Material::ShadingMode::OrenNayar;
                        break;
                    case aiShadingMode_Minnaert:
                        m.ShadingMode = Model::Material::ShadingMode::Minnaert;
                        break;
                    case aiShadingMode_CookTorrance:
                        m.ShadingMode = Model::Material::ShadingMode::CookTorrance;
                        break;
                    case aiShadingMode_NoShading: // also aiShadingMode_Unlit
                        m.ShadingMode = Model::Material::ShadingMode::Unlit;
                        break;
                    case aiShadingMode_Fresnel:
                        m.ShadingMode = Model::Material::ShadingMode::Fresnel;
                        break;
                    case aiShadingMode_PBR_BRDF:
                        m.ShadingMode = Model::Material::ShadingMode::PBRBRDF;
                        break;
                }
            }
        }
        {
            aiBlendMode blendFunc;
            if (mat.Get(AI_MATKEY_BLEND_FUNC, blendFunc) == aiReturn_SUCCESS) {
                switch (blendFunc) {
                case aiBlendMode_Default:
                    m.BlendMode = Model::Material::BlendMode::Default;
                    break;
                case aiBlendMode_Additive:
                    m.BlendMode = Model::Material::BlendMode::Additive;
                    break;
                }
            }
        }
        {
            float opacity;
            if (mat.Get(AI_MATKEY_OPACITY, opacity) == aiReturn_SUCCESS) {
                m.Opacity = opacity;
            }
        }
        {
            float shininess;
            if (mat.Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
                m.Shininess = shininess;
            }
        }
        {
            float shininessStrength;
            if (mat.Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength) == aiReturn_SUCCESS) {
                m.ShininessStrength = shininessStrength;
            }
        }
        {
            float refracti;
            if (mat.Get(AI_MATKEY_REFRACTI, refracti) == aiReturn_SUCCESS) {
                m.Refracti = refracti;
            }
        }

        for (size_t i = 0; i < textureTypes.size(); i++) {
            Model::Material::TextureStack& stack = m.TextureStacks[i];
            aiTextureType textureType = textureTypes[i];

            // Constrain max number of textures to process to 4 (TextureStacks size)
            size_t textureCount = std::min<size_t>(mat.GetTextureCount(textureType), m.TextureStacks.size());
            stack.TextureCount = textureCount;
            for (size_t j = 0; j < textureCount; j++) {
                Model::Material::TextureInfo& textureInfo = stack.Textures[j];

                {
                    aiString texturePath;
                    if (mat.Get(AI_MATKEY_TEXTURE(textureType, j), texturePath) == aiReturn_SUCCESS) {
                        textureInfo.Path = texturePath.C_Str();
                    }
                }
                {
                    float textureBlend;
                    if (mat.Get(AI_MATKEY_TEXBLEND(textureType, j), textureBlend) == aiReturn_SUCCESS) {
                        textureInfo.BlendAmount = textureBlend;
                    }
                }
                {
                    aiTextureOp textureOp;
                    if (mat.Get(AI_MATKEY_TEXOP(textureType, j), textureOp) == aiReturn_SUCCESS) {
                        switch (textureOp) {
                            case aiTextureOp_Multiply:
                                textureInfo.Operation = Model::Material::TextureOperation::Multiply;
                                break;
                            case aiTextureOp_Add:
                                textureInfo.Operation = Model::Material::TextureOperation::Add;
                                break;
                            case aiTextureOp_Subtract:
                                textureInfo.Operation = Model::Material::TextureOperation::Subtract;
                                break;
                            case aiTextureOp_Divide:
                                textureInfo.Operation = Model::Material::TextureOperation::Divide;
                                break;
                            case aiTextureOp_SmoothAdd:
                                textureInfo.Operation = Model::Material::TextureOperation::SmoothAdd;
                                break;
                            case aiTextureOp_SignedAdd:
                                textureInfo.Operation = Model::Material::TextureOperation::SignedAdd;
                                break;
                        }
                    }
                }
                {
                    aiTextureMapping mapping;
                    if (mat.Get(AI_MATKEY_MAPPING(textureType, j), mapping) == aiReturn_SUCCESS) {
                        switch (mapping) {
                        case aiTextureMapping_UV:
                            textureInfo.Mapping = Model::Material::TextureMapping::UV;
                            break;
                        case aiTextureMapping_SPHERE:
                            textureInfo.Mapping = Model::Material::TextureMapping::Sphere;
                            break;
                        case aiTextureMapping_CYLINDER:
                            textureInfo.Mapping = Model::Material::TextureMapping::Cylinder;
                            break;
                        case aiTextureMapping_BOX:
                            textureInfo.Mapping = Model::Material::TextureMapping::Box;
                            break;
                        case aiTextureMapping_PLANE:
                            textureInfo.Mapping = Model::Material::TextureMapping::Plane;
                            break;
                        case aiTextureMapping_OTHER:
                            textureInfo.Mapping = Model::Material::TextureMapping::Undefined;
                            break;
                        }
                    }
                }
                {
                    int uvwsrc;
                    if (mat.Get(AI_MATKEY_UVWSRC(textureType, j), uvwsrc) == aiReturn_SUCCESS) {
                        textureInfo.UVWSource = uvwsrc;
                    }
                }
                {
                    aiTextureMapMode mappingModeU, mappingModeV;
                    if (mat.Get(AI_MATKEY_MAPPINGMODE_U(textureType, j), mappingModeU) == aiReturn_SUCCESS) {
                        switch (mappingModeU) {
                            case aiTextureMapMode_Wrap:
                                textureInfo.MapModeU = Model::Material::TextureMapMode::Wrap;
                                break;
                            case aiTextureMapMode_Clamp:
                                textureInfo.MapModeU = Model::Material::TextureMapMode::Clamp;
                                break;
                            case aiTextureMapMode_Decal:
                                textureInfo.MapModeU = Model::Material::TextureMapMode::Decal;
                                break;
                            case aiTextureMapMode_Mirror:
                                textureInfo.MapModeU = Model::Material::TextureMapMode::Mirror;
                                break;
                        }
                    }
                    if (mat.Get(AI_MATKEY_MAPPINGMODE_V(textureType, j), mappingModeV) == aiReturn_SUCCESS) {
                        switch (mappingModeV) {
                        case aiTextureMapMode_Wrap:
                            textureInfo.MapModeV = Model::Material::TextureMapMode::Wrap;
                            break;
                        case aiTextureMapMode_Clamp:
                            textureInfo.MapModeV = Model::Material::TextureMapMode::Clamp;
                            break;
                        case aiTextureMapMode_Decal:
                            textureInfo.MapModeV = Model::Material::TextureMapMode::Decal;
                            break;
                        case aiTextureMapMode_Mirror:
                            textureInfo.MapModeV = Model::Material::TextureMapMode::Mirror;
                            break;
                        }
                    }
                }
                {
                    aiVector3D texmapAxis;
                    if (mat.Get(AI_MATKEY_TEXMAP_AXIS(textureType, j), texmapAxis) == aiReturn_SUCCESS) {
                        textureInfo.MappingAxis = {
                            texmapAxis.x,
                            texmapAxis.y,
                            texmapAxis.z
                        };
                    }
                }
                {
                    aiTextureFlags texFlags;
                    if (mat.Get(AI_MATKEY_TEXFLAGS(textureType, j), texFlags) == aiReturn_SUCCESS) {
                        if (static_cast<bool>(texFlags & aiTextureFlags_Invert)) {
                            textureInfo.Flags = textureInfo.Flags | Model::Material::TextureFlags::Invert;
                        }
                        if (static_cast<bool>(texFlags & aiTextureFlags_UseAlpha)) {
                            textureInfo.Flags = textureInfo.Flags | Model::Material::TextureFlags::UseAlpha;
                        }
                        if (static_cast<bool>(texFlags & aiTextureFlags_IgnoreAlpha)) {
                            textureInfo.Flags = textureInfo.Flags | Model::Material::TextureFlags::IgnoreAlpha;
                        }
                    }
                }
            }
        }
    }

    return rv;
}

std::vector<TextureDeserializationResult> processTextures(const aiScene& scene) {
    std::vector<TextureDeserializationResult> rv{};

    unsigned textureCount{};
    for (size_t i = 0; i < scene.mNumMaterials; i++) {
        aiMaterial& mat = *scene.mMaterials[i];
        for (auto textureType : textureTypes) {
            textureCount += mat.GetTextureCount(textureType);
        }
    }

    rv.reserve(textureCount);
    for (size_t i = 0; i < scene.mNumMaterials; i++) {
        aiMaterial& mat = *scene.mMaterials[i];

        for (auto textureType : textureTypes) {
            aiString texturePath;

            for (size_t j = 0; j < mat.GetTextureCount(textureType); j++) {
                TextureDeserializationResult& tdr = rv.emplace_back();

                aiReturn ret = mat.GetTexture(textureType, static_cast<unsigned>(j), &texturePath);
                assert(ret != AI_FAILURE);

                if (const aiTexture* embedded = scene.GetEmbeddedTexture(texturePath.C_Str())) {
                    if (embedded->mHeight == 0) {
                        std::string name = std::format("{} (Embedded)", embedded->mFilename.C_Str());

                        RawData raw;
                        raw.resize(embedded->mWidth);
                        std::memcpy(raw.data(), embedded->pcData, embedded->mWidth);

                        EncodedTextureData data {
                            name,
                            raw
                        };

                        tdr = DeserializeTexture(data);
                    } else {
                        tdr.deserializedTexture.Name = std::format("{} (Embedded)", embedded->mFilename.C_Str());
                        tdr.deserializedTexture.Width = embedded->mWidth;
                        tdr.deserializedTexture.Height = embedded->mHeight;
                        tdr.deserializedTexture.Channels = 4;
                        tdr.deserializedTexture.Format = DataFormat::RGBA8;

                        const size_t pixelCount = static_cast<size_t>(embedded->mWidth * embedded->mHeight);
                        tdr.deserializedTexture.PixelData.resize(pixelCount * 4);
                        for (size_t k = 0; k < pixelCount; k++) {
                            const aiTexel& texel = embedded->pcData[k];
                            const size_t base = k * 4;
                            tdr.deserializedTexture.PixelData[base + 0] = static_cast<std::byte>(texel.r);
                            tdr.deserializedTexture.PixelData[base + 1] = static_cast<std::byte>(texel.g);
                            tdr.deserializedTexture.PixelData[base + 2] = static_cast<std::byte>(texel.b);
                            tdr.deserializedTexture.PixelData[base + 3] = static_cast<std::byte>(texel.a);
                        }
                    }
                } else if(std::filesystem::exists(texturePath.C_Str())) {
                    std::ifstream file(texturePath.C_Str(), std::ios::in | std::ios::binary);
                    assert(file);

                    std::stringstream buffer;
                    buffer << file.rdbuf();

                    std::string content = buffer.str();
                    RawData raw;
                    raw.resize(content.size());
                    for (auto c : content) {
                        raw.push_back(static_cast<std::byte>(c));
                    }

                    EncodedTextureData data{
                        texturePath.C_Str(),
                        raw
                    };

                    tdr = DeserializeTexture(data);
                } else {
                    tdr.erred = true;
                    tdr.errors.emplace_back(std::format("Couldn't deserialize texture at {}. File doesn't exist or you have not permission to read it.", texturePath.C_Str()));
                }

                texturePath.Clear();
            }
        }
    }

    return rv;
}