#include <Engine/Assets.hpp>

#include <format>
#include <string>
#include <utility>
#include <algorithm>

#include <Utility/FormatBytes.hpp>

#include <Engine/Core.hpp>
#include <Engine/ProjectDeserializer.hpp>
#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>
#include <Engine/ComponentDeserializer.hpp>
#include <Engine/SamplerSerializer.hpp>
#include <Engine/SamplerDeserializer.hpp>
#include <Engine/TextureSerializer.hpp>
#include <Engine/TextureDeserializer.hpp>
#include <Engine/ShaderDeserializer.hpp>
#include <Engine/ShaderProgramSerializer.hpp>
#include <Engine/ShaderProgramDeserializer.hpp>
#include <Engine/MaterialSerializer.hpp>
#include <Engine/MaterialDeserializer.hpp>
#include <Engine/FrameBufferSerializer.hpp>
#include <Engine/FrameBufferDeserializer.hpp>
#include <Engine/MeshSerializer.hpp>
#include <Engine/MeshDeserializer.hpp>
#include <Engine/ModelSerializer.hpp>
#include <Engine/ModelDeserializer.hpp>

AssetHandle::AssetHandle() noexcept :
    _asset(nullptr) {}
AssetHandle::AssetHandle(Asset* const asset) noexcept :
    _asset(asset) {}
Asset& AssetHandle::operator*() const noexcept { return *_asset; }
Asset* AssetHandle::operator->() const noexcept { return _asset; }
AssetHandle::operator Asset* () const noexcept { return _asset; }
AssetHandle::operator bool() const noexcept { return HasValue(); }

bool AssetHandle::HasValue() const noexcept { return _asset != nullptr; }
Asset& AssetHandle::Value() const noexcept { return *_asset; }
void AssetHandle::Reset() noexcept { _asset = nullptr; }

Asset& AssetDatabase::operator[](size_t index) noexcept { return assets.find(index)->second; }
const Asset& AssetDatabase::operator[](size_t index) const noexcept { return assets.find(index)->second; }
AssetDatabase::UUIDMap<Asset>::iterator       AssetDatabase::begin()        noexcept { return assets.begin();  }
AssetDatabase::UUIDMap<Asset>::iterator       AssetDatabase::end()          noexcept { return assets.end();    }
AssetDatabase::UUIDMap<Asset>::const_iterator AssetDatabase::cbegin() const noexcept { return assets.cbegin(); }
AssetDatabase::UUIDMap<Asset>::const_iterator AssetDatabase::cend()   const noexcept { return assets.cend();   }
AssetDatabase::UUIDMap<Asset>::const_iterator AssetDatabase::begin()  const noexcept { return assets.begin();  }
AssetDatabase::UUIDMap<Asset>::const_iterator AssetDatabase::end()    const noexcept { return assets.end();    }

Asset& AssetDatabase::Emplace(UUID uuid, FNode* file, Assets& owningManager) noexcept {
    auto& asset = assets.try_emplace(uuid, uuid, owningManager).first->second;
    files.try_emplace(uuid, file);
    data.try_emplace(uuid, Assets::CreateEmptyAssetDataUsingFile(*file));
    versions.try_emplace(uuid);
    infoLists.try_emplace(uuid);
    warningLists.try_emplace(uuid);
    errorLists.try_emplace(uuid);
    return asset;
}
AssetData& AssetDatabase::EmplaceAsSubAsset(UUID uuid, UUID owner, SubAssetTree::NodeIndex ownerIndex) noexcept {
    assert(Contains(owner));
    assets.try_emplace(uuid, uuid, const_cast<Assets&>(assets.at(owner).OwningManager()));
    auto empty = Assets::CreateEmptyAssetDataUsingFile(*files[owner]);
    empty.Type = Assets::GenericAssetTypeName;
    auto& data = this->data.try_emplace(uuid, empty).first->second;
    versions.try_emplace(uuid);
    infoLists.try_emplace(uuid);
    warningLists.try_emplace(uuid);
    errorLists.try_emplace(uuid);

    if (ownerIndex == SubAssetTree::Root) {
        ownerIndex = subAssets.FindNodeIndexDFS(owner);
    }
    subAssets.EmplaceNode(ownerIndex, uuid);

    return data;
}
bool AssetDatabase::Contains(UUID uuid) const noexcept {
    return assets.contains(uuid);
}
Asset& AssetDatabase::At(UUID uuid) noexcept {
    return assets.at(uuid);
}
const Asset& AssetDatabase::At(UUID uuid) const noexcept {
    return assets.at(uuid);
}
void AssetDatabase::Remove(UUID uuid) noexcept {
    assets.erase(uuid);
    files.erase(uuid);
    data.erase(uuid);
    versions.erase(uuid);
    infoLists.erase(uuid);
    warningLists.erase(uuid);
    errorLists.erase(uuid);
}
void AssetDatabase::Clear() noexcept {
    assets.clear();
    files.clear();
    data.clear();
    versions.clear();
    infoLists.clear();
    warningLists.clear();
    errorLists.clear();
}

bool Assets::IsProjectFile(const FNode& file)             noexcept { return file.ext == ProjectExtension;             }
bool Assets::IsSceneFile(const FNode& file)               noexcept { return file.ext == SceneExtension;               }
bool Assets::IsComponentDefinitionFile(const FNode& file) noexcept { return file.ext == ComponentDefinitionExtension; }
bool Assets::IsSamplerFile(const FNode& file)             noexcept { return file.ext == SamplerExtension;             }
bool Assets::IsTextureFile(const FNode& file) noexcept {
    return file.ext == TextureExtensionPNG ||
        file.ext == TextureExtensionBMP ||
        file.ext == TextureExtensionTGA ||
        file.ext == TextureExtensionJPG ||
        file.ext == TextureExtensionJPEG;
}
bool Assets::IsShaderFile(const FNode& file) noexcept {
    return  IsVertexShaderFile(file) ||
            IsTessellationControlShaderFile(file) ||
            IsTessellationEvaluationShaderFile(file) ||
            IsGeometryShaderFile(file) ||
            IsFragmentShaderFile(file) ||
            IsComputeShaderFile(file);
}
bool Assets::IsVertexShaderFile(const FNode& file)                 noexcept { return file.ext == VertexShaderExtension;                 }
bool Assets::IsTessellationControlShaderFile(const FNode& file)    noexcept { return file.ext == TessellationControlShaderExtension;    }
bool Assets::IsTessellationEvaluationShaderFile(const FNode& file) noexcept { return file.ext == TessellationEvaluationShaderExtension; }
bool Assets::IsGeometryShaderFile(const FNode& file)               noexcept { return file.ext == GeometryShaderExtension;               }
bool Assets::IsFragmentShaderFile(const FNode& file)               noexcept { return file.ext == FragmentShaderExtension;               }
bool Assets::IsComputeShaderFile(const FNode & file)               noexcept { return file.ext == ComputeShaderExtension;                }
bool Assets::IsShaderProgramFile(const FNode& file)                noexcept { return file.ext == ShaderProgramExtension;                }
bool Assets::IsMaterialFile(const FNode& file)                     noexcept { return file.ext == MaterialExtension;                     }
bool Assets::IsFrameBufferFile(const FNode& file)                  noexcept { return file.ext == FrameBufferExtension;                  }
bool Assets::IsMeshFile(const FNode& file)                         noexcept { return file.ext == MeshExtension;                         }
bool Assets::IsModelFile(const FNode& file) noexcept {
    return file.ext == ModelExtensionFBX ||
        file.ext == ModelExtensionGLB;
}
bool Assets::IsScriptFile(const FNode& file)                       noexcept { return file.ext == SCRIPT_EXT;                            }

Assets::Assets(const Project& project, AssetGPUBridge& bridge) noexcept :
    _root({ &project, nullptr, "", "", "", true }),
    bridge(bridge) {
    BuildFileNodeTree(project, _root);
    ImportAllFiles(database, _root);
}

FNode& Assets::CreateFolder(FNode& parentFolder, const std::string_view folderName) noexcept {
    FNode& rv = *parentFolder.CreateChildFolder({ parentFolder.owner, &parentFolder, std::string(folderName) });
    ReimportAll(); /* folder structure is mutated, must reimport to re-build! */
    return rv;
}
void Assets::MoveFolder(FNode& folder, FNode& targetParentFolder) noexcept {
    folder.MoveUnder(targetParentFolder);
    // ReimportAll(); /* folder structure is mutated, must reimport to re-build! */
}
void Assets::DeleteFolder(FNode& folder) noexcept {
    for (auto& child : folder.Children()) {
        if (child.IsDirectory()) {
            DeleteFolder(child);
        } else {
            DeleteAsset(files[&child]);
        }
    }
    folder.Delete();
    // ReimportAll(); /* folder structure is mutated, must reimport to re-build! */
}

AssetHandle Assets::CreateAssetAt(FNode& folderPath, const std::string_view fileName, const std::string_view serializedData) noexcept {
    std::filesystem::path p = fileName;
    const FNode* newAssetFile = folderPath.CreateChildFile({
        folderPath.owner,
        &folderPath,
        p.stem().string(),
        p.extension().string(),
        std::string(serializedData)
    });
    const auto [uuid, handle] = ImportFile(database, *newAssetFile);
    ForceDeserializeAsset(uuid);
    return handle;
}
void Assets::MoveAsset(const UUID uuid, FNode& targetParentFolder) noexcept {
    assert(database.Contains(uuid));
    database.files[uuid]->MoveUnder(targetParentFolder);
    ReimportAll();
}
void Assets::DeleteAsset(const UUID uuid) noexcept {
    assert(database.Contains(uuid));

    FNode& file = *database.files[uuid];
    files.erase(&file);
    file.Delete();

    database.Remove(uuid);
    std::erase(allAssets, uuid);
    std::erase(sceneAssets, uuid);
    std::erase(componentDefinitionAssets, uuid);
    std::erase(shaderAssets, uuid);
    std::erase(shaderProgramAssets, uuid);
    std::erase(materialAssets, uuid);
    std::erase(textureAssets, uuid);
    std::erase(frameBufferAssets, uuid);
    std::erase(meshAssets, uuid);
    std::erase(modelAssets, uuid);

    dependencyGraph.RemoveVertex(uuid);

    DeleteOtherResourcesCorrespondingToDeletedAsset(uuid);
    Events.OnAssetDataDeleted(uuid);
    Events.OnAssetDestructed(uuid);
}

AssetHandle Assets::FindAsset(UUID uuid) const noexcept {
    if (!database.Contains(uuid)) { return nullptr; }

    /*
    * casting away const is safe here
    * because database does not contain "const Asset*"
    * in the first place, it contains "Asset"
    */
    return { const_cast<Asset*>(&database[uuid]) };
}
AssetHandle Assets::FindAssetAt(const FNode& file) const noexcept {
    if (files.contains(&file)) {
        UUID uuid = files.find(&file)->second;
        /*
        * casting away const is safe here
        * because database does not contain "const Asset"
        * in the first place, it contains "Asset"
        */
        return { const_cast<Asset*>(&database[uuid]) };
    }
    return nullptr;
}
bool Assets::IsAssetExistsAt(const FNode& file) const noexcept { return files.contains(&file); }

FNode& Assets::GetFileOfAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return *const_cast<AssetDatabase&>(database).files[uuid];
}
const AssetData& Assets::GetDataOfAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return const_cast<AssetDatabase&>(database).data[uuid];
}
SubAssetList Assets::GetSubAssetsOfAsset(const UUID uuid) const noexcept {
    auto index = database.subAssets.FindNodeIndexBFS(uuid, AssetDatabase::SubAssetTree::Root);
    if (index == AssetDatabase::SubAssetTree::Invalid) {
        // No such entry at sub-asset tree? Ok, then it has no sub-assets!
        // Simply create a ChildrenList with 0 indices
        // and use that "empty" ChildrenList to create SubAssetList.
        static const std::vector<AssetDatabase::SubAssetTree::NodeIndex>& EmptyList{};
        return { AssetDatabase::SubAssetTree::ChildrenList(const_cast<AssetDatabase&>(database).subAssets, EmptyList), *this };
    }
    return { database.subAssets.ChildrenOfNodeAt(index), *this};
}
uint64_t Assets::GetVersionOfAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return const_cast<AssetDatabase&>(database).versions[uuid];
}
std::optional<std::string_view> Assets::TryGetNameOfAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    const AssetData& data = const_cast<AssetDatabase&>(database).data[uuid];
    return std::visit(overloaded::lambda {
        [] (const EmptyAssetData& empty) -> std::optional<std::string_view> {
            if (empty.Name == "") {
                return std::nullopt;
            }
            return empty.Name;
        },
        [] (const Scene& scene)            -> std::optional<std::string_view> { return scene.Name;       },
        [](const Component& component)     -> std::optional<std::string_view> { return component.Name;   },
        [](const Sampler& sampler)         -> std::optional<std::string_view> { return sampler.Name;     },
        [](const Texture& texture)         -> std::optional<std::string_view> { return texture.Name;     },
        [](const Shader& shader)           -> std::optional<std::string_view> { return shader.Name;      },
        [](const ShaderProgram& program)   -> std::optional<std::string_view> { return program.Name;     },
        [](const Material& material)       -> std::optional<std::string_view> { return material.Name;    },
        [](const FrameBuffer& framebuffer) -> std::optional<std::string_view> { return framebuffer.Name; },
        [](const Mesh& mesh)               -> std::optional<std::string_view> { return mesh.Name;        },
        [](const Model& model)             -> std::optional<std::string_view> { return model.Name;       },
    }, data);
}
HashedString Assets::GetTypeNameOfAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    const AssetData& data = const_cast<AssetDatabase&>(database).data[uuid];
    using namespace entt::literals;
    return std::visit(overloaded::lambda{
        [](const EmptyAssetData& empty)                     -> HashedString { return empty.Type;            },
        []([[maybe_unused]] const Scene& scene)             -> HashedString { return SceneTypeName;         },
        []([[maybe_unused]] const Component& component)     -> HashedString { return ComponentTypeName;     },
        []([[maybe_unused]] const Sampler& sampler)         -> HashedString { return SamplerTypeName;       },
        []([[maybe_unused]] const Texture& texture)         -> HashedString { return TextureTypeName;       },
        []([[maybe_unused]] const Shader& shader)           -> HashedString { return ShaderTypeName;        },
        []([[maybe_unused]] const ShaderProgram& program)   -> HashedString { return ShaderProgramTypeName; },
        []([[maybe_unused]] const Material& material)       -> HashedString { return MaterialTypeName;      },
        []([[maybe_unused]] const FrameBuffer& framebuffer) -> HashedString { return FrameBufferTypeName;   },
        []([[maybe_unused]] const Mesh& mesh)               -> HashedString { return MeshTypeName;          },
        []([[maybe_unused]] const Model& model)             -> HashedString { return ModelTypeName;         },
    }, data);
}

void Assets::SerializeAsset(const UUID uuid) noexcept {
    assert(database.Contains(uuid));

    if (!AssetHasDeserializedData(uuid)) {
        DOA_LOG_WARNING("Couldn't serialize asset. Asset is not deserialized.");
        return;
    }
    if (!AssetHasContentInSystemMemory(uuid)) {
        DOA_LOG_WARNING("Couldn't serialize asset. Asset's must have content in system memory! Content is unloaded, you can load it by calling Assets::ReadContentOfAssetIntoSystemMemory");
        return;
    }

    FNode* file = database.files[uuid];

    if (IsSceneAsset(uuid)) {
        std::string serializedData;
        serializedData = GetDataOfAssetAs<Scene>(uuid).Serialize();
        tinyxml2::XMLDocument doc;
        doc.Parse(serializedData.c_str());
        doc.SaveFile(file->AbsolutePath().string().c_str());
        return;
    }
    if (IsComponentDefinitionAsset(uuid)) {
        std::string serializedData;
        serializedData = GetDataOfAssetAs<Component>(uuid).Serialize();
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
    if (IsSamplerAsset(uuid)) {
        std::string serializedData;
        serializedData = GetDataOfAssetAs<Sampler>(uuid).Serialize();
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
    if (IsTextureAsset(uuid)) {
        EncodedTextureData serializedData;
        serializedData = SerializeTexture(GetDataOfAssetAs<Texture>(uuid), ExtToEncoding(file->Extension())).value();

        std::string stringified;
        stringified.reserve(serializedData.EncodedData.size());
        for (const std::byte b : serializedData.EncodedData) {
            stringified.push_back(static_cast<char>(b));
        }

        file->ModifyContent(std::move(stringified));
        file->DisposeContent();
    }
    if (IsShaderAsset(uuid)) {
        std::string serializedData;
        serializedData = GetDataOfAssetAs<Shader>(uuid).Serialize();
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
    if (IsShaderProgramAsset(uuid)) {
        std::string serializedData;
        serializedData = GetDataOfAssetAs<ShaderProgram>(uuid).Serialize();
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
    if (IsMaterialAsset(uuid)) {
        std::string serializedData;
        serializedData = GetDataOfAssetAs<Material>(uuid).Serialize();
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
    if (IsFrameBufferAsset(uuid)) {
        std::string serializedData;
        serializedData = GetDataOfAssetAs<FrameBuffer>(uuid).Serialize();
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
    if (IsMeshAsset(uuid)) {
        std::string serializedData;
        serializedData = GetDataOfAssetAs<Mesh>(uuid).Serialize();
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
    if (IsModelAsset(uuid)) {
        DOA_LOG_ERROR("MODEL ASSET SERIALIZATION NOT IMPLEMENTED YET!");
        DOA_LOG_ERROR("MODEL ASSET SERIALIZATION NOT IMPLEMENTED YET!");
        DOA_LOG_ERROR("MODEL ASSET SERIALIZATION NOT IMPLEMENTED YET!");
        DOA_LOG_ERROR("MODEL ASSET SERIALIZATION NOT IMPLEMENTED YET!");
        DOA_LOG_ERROR("MODEL ASSET SERIALIZATION NOT IMPLEMENTED YET!");
        // TODO oof...
        //std::string serializedData;
        //serializedData = SerializeModel(GetDataOfAssetAs<Model>(uuid));
        //file->ModifyContent(std::move(serializedData));
        //file->DisposeContent();
    }
}
void Assets::DeserializeAsset(const UUID uuid) noexcept {
    assert(database.Contains(uuid));

    FNode* file = database.files[uuid];
    AssetData& data = database.data[uuid];
    auto& infoList = database.infoLists[uuid];
    auto& warningList = database.warningLists[uuid];
    auto& errorList = database.errorLists[uuid];

    DOA_LOG_TRACE("Deserializing asset \"%s\" with UUID: %s", file->Name().data(), uuid.AsString().c_str());

    if (AssetHasDeserializedData(uuid)) {
        DOA_LOG_INFO("Didn't deserialize asset \"%s\" with UUID: %s.", file->Name().data(), uuid.AsString().c_str());
        DOA_LOG_INFO("\tAsset already has deserialized data.");
        return;
    }

    if (IsSceneAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Scene asset.");
        data = DeserializeScene(*file);
    }
    if (IsComponentDefinitionAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Component Definition asset.");
        auto result = DeserializeComponent(*file);
        for (auto& message : result.messages) {
            switch (message.messageType) {
            case ComponentCompilerMessageType::Info:
                infoList.emplace_back(std::move(message));
                break;
            case ComponentCompilerMessageType::Warning:
                warningList.emplace_back(std::move(message));
                break;
            case ComponentCompilerMessageType::Error:
                errorList.emplace_back(std::move(message));
                break;
            }
        }
        data = std::move(result.deserializedComponent);
    }
    if (IsSamplerAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Sampler asset.");
        SamplerDeserializationResult result = DeserializeSampler(*file);
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedSampler);
    }
    if (IsTextureAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Texture asset.");
        TextureDeserializationResult result = DeserializeTexture(*file);
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedTexture);
    }
    if (IsShaderAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Shader asset.");
        ShaderDeserializationResult result;
        if (Assets::IsVertexShaderFile(*file)) {
            DOA_LOG_TRACE("\t\tExtra, detected as a Vertex Shader asset.");
            result = DeserializeVertexShader(*file);
        }
        if (Assets::IsTessellationControlShaderFile(*file)) {
            DOA_LOG_TRACE("\t\tExtra, detected as a Tessellation Control Shader asset.");
            result = DeserializeTessellationControlShader(*file);
        }
        if (Assets::IsTessellationEvaluationShaderFile(*file)) {
            DOA_LOG_TRACE("\t\tExtra, detected as a Tessellation Evaluation Shader asset.");
            result = DeserializeTessellationEvaluationShader(*file);
        }
        if (Assets::IsGeometryShaderFile(*file)) {
            DOA_LOG_TRACE("\t\tExtra, detected as a Geometry Shader asset.");
            result = DeserializeGeometryShader(*file);
        }
        if (Assets::IsFragmentShaderFile(*file)) {
            DOA_LOG_TRACE("\t\tExtra, detected as a Fragment Shader asset.");
            result = DeserializeFragmentShader(*file);
        }
        if (Assets::IsComputeShaderFile(*file)) {
            DOA_LOG_TRACE("\t\tExtra, detected as a Compute Shader asset.");
            result = DeserializeComputeShader(*file);
        }
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedShader);
    }
    if (IsShaderProgramAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Shader Program asset.");
        ShaderProgramDeserializationResult result = DeserializeShaderProgram(*file);
        if (result.erred) {
            for (auto& error : result.errors) {
                errorList.emplace_back(std::move(error));
            }
        } else {
            infoList.emplace_back(std::string("Shader Program is complete and ready for use."));
        }
        data = std::move(result.deserializedShaderProgram);
    }
    if (IsMaterialAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Material asset.");
        MaterialDeserializationResult result = DeserializeMaterial(*file);
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedMaterial);
    }
    if (IsFrameBufferAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Frame Buffer asset.");
        FrameBufferDeserializationResult result = DeserializeFrameBuffer(*file);
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedFrameBuffer);
    }
    if (IsMeshAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Mesh asset.");
        MeshDeserializationResult result = DeserializeMesh(*file);
        for (auto& warning : result.warnings) {
            warningList.emplace_back(std::move(warning));
        }
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedMesh);
    }
    if (IsModelAsset(uuid)) {
        DOA_LOG_TRACE("\tDetected as a Model asset.");
        ModelDeserializationResult result = DeserializeModel(*file);
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        Model& model{ result.deserializedModel };

        auto index = database.subAssets.EmplaceNode(AssetDatabase::SubAssetTree::Root, uuid);
        model.Meshes.resize(result.deserializedMeshes.size());
        for (size_t i = 0; i < result.deserializedMeshes.size(); i++) {
            Mesh& mesh = result.deserializedMeshes[i];
            UUID meshID = FindOrGenerateUUIDForSubAsset(uuid, mesh.Name, MeshTypeName);
            database.EmplaceAsSubAsset(meshID, uuid, index) = std::move(mesh);
            model.Meshes[i] = { meshID, result.meshMaterialIndices[i] };
        }
        model.Textures.reserve(result.deserializedTextures.size());
        for (auto& var : result.deserializedTextures) {
            Model::Texture texture;
            texture = std::visit(overloaded::lambda {
                [this, uuid](const std::filesystem::path& path) -> Model::Texture {
                    auto search = std::ranges::find_if(files, [&path](const auto& elem) {
                        return elem.first->Path() == path;
                    });
                    assert(search != files.end());

                    UUID textureUUID = search->second;
                    TryRegisterDependencyBetween(uuid, textureUUID);
                    return { textureUUID, false };
                },
                [this, uuid, index](TextureDeserializationResult& tdr) -> Model::Texture {
                    if (tdr.erred) { return { UUID::Empty(), false }; }

                    Texture& texture = tdr.deserializedTexture;
                    UUID textureUUID = FindOrGenerateUUIDForSubAsset(uuid, texture.Name, TextureTypeName);
                    database.EmplaceAsSubAsset(textureUUID, uuid, index) = std::move(texture);
                    return { textureUUID, true };
                }
            }, var);

            if (texture.TextureUUID != UUID::Empty()) {
                model.Textures.push_back(texture);
            }
        }
        model.Textures.shrink_to_fit();

        // Must re-bind as erratic additions in the above loops may (or not) cause re-alloc on database...
        AssetData& data = database.data[uuid];
        data = std::move(model);
    }
    /*
    * TODO others
    */
    database.versions[uuid]++;

    // Handle post deserialization actions (if any needed)
    if (IsSceneAsset(uuid)) { PerformPostDeserializationAction<Scene>(uuid); }
    if (IsComponentDefinitionAsset(uuid)) { PerformPostDeserializationAction<Component>(uuid); }
    if (IsSamplerAsset(uuid)) { PerformPostDeserializationAction<Sampler>(uuid); }
    if (IsTextureAsset(uuid)) { PerformPostDeserializationAction<Texture>(uuid); }
    if (IsShaderAsset(uuid)) { PerformPostDeserializationAction<Shader>(uuid); }
    if (IsShaderProgramAsset(uuid)) { PerformPostDeserializationAction<ShaderProgram>(uuid); }
    if (IsMaterialAsset(uuid)) { PerformPostDeserializationAction<Material>(uuid); }
    if (IsFrameBufferAsset(uuid)) { PerformPostDeserializationAction<FrameBuffer>(uuid); }
    if (IsMeshAsset(uuid)) { PerformPostDeserializationAction<Mesh>(uuid); }
    if (IsModelAsset(uuid)) { PerformPostDeserializationAction<Model>(uuid); }

    if (dependencyGraph.HasVertex(uuid)) {
        auto edgeVertices = dependencyGraph.GetIncomingEdgesOf(uuid);
        while (edgeVertices.HasNext()) {
            const UUID& dependentID = edgeVertices.Next();
            assert(database.Contains(dependentID));
            database[dependentID].ForceDeserialize();
        }
    }

    Events.OnAssetDeserialized(uuid);

    ReleaseContentOfAssetInSystemMemory(uuid);

    DOA_LOG_TRACE("Deserialized asset \"%s\" with UUID: %s", file->Name().data(), uuid.AsString().c_str());
}
void Assets::ForceDeserializeAsset(const UUID uuid) noexcept {
    assert(database.Contains(uuid));
    DeleteDeserializedDataOfAsset(uuid);
    DeserializeAsset(uuid);
}
void Assets::DeleteDeserializedDataOfAsset(const UUID uuid) noexcept {
    assert(database.Contains(uuid));
    if (IsSubAsset(uuid)) {
        DOA_LOG_WARNING("Assets::DeleteDeserializedDataOfAsset called with UUID: %s\n. SubAsset data cannot be deleted by itself. Try deleting parent data.");
        return;
    }
    DeleteOtherResourcesCorrespondingToDeletedAsset(uuid);

    database.data[uuid] = CreateEmptyAssetDataUsingData(database.data[uuid]);
    database.infoLists[uuid].clear();
    database.warningLists[uuid].clear();
    database.errorLists[uuid].clear();
    database.versions[uuid]++;

    Events.OnAssetDataDeleted(uuid);
}
bool Assets::AssetHasDeserializedData(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return !std::holds_alternative<EmptyAssetData>(const_cast<AssetDatabase&>(database).data[uuid]);
}
UUID Assets::InstantiateAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return UUID(); // TODO instatiate
}
bool Assets::AssetHasContentInSystemMemory(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));

    if (!AssetHasDeserializedData(uuid)) { return false; }

    if (IsSceneAsset(uuid))               { return true; }
    if (IsComponentDefinitionAsset(uuid)) { return true; }
    if (IsSamplerAsset(uuid))             { return true; }
    if (IsTextureAsset(uuid))             { return GetDataOfAssetAs<Texture>(uuid).PixelData.has_value(); }
    if (IsShaderAsset(uuid))              { return true; }
    if (IsShaderProgramAsset(uuid))       { return true; }
    if (IsMaterialAsset(uuid))            { return true; }
    if (IsFrameBufferAsset(uuid))         { return true; }
    if (IsMeshAsset(uuid))                { return GetDataOfAssetAs<Mesh>(uuid).Vertices.has_value(); }
    if (IsModelAsset(uuid))               {
        const Model& model = GetDataOfAssetAs<Model>(uuid);
        for (const auto& mMesh : model.Meshes) {
            const Mesh& mesh = GetDataOfAssetAs<Mesh>(mMesh.MeshUUID);

            if (!mesh.Vertices.has_value()) { return false; }
        }
        for (const auto& mTexture : model.Textures) {
            if (!mTexture.Embedded) { continue; }

            const Texture& texture = GetDataOfAssetAs<Texture>(mTexture.TextureUUID);

            if (!texture.PixelData.has_value()) { return false; }
        }
        return true;
    }
    std::unreachable(); // forgot to check an asset type...
}
bool Assets::AssetHasContentInVideoMemory(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));

    if (!AssetHasDeserializedData(uuid)) { return false; }

    if (IsSceneAsset(uuid))               { return false; }
    if (IsComponentDefinitionAsset(uuid)) { return false; }
    if (IsSamplerAsset(uuid))             { return bridge.GetSamplers().Exists(uuid); }
    if (IsTextureAsset(uuid))             { return bridge.GetTextures().Exists(uuid); }
    if (IsShaderAsset(uuid))              { return bridge.GetShaders().Exists(uuid); }
    if (IsShaderProgramAsset(uuid))       { return bridge.GetShaderPrograms().Exists(uuid); }
    if (IsMaterialAsset(uuid))            { return false; }
    if (IsFrameBufferAsset(uuid))         { return bridge.GetFrameBuffers().Exists(uuid); }
    if (IsMeshAsset(uuid))                { return bridge.GetVertexBuffers().Exists(uuid); }
    if (IsModelAsset(uuid))               {
        const Model& model = GetDataOfAssetAs<Model>(uuid);
        for (const auto& mMesh : model.Meshes) {
            if (!bridge.GetVertexBuffers().Exists(mMesh.MeshUUID)) { return false; }
        }
        for (const auto& mTexture : model.Textures) {
            if (!mTexture.Embedded) { continue; }

            if (!bridge.GetTextures().Exists(mTexture.TextureUUID)) { return false; }
        }
        return true;
    }
    std::unreachable(); // forgot to check an asset type...
}
void Assets::ReadContentOfAssetIntoSystemMemory(const UUID uuid) noexcept {
    assert(database.Contains(uuid));

    FNode* file = database.files[uuid];
    AssetData& data = database.data[uuid];
    auto& infoList = database.infoLists[uuid];
    auto& warningList = database.warningLists[uuid];
    auto& errorList = database.errorLists[uuid];

    if (IsTextureAsset(uuid)) {
        TextureDeserializationResult result = DeserializeTexture(*file);
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedTexture);
    }
    if (IsMeshAsset(uuid)) {
        MeshDeserializationResult result = DeserializeMesh(*file);
        for (auto& warning : result.warnings) {
            warningList.emplace_back(std::move(warning));
        }
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedMesh);
    }
    if (IsModelAsset(uuid)) {
        ModelDeserializationResult result = DeserializeModel(*file);
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        Model& model{ result.deserializedModel };

        auto index = database.subAssets.EmplaceNode(AssetDatabase::SubAssetTree::Root, uuid);
        model.Meshes.resize(result.deserializedMeshes.size());
        for (size_t i = 0; i < result.deserializedMeshes.size(); i++) {
            Mesh& mesh = result.deserializedMeshes[i];
            UUID meshID = FindOrGenerateUUIDForSubAsset(uuid, mesh.Name, MeshTypeName);
            database.EmplaceAsSubAsset(meshID, uuid, index) = std::move(mesh);
            model.Meshes[i] = { meshID, result.meshMaterialIndices[i] };
        }
        model.Textures.reserve(result.deserializedTextures.size());
        for (auto& var : result.deserializedTextures) {
            Model::Texture texture;
            texture = std::visit(overloaded::lambda{
                [this, uuid](const std::filesystem::path& path) -> Model::Texture {
                    auto search = std::ranges::find_if(files, [&path](const auto& elem) {
                        return elem.first->Path() == path;
                    });
                    assert(search != files.end());

                    UUID textureUUID = search->second;
                    TryRegisterDependencyBetween(uuid, textureUUID);
                    return { textureUUID, false };
                },
                [this, uuid, index](TextureDeserializationResult& tdr) -> Model::Texture {
                    if (tdr.erred) { return { UUID::Empty(), false }; }

                    Texture& texture = tdr.deserializedTexture;
                    UUID textureUUID = FindOrGenerateUUIDForSubAsset(uuid, texture.Name, TextureTypeName);
                    database.EmplaceAsSubAsset(textureUUID, uuid, index) = std::move(texture);
                    return { textureUUID, true };
                }
            }, var);

            if (texture.TextureUUID != UUID::Empty()) {
                model.Textures.push_back(texture);
            }
        }
        model.Textures.shrink_to_fit();

        // Must re-bind as erratic additions in the above loops may (or not) cause re-alloc on database...
        AssetData& data = database.data[uuid];
        data = std::move(model);
    }
}
void Assets::ReleaseContentOfAssetInSystemMemory(const UUID uuid) noexcept {
    assert(database.Contains(uuid));

    if (!AssetHasDeserializedData(uuid)) { return; }

    if (IsTextureAsset(uuid)) {
        GetDataOfAssetAs<Texture>(uuid).PixelData.reset();
    }
    if (IsMeshAsset(uuid)) {
        Mesh& mesh = GetDataOfAssetAs<Mesh>(uuid);
        mesh.Vertices.reset();
        mesh.Indices.reset();
    }
    if (IsModelAsset(uuid)) {
        Model& model = GetDataOfAssetAs<Model>(uuid);
        for (auto& mMesh : model.Meshes) {
            ReleaseContentOfAssetInSystemMemory(mMesh.MeshUUID);
        }
        for (auto& mTexture : model.Textures) {
            if (!mTexture.Embedded) { continue; }

            ReleaseContentOfAssetInSystemMemory(mTexture.TextureUUID);
        }
    }
}
void Assets::UploadContentOfAssetIntoVideoMemory(const UUID uuid) noexcept {
    assert(database.Contains(uuid));

    if (!AssetHasDeserializedData(uuid)) { return; }

    if (IsTextureAsset(uuid)) {
        std::vector<TextureAllocatorMessage> messages = bridge.GetTextures().Allocate(*this, uuid);

        std::vector<std::any>& errorMessages = database.errorLists[uuid];
        for (auto& message : messages) {
            errorMessages.emplace_back(std::move(message));
        }
    }
    if (IsMeshAsset(uuid)) {
        std::vector<BufferAllocatorMessage> vMessages = bridge.GetVertexBuffers().Allocate(*this, uuid);
        std::vector<BufferAllocatorMessage> iMessages = bridge.GetIndexBuffers().Allocate(*this, uuid);

        std::vector<std::any>& infoMessages = database.infoLists[uuid];
        std::vector<std::any>& warningMessages = database.warningLists[uuid];
        std::vector<std::any>& errorMessages = database.errorLists[uuid];
        for (auto& message : vMessages) {
            errorMessages.emplace_back(std::move(message));
        }
        for (auto& message : iMessages) {
            errorMessages.emplace_back(std::move(message));
        }
        if (errorMessages.empty()) {
            Mesh& mesh = GetDataOfAssetAs<Mesh>(uuid);

            assert(mesh.Vertices.has_value());
            Mesh::VertexList& vertices = mesh.Vertices.value();
            infoMessages.emplace_back(std::string("Mesh deserialized successfully."));
            infoMessages.emplace_back(std::format("\tVertex Count: {}", vertices.size()));
            infoMessages.emplace_back(std::format("\t\tVertex Buffer Size: {} bytes (VRAM)", std::span{ vertices }.size_bytes()));
            if (mesh.Indices.has_value()) {
                Mesh::IndexList& indices = mesh.Indices.value();
                infoMessages.emplace_back(std::format("\tIndex Count: {}", indices.size()));
                infoMessages.emplace_back(std::format("\t\tIndex Buffer Size: {} bytes (VRAM)", std::span{ indices }.size_bytes()));

                if (indices.size() % 3 != 0) {
                    warningMessages.emplace_back(std::string("Index count not divisible by 3. Mesh faces might be ill-formed."));
                }
            } else {
                warningMessages.emplace_back(std::string("Mesh has no index table. This may affect performance."));
            }
        }
    }
    if (IsModelAsset(uuid)) {
        Model& model = GetDataOfAssetAs<Model>(uuid);
        for (auto& mMesh : model.Meshes) {
            UploadContentOfAssetIntoVideoMemory(mMesh.MeshUUID);
        }
        for (auto& mTexture : model.Textures) {
            if (!mTexture.Embedded) { continue; }

            UploadContentOfAssetIntoVideoMemory(mTexture.TextureUUID);
        }
    }
}
void Assets::ReleaseContentOfAssetInVideoMemory(const UUID uuid) noexcept {
    assert(database.Contains(uuid));

    if (IsTextureAsset(uuid)) {
        bridge.GetTextures().Deallocate(uuid);
    }
    if (IsMeshAsset(uuid)) {
        bridge.GetVertexBuffers().Deallocate(uuid);
        bridge.GetIndexBuffers().Deallocate(uuid);
    }
    if (IsModelAsset(uuid)) {
        Model& model = GetDataOfAssetAs<Model>(uuid);
        for (auto& mMesh : model.Meshes) {
            ReleaseContentOfAssetInVideoMemory(mMesh.MeshUUID);
        }
        for (auto& mTexture : model.Textures) {
            if (!mTexture.Embedded) { continue; }

            ReleaseContentOfAssetInVideoMemory(mTexture.TextureUUID);
        }
    }
}

bool Assets::IsSceneAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == SceneTypeName;
}
bool Assets::IsComponentDefinitionAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == ComponentTypeName;
}
bool Assets::IsSamplerAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == SamplerTypeName;
}
bool Assets::IsTextureAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == TextureTypeName;
}
bool Assets::IsShaderAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == ShaderTypeName;
}
bool Assets::IsShaderProgramAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == ShaderProgramTypeName;
}
bool Assets::IsMaterialAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == MaterialTypeName;
}
bool Assets::IsFrameBufferAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == FrameBufferTypeName;
}
bool Assets::IsMeshAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == MeshTypeName;
}
bool Assets::IsModelAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == ModelTypeName;
}
bool Assets::IsScriptAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return GetTypeNameOfAsset(uuid) == "Script";
}

bool Assets::IsSubAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    FNode* file = const_cast<AssetDatabase&>(database).files[uuid];
    UUID id = const_cast<Assets&>(*this).files[file];
    return id != uuid; // id is own-id if not sub asset. parent-id if otherwise.
}

bool Assets::AssetHasInfoMessages(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return !const_cast<AssetDatabase&>(database).infoLists[uuid].empty();
}
const std::vector<std::any>& Assets::GetInfoMessagesOfAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return const_cast<AssetDatabase&>(database).infoLists[uuid];
}

bool Assets::AssetHasWarningMessages(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return !const_cast<AssetDatabase&>(database).warningLists[uuid].empty();
}
const std::vector<std::any>& Assets::GetWarningMessagesOfAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return const_cast<AssetDatabase&>(database).warningLists[uuid];
}

bool Assets::AssetHasErrorMessages(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return !const_cast<AssetDatabase&>(database).errorLists[uuid].empty();
}
const std::vector<std::any>& Assets::GetErrorMessagesOfAsset(const UUID uuid) const noexcept {
    assert(database.Contains(uuid));
    return const_cast<AssetDatabase&>(database).errorLists[uuid];
}

FNode& Assets::Root() noexcept { return _root; }
const FNode& Assets::Root() const noexcept { return _root; }

const Assets::UUIDCollection& Assets::AllAssetsIDs()                const noexcept { return allAssets;                 }
const Assets::UUIDCollection& Assets::SceneAssetIDs()               const noexcept { return sceneAssets;               }
const Assets::UUIDCollection& Assets::ScriptAssetIDs()              const noexcept { return scriptAssets;              }
const Assets::UUIDCollection& Assets::SamplerAssetIDs()             const noexcept { return samplerAssets;             }
const Assets::UUIDCollection& Assets::TextureAssetIDs()             const noexcept { return textureAssets;             }
const Assets::UUIDCollection& Assets::ComponentDefinitionAssetIDs() const noexcept { return componentDefinitionAssets; }
const Assets::UUIDCollection& Assets::ShaderAssetIDs()              const noexcept { return shaderAssets;              }
const Assets::UUIDCollection& Assets::ShaderProgramAssetIDs()       const noexcept { return shaderProgramAssets;       }
const Assets::UUIDCollection& Assets::MaterialAssetIDs()            const noexcept { return materialAssets;            }
const Assets::UUIDCollection& Assets::FrameBufferAssetIDs()         const noexcept { return frameBufferAssets;         }
const Assets::UUIDCollection& Assets::MeshAssetIDs()                const noexcept { return meshAssets;                }
const Assets::UUIDCollection& Assets::ModelAssetIDs()               const noexcept { return modelAssets;               }

const AssetGPUBridge& Assets::GPUBridge() const noexcept { return bridge; }

std::pair<UUID, AssetHandle> Assets::Import(const FNode& file) noexcept { return ImportFile(database, file); }
void Assets::ReimportAll() noexcept {
    for (const auto& [uuid, _] : database.assets) {
        DeleteOtherResourcesCorrespondingToDeletedAsset(uuid);
        Events.OnAssetDataDeleted(uuid);
    }
    database.Clear();
    files.clear();
    allAssets.clear();
    sceneAssets.clear();
    componentDefinitionAssets.clear();
    samplerAssets.clear();
    textureAssets.clear();
    shaderAssets.clear();
    shaderProgramAssets.clear();
    materialAssets.clear();
    frameBufferAssets.clear();
    meshAssets.clear();
    modelAssets.clear();

    _root.children.clear();
    dependencyGraph.Clear();
    bridge.Clear();
    BuildFileNodeTree(*_root.OwningProject(), _root);
    ImportAllFiles(database, _root);
    EnsureDeserialization();
}
// donkey donk
void Assets::EnsureDeserialization() noexcept {
    // ORDER MATTERS
    Deserialize(componentDefinitionAssets);
    Deserialize(sceneAssets);
    Deserialize(textureAssets);
    Deserialize(samplerAssets);
    Deserialize(shaderAssets);
    Deserialize(shaderProgramAssets);
    Deserialize(materialAssets);
    Deserialize(frameBufferAssets);
    Deserialize(meshAssets);
    Deserialize(modelAssets);

    ReBuildDependencyGraph();
}

void Assets::TryRegisterDependencyBetween(UUID dependent, UUID dependency) noexcept {
    if (dependencyGraph.HasVertex(dependent) && !dependencyGraph.HasEdge(dependent, dependency)) {
        dependencyGraph.AddEdge(dependent, dependency);
    }
}
void Assets::TryDeleteDependencyBetween(UUID dependent, UUID dependency) noexcept {
    if (dependencyGraph.HasVertex(dependent) && dependencyGraph.HasEdge(dependent, dependency)) {
        dependencyGraph.RemoveEdge(dependent, dependency);
    }
}

EmptyAssetData Assets::CreateEmptyAssetDataUsingFile(const FNode& file) noexcept {
    if (Assets::IsSceneFile(file))               { return { file.Name(), Assets::SceneTypeName };         }
    if (Assets::IsComponentDefinitionFile(file)) { return { file.Name(), Assets::ComponentTypeName };     }
    if (Assets::IsShaderFile(file))              { return { file.Name(), Assets::ShaderTypeName };        }
    if (Assets::IsShaderProgramFile(file))       { return { file.Name(), Assets::ShaderProgramTypeName }; }
    if (Assets::IsMaterialFile(file))            { return { file.Name(), Assets::MaterialTypeName };      }
    if (Assets::IsSamplerFile(file))             { return { file.Name(), Assets::SamplerTypeName };       }
    if (Assets::IsTextureFile(file))             { return { file.Name(), Assets::TextureTypeName };       }
    if (Assets::IsFrameBufferFile(file))         { return { file.Name(), Assets::FrameBufferTypeName };   }
    if (Assets::IsMeshFile(file))                { return { file.Name(), Assets::MeshTypeName };          }
    if (Assets::IsModelFile(file))               { return { file.Name(), Assets::ModelTypeName };         }
    return { file.Name(), Assets::GenericAssetTypeName };
}
EmptyAssetData Assets::CreateEmptyAssetDataUsingData(const AssetData& data) noexcept {
    return std::visit(overloaded::lambda {
        [](const EmptyAssetData& empty)    -> EmptyAssetData { return empty; },
        [](const Scene& scene)             -> EmptyAssetData { return { scene.Name,       SceneTypeName         }; },
        [](const Component& component)     -> EmptyAssetData { return { component.Name,   ComponentTypeName     }; },
        [](const Sampler& sampler)         -> EmptyAssetData { return { sampler.Name,     SamplerTypeName       }; },
        [](const Texture& texture)         -> EmptyAssetData { return { texture.Name,     TextureTypeName       }; },
        [](const Shader& shader)           -> EmptyAssetData { return { shader.Name,      ShaderTypeName        }; },
        [](const ShaderProgram& program)   -> EmptyAssetData { return { program.Name,     ShaderProgramTypeName }; },
        [](const Material& material)       -> EmptyAssetData { return { material.Name,    MaterialTypeName      }; },
        [](const FrameBuffer& frameBuffer) -> EmptyAssetData { return { frameBuffer.Name, FrameBufferTypeName   }; },
        [](const Mesh& mesh)               -> EmptyAssetData { return { mesh.Name,        MeshTypeName          }; },
        [](const Model& model)             -> EmptyAssetData { return { model.Name,       ModelTypeName         }; },
    }, data);
    std::unreachable();
}

UUID Assets::GenerateUUID() const noexcept {
    UUID uuid{};
    while (database.Contains(uuid) || uuid == UUID::Empty()) {
        // collision detected! generate new UUID
        uuid = UUID();
    }
    return uuid;
}
UUID Assets::FindOrGenerateUUIDForSubAsset(UUID owner, std::string_view subAssetName, HashedString subAssetType) noexcept {
    /* Find or Generate a UUID for a SubAsset:
        * Step 1: Get the owner's import data file: fileName.fileExtension.id
        * Step 2: Read the file for "subAssets" element
            * Step 2a: If not exists, create the element, generate UUID and add it to the "subAssets" element
            * Step 2b: Else, if exists, look for all children, try to match the information in children with queried name and type
                * Step 2ba: If can match, read UUID from the node
                * Step 2bb: Else, if cant' match, genereate UUID and add it to the "subAssets" element
    */
    UUID rv{ UUID::Empty() };

    // Step 1
    FNode& ownerFile = *database.files[owner];
    FNode importData = FNode::HollowCopy(ownerFile);
    importData.ext.append(AssetIDExtension);
    importData.fullName.append(AssetIDExtension);

    // Step 2
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile(importData.AbsolutePath().string().c_str());
    assert(err != tinyxml2::XMLError::XML_ERROR_FILE_NOT_FOUND);
    assert(err == tinyxml2::XMLError::XML_SUCCESS);

    tinyxml2::XMLElement* subAssetsElement = doc.RootElement()->FirstChildElement("subAssets");
    if (!subAssetsElement) {
        // Step 2a
        tinyxml2::XMLPrinter printer;
        printer.PushComment("WARNING!! This file is not for editing! Don't!");
        printer.OpenElement("importData");
        printer.OpenElement("uuid");
        printer.PushText(owner);
        printer.CloseElement();

        printer.OpenElement("subAssets"); {
            printer.OpenElement("subAsset"); {
                printer.OpenElement("type");
                printer.PushText(subAssetType.data());
                printer.CloseElement();

                printer.OpenElement("name");
                printer.PushText(subAssetName.data());
                printer.CloseElement();

                rv = GenerateUUID();
                printer.OpenElement("uuid");
                printer.PushText(rv);
                printer.CloseElement();
            }
            printer.CloseElement();
        }
        printer.CloseElement();
        printer.CloseElement();

        doc.Parse(printer.CStr());
        doc.SaveFile(importData.AbsolutePath().string().c_str());

        if (ownerFile.HasContent()) {
            ownerFile.DisposeContent();
            ownerFile.ReadContent();
        }
    } else {
        // Step 2b
        assert(subAssetsElement->ChildElementCount() > 0);
        for (const tinyxml2::XMLElement* child = subAssetsElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
            // Step 2ba
            assert(std::string_view{ "subAsset" }.compare(child->Name()) == 0);

            const tinyxml2::XMLElement* typeElement = child->FirstChildElement("type");
            const tinyxml2::XMLElement* nameElement = child->FirstChildElement("name");
            const tinyxml2::XMLElement* uuidElement = child->FirstChildElement("uuid");
            assert(typeElement && nameElement && uuidElement &&
                   typeElement->GetText() && nameElement->GetText());

            if (std::string_view{ subAssetType.data() }.compare(typeElement->GetText()) == 0 &&
                subAssetName.compare(nameElement->GetText()) == 0) {
                uint64_t val;
                [[maybe_unused]] tinyxml2::XMLError error = uuidElement->QueryUnsigned64Text(&val);
                assert(error == tinyxml2::XMLError::XML_SUCCESS);
                rv = val;
            }
        }
        if (rv == UUID::Empty()) {
            // Step 2bb
            tinyxml2::XMLElement& newChild = *subAssetsElement->InsertNewChildElement("subAsset");

            tinyxml2::XMLElement& typeElement = *newChild.InsertNewChildElement("type");
            tinyxml2::XMLElement& nameElement = *newChild.InsertNewChildElement("name");
            tinyxml2::XMLElement& uuidElement = *newChild.InsertNewChildElement("uuid");

            typeElement.SetText(subAssetType.data());
            nameElement.SetText(subAssetName.data());

            rv = GenerateUUID();
            uuidElement.SetText(rv.AsString().c_str());

            doc.SaveFile(importData.AbsolutePath().string().c_str());

            if (ownerFile.HasContent()) {
                ownerFile.DisposeContent();
                ownerFile.ReadContent();
            }
        }
    }

    assert(rv != UUID::Empty());
    return rv;
}

std::pair<UUID, AssetHandle> Assets::ImportFile(AssetDatabase& database, const FNode& file) noexcept {
    /* Import a file:
        * Step 1: Get the sibling file: fileName.fileExtension.id
        * Step 2: Check if such file exists
        * Step 3: If not exists, create it, generate a UUID and write the UUID
        * Step 4: Read the file for the UUID
        * Step 5: If there is a collision, resolve it by generating new UUID's until the collision is resolved
        * Step 6: Register the file into the database
        * ------- Step 7: Call the importer to import the content to the memory
        (this is no longer the case, as we have dependencies between assets
        eg. Scene depends on ComponentDefinition or Material depends on Program, Program depends on Shader etc.)
        * Step 8: Separate imported asset to its own subcategory (and put it into allAssets list)
    */
    if (IsProjectFile(file)) { return { UUID::Empty(), nullptr }; }
    if (file.IsDirectory()) { return { UUID::Empty(), nullptr }; }
    if (file.ext == AssetIDExtension) { return { UUID::Empty(), nullptr }; }
    DOA_LOG_TRACE("Importing asset at \"%ls\"", file.Path().c_str());
    // Step 1
    FNode importData = FNode::HollowCopy(file);
    importData.ext.append(AssetIDExtension);
    importData.fullName.append(AssetIDExtension);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile(importData.AbsolutePath().string().c_str());
    // Step 2
    if (err == tinyxml2::XMLError::XML_ERROR_FILE_NOT_FOUND) {
        // Step 3
        doc.Clear();

        tinyxml2::XMLPrinter printer;
        printer.PushComment("WARNING!! This file is not for editing! Don't!");
        printer.OpenElement("importData");
        printer.OpenElement("uuid");
        printer.PushText(GenerateUUID());
        printer.CloseElement();
        printer.CloseElement();

        auto& siblings = file.parent->children;
        std::unique_ptr<FNode> id = std::make_unique<FNode>(FNodeCreationParams{ file.owner, file.parent, file.fullName, ".id" });
        auto pos = std::ranges::find_if(siblings, [&file](auto& ptr) {
            return ptr.get() == &file;
        });
        assert(pos != siblings.end()); // File you are attemping to import is not registered as a children of it's parent. Did you correctly call FNode::CreateChildFileFor?
        pos++;
        siblings.insert(pos, std::move(id));

        doc.Parse(printer.CStr());
        doc.SaveFile(importData.AbsolutePath().string().c_str());
        err = tinyxml2::XMLError::XML_SUCCESS; // Notice this line! This is for an unconditional fall to Step 4!
    }
    if (err == tinyxml2::XMLError::XML_SUCCESS) {
        // Step 4
        UUID uuid(doc.RootElement()->FirstChildElement("uuid")->Unsigned64Text(UUID::Empty()));
        assert(uuid != UUID::Empty()); // this should never happen, break

        // Step 5
        while (database.Contains(uuid)) {
            // collision detected! generate new UUID
            uuid = UUID();
            doc.RootElement()->FirstChildElement("uuid")->SetText(uuid);
            doc.SaveFile(importData.AbsolutePath().string().c_str());
        }

        // Step 6
        auto& asset = database.Emplace(uuid, const_cast<FNode*>(&file), *this);
        files.emplace(&file, uuid);

        // Step 7
        // asset.Deserialize();

        // Step 8
        allAssets.push_back(uuid);
        if (asset.IsScene()) {
            sceneAssets.push_back(uuid);
        }
        if (asset.IsComponentDefinition()) {
            componentDefinitionAssets.push_back(uuid);
        }
        if (asset.IsSampler()) {
            samplerAssets.push_back(uuid);
        }
        if (asset.IsTexture()) {
            textureAssets.push_back(uuid);
        }
        if (asset.IsShader()) {
            shaderAssets.push_back(uuid);
        }
        if (asset.IsShaderProgram()) {
            shaderProgramAssets.push_back(uuid);
        }
        if (asset.IsMaterial()) {
            materialAssets.push_back(uuid);
        }
        if (asset.IsFrameBuffer()) {
            frameBufferAssets.push_back(uuid);
        }
        if (asset.IsMesh()) {
            meshAssets.push_back(uuid);
        }
        if (asset.IsModel()) {
            modelAssets.push_back(uuid);
        }

        dependencyGraph.AddVertex(uuid);
        DOA_LOG_TRACE("Imported asset at \"%ls\"", file.Path().c_str());
        return { uuid, &asset };
    } else {
        DOA_LOG_ERROR("Failed to import asset at \"%ls\" do you have read/write access to the directory?", file.Path().c_str());
        return { UUID::Empty(), nullptr };
    }
}
void Assets::ImportAllFiles(AssetDatabase& database, const FNode& root) noexcept {
    // ImportFile mutates root.Children, therefore we can't use a range
    // for loop as; if a re-allocation were to happen, pointers used in
    // range for loop are invalidated.
    ImportFile(database, root);
    const auto children = root.Children();
    for (size_t i = 0; i < children.size(); i++) {
        ImportAllFiles(database, children[i]);
    }
}
void Assets::Deserialize(const UUIDCollection& assets) noexcept {
    for (const UUID id : assets) {
        database[id].DeleteDeserializedData();
        database[id].ForceDeserialize();
    }
}

void Assets::BuildFileNodeTree(const Project& project, FNode& root) noexcept {
    auto ws = project.Workspace();
    std::filesystem::current_path(ws);
    auto it = std::filesystem::directory_iterator(ws / root.Path());
    for (const auto& entry : it) {
        if(entry.is_directory()) {
            root.children.push_back(std::make_unique<FNode>(FNodeCreationParams{
                .owner = root.owner,
                .parent = &root,
                .name = entry.path().filename().replace_extension().string(),
                .isDirectory = true,
            }));
        } else {
            root.children.push_back(std::make_unique<FNode>(FNodeCreationParams{
                .owner = root.owner,
                .parent = &root,
                .name = entry.path().filename().replace_extension().string(),
                .ext = entry.path().extension().string(),
                .isDirectory = false,
            }));
        }
    }
    // DON'T merge this loop with the above.
    for (const auto& child : root.children) {
        if (child->IsDirectory()) {
            BuildFileNodeTree(project, *child);
        }
    }
}
void Assets::ReBuildDependencyGraph() noexcept {
    dependencyGraph.Clear();
    for (const auto& [id, _] : database) {
        dependencyGraph.AddVertex(id);
    }

    for (const auto& [id, asset] : database) {
        // Some asset types have no innate dependencies.
        if (asset.IsScene()) {}
        if (asset.IsComponentDefinition()) {}
        if (asset.IsSampler()) {}
        if (asset.IsTexture()) {}
        if (asset.IsShader()) {}
        if (asset.IsShaderProgram()) {
            const ShaderProgram& program = asset.DataAs<ShaderProgram>();
            if (program.HasVertexShader() && dependencyGraph.HasVertex(program.VertexShader)) {
                dependencyGraph.AddEdge(id, program.VertexShader);
            }
            if (program.HasTessellationControlShader() && dependencyGraph.HasVertex(program.TessellationControlShader)) {
                dependencyGraph.AddEdge(id, program.TessellationControlShader);
            }
            if (program.HasTessellationEvaluationShader() && dependencyGraph.HasVertex(program.TessellationEvaluationShader)) {
                dependencyGraph.AddEdge(id, program.TessellationEvaluationShader);
            }
            if (program.HasGeometryShader() && dependencyGraph.HasVertex(program.GeometryShader)) {
                dependencyGraph.AddEdge(id, program.GeometryShader);
            }
            if (program.HasFragmentShader() && dependencyGraph.HasVertex(program.FragmentShader)) {
                dependencyGraph.AddEdge(id, program.FragmentShader);
            }
        }
        if (asset.IsMaterial()) {
            const Material& material = asset.DataAs<Material>();
            if (material.HasShaderProgram() && dependencyGraph.HasVertex(material.ShaderProgram)) {
                dependencyGraph.AddEdge(id, material.ShaderProgram);
            }
        }
        if (asset.IsFrameBuffer()) {}
        if (asset.IsMesh()) {}
        if (asset.IsModel()) {}
    }
}

void Assets::DeleteOtherResourcesCorrespondingToDeletedAsset(const UUID uuid) noexcept {
    // Asset data deletion sometimes need to be resolved on GPU side too.
    // For example: A texture asset being deleted, must trigger a
    // texture object deletion on GPU. This functionality is performed
    // via AssetGPUBridge.
    if (uuid == UUID::Empty() || !AssetHasDeserializedData(uuid)) { return; }
    if (IsSceneAsset(uuid))               {                                                                                        }
    if (IsComponentDefinitionAsset(uuid)) {                                                                                        }
    if (IsSamplerAsset(uuid))             { bridge.GetSamplers().Deallocate(uuid);                                                 }
    if (IsTextureAsset(uuid))             { bridge.GetTextures().Deallocate(uuid);                                                 }
    if (IsShaderAsset(uuid))              { bridge.GetShaders().Deallocate(uuid);                                                  }
    if (IsShaderProgramAsset(uuid))       { bridge.GetShaderPrograms().Deallocate(uuid);                                           }
    if (IsMaterialAsset(uuid))            {                                                                                        }
    if (IsFrameBufferAsset(uuid))         { bridge.GetFrameBuffers().Deallocate(uuid);                                             }
    if (IsMeshAsset(uuid))                { bridge.GetVertexBuffers().Deallocate(uuid); bridge.GetIndexBuffers().Deallocate(uuid); }
    if (IsModelAsset(uuid))               {
        AssetData& data = database.data[uuid];
        Model& model = std::get<Model>(data);

        for (const auto& mMesh : model.Meshes) {
            bridge.GetVertexBuffers().Deallocate(mMesh.MeshUUID);
            bridge.GetIndexBuffers().Deallocate(mMesh.MeshUUID);
        }
        for (const auto& mTexture : model.Textures) {
            if (!mTexture.Embedded) { continue; }

            bridge.GetTextures().Deallocate(mTexture.TextureUUID);
        }
    }
}

template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<Scene>(std::string_view name) noexcept {
    return { name.data(), SceneTypeName };
}
template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<Component>(std::string_view name) noexcept {
    return { name.data(), ComponentTypeName };
}
template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<Sampler>(std::string_view name) noexcept {
    return { name.data(), SamplerTypeName };
}
template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<Texture>(std::string_view name) noexcept {
    return { name.data(), TextureTypeName };
}
template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<Shader>(std::string_view name) noexcept {
    return { name.data(), ShaderTypeName };
}
template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<ShaderProgram>(std::string_view name) noexcept {
    return { name.data(), ShaderProgramTypeName };
}
template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<Material>(std::string_view name) noexcept {
    return { name.data(), MaterialTypeName };
}
template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<FrameBuffer>(std::string_view name) noexcept {
    return { name.data(), FrameBufferTypeName };
}
template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<Mesh>(std::string_view name) noexcept {
    return { name.data(), MeshTypeName };
}
template<>
EmptyAssetData Assets::CreateEmptyAssetDataUsingType<Model>(std::string_view name) noexcept {
    return { name.data(), ModelTypeName };
}

template<>
void Assets::PerformPostDeserializationAction<Sampler>(const UUID uuid) noexcept {
    bridge.GetSamplers().Deallocate(uuid);
    std::vector<SamplerAllocatorMessage> messages = bridge.GetSamplers().Allocate(*this, uuid);

    std::vector<std::any>& errorMessages = database.errorLists[uuid];
    for (auto& message : messages) {
        errorMessages.emplace_back(std::move(message));
    }
}
template<>
void Assets::PerformPostDeserializationAction<Texture>(const UUID uuid) noexcept {
    bridge.GetTextures().Deallocate(uuid);
    std::vector<TextureAllocatorMessage> messages = bridge.GetTextures().Allocate(*this, uuid);

    std::vector<std::any>& errorMessages = database.errorLists[uuid];
    for (auto& message : messages) {
        errorMessages.emplace_back(std::move(message));
    }
}
template<>
void Assets::PerformPostDeserializationAction<Shader>(const UUID uuid) noexcept {
    bridge.GetShaders().Deallocate(uuid);
    std::vector<ShaderCompilerMessage> messages = bridge.GetShaders().Allocate(*this, uuid);

    std::vector<std::any>& infoMessages = database.infoLists[uuid];
    std::vector<std::any>& warningMessages = database.warningLists[uuid];
    std::vector<std::any>& errorMessages = database.errorLists[uuid];
    for (auto& message : messages) {
        switch (message.MessageType) {
        using enum ShaderCompilerMessage::Type;
        case Info:
            infoMessages.emplace_back(std::move(message));
            break;
        case Warning:
            warningMessages.emplace_back(std::move(message));
            break;
        case Error:
            errorMessages.emplace_back(std::move(message));
            break;
        default:
            std::unreachable();
        }
    }
}
template<>
void Assets::PerformPostDeserializationAction<ShaderProgram>(const UUID uuid) noexcept {
    bridge.GetShaderPrograms().Deallocate(uuid);
    std::vector<ShaderLinkerMessage> messages = bridge.GetShaderPrograms().Allocate(*this, uuid);

    std::vector<std::any>& errorMessages = database.errorLists[uuid];
    for (auto& message : messages) {
        errorMessages.emplace_back(std::move(message));
    }
}

size_t MaterialPostDeserialization::TypeNameToVariantIndex(std::string_view typeName) noexcept {
    // TODO refactor this to remove the magic numbers, see https://stackoverflow.com/questions/52303316/get-index-by-type-in-stdvariant
    if (typeName == "float")     { return 0uLL; }
    else if (typeName == "vec2") { return 1uLL; }
    else if (typeName == "vec3") { return 2uLL; }
    else if (typeName == "vec4") { return 3uLL; }

    //else if (typeName == "double") {}
    //else if (typeName == "dvec2")  {}
    //else if (typeName == "dvec3")  {}
    //else if (typeName == "dvec4")  {}

    else if (typeName == "int")   { return 4uLL; }
    else if (typeName == "ivec2") { return 5uLL; }
    else if (typeName == "ivec3") { return 6uLL; }
    else if (typeName == "ivec4") { return 7uLL; }

    else if (typeName == "unsigned int") { return 8uLL; }
    else if (typeName == "uvec2")        { return 9uLL; }
    else if (typeName == "uvec3")        { return 10uLL; }
    else if (typeName == "uvec4")        { return 11uLL; }

    else if (typeName == "bool")  { return 4uLL; }
    else if (typeName == "bvec2") { return 5uLL; }
    else if (typeName == "bvec3") { return 6uLL; }
    else if (typeName == "bvec4") { return 7uLL; }

    else if (typeName == "mat2")   { return 12uLL; }
    else if (typeName == "mat3")   { return 13uLL; }
    else if (typeName == "mat4")   { return 14uLL; }
    else if (typeName == "mat2x3") { return 15uLL; }
    else if (typeName == "mat2x4") { return 16uLL; }
    else if (typeName == "mat3x2") { return 17uLL; }
    else if (typeName == "mat3x4") { return 18uLL; }
    else if (typeName == "mat4x2") { return 19uLL; }
    else if (typeName == "mat4x3") { return 20uLL; }

    else if (typeName == "sampler2D") { return 21uLL; }
    else { DOA_LOG_WARNING("MaterialPostDeserialization::TypeNameToVariantIndex encountered unknown typeName %s", typeName.data()); return std::numeric_limits<unsigned long long>::max(); } // =)
}
void MaterialPostDeserialization::InsertUniform(Material::Uniforms& uniforms, int location, const UniformValue& uniform) noexcept {
    if (const Uniform1f* ptr = std::get_if<Uniform1f>(&uniform.Value))      { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const Uniform2f* ptr = std::get_if<Uniform2f>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const Uniform3f* ptr = std::get_if<Uniform3f>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const Uniform4f* ptr = std::get_if<Uniform4f>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }

    else if (const Uniform1i* ptr = std::get_if<Uniform1i>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const Uniform2i* ptr = std::get_if<Uniform2i>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const Uniform3i* ptr = std::get_if<Uniform3i>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const Uniform4i* ptr = std::get_if<Uniform4i>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }

    else if (const Uniform1ui* ptr = std::get_if<Uniform1ui>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const Uniform2ui* ptr = std::get_if<Uniform2ui>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const Uniform3ui* ptr = std::get_if<Uniform3ui>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const Uniform4ui* ptr = std::get_if<Uniform4ui>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }

    else if (const UniformMatrix2f* ptr = std::get_if<UniformMatrix2f>(&uniform.Value))     { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const UniformMatrix3f* ptr = std::get_if<UniformMatrix3f>(&uniform.Value))     { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const UniformMatrix4f* ptr = std::get_if<UniformMatrix4f>(&uniform.Value))     { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const UniformMatrix2x3f* ptr = std::get_if<UniformMatrix2x3f>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const UniformMatrix3x2f* ptr = std::get_if<UniformMatrix3x2f>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const UniformMatrix2x4f* ptr = std::get_if<UniformMatrix2x4f>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const UniformMatrix4x2f* ptr = std::get_if<UniformMatrix4x2f>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const UniformMatrix3x4f* ptr = std::get_if<UniformMatrix3x4f>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
    else if (const UniformMatrix4x3f* ptr = std::get_if<UniformMatrix4x3f>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }

    else if (const UniformSampler2D* ptr = std::get_if<UniformSampler2D>(&uniform.Value)) { uniforms.Set(location, uniform.Name, *ptr); }
}
void MaterialPostDeserialization::EmplaceUniform(Material::Uniforms& uniforms, int location, std::string_view typeName, std::string_view name, int arraySize) noexcept {
    assert(arraySize > 0);
    if (typeName == "float") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location + i, name, Uniform1f{});
        }
    } else if (typeName == "vec2") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform2f{});
        }
    } else if (typeName == "vec3") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform3f{});
        }
    } else if (typeName == "vec4") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform4f{});
        }
    }

    else if (typeName == "double") {}
    else if (typeName == "dvec2")  {}
    else if (typeName == "dvec3")  {}
    else if (typeName == "dvec4")  {}

    else if (typeName == "int") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform1i{});
        }
    } else if (typeName == "ivec2") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform2i{});
        }
    } else if (typeName == "ivec3") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform3i{});
        }
    } else if (typeName == "ivec4") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform4i{});
        }
    }

    else if (typeName == "unsigned int") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform1ui{});
        }
    } else if (typeName == "uvec2") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform2ui{});
        }
    } else if (typeName == "uvec3") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform3ui{});
        }
    } else if (typeName == "uvec4") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform4ui{});
        }
    }

    else if (typeName == "bool") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform1i{});
        }
    } else if (typeName == "bvec2") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform2i{});
        }
    } else if (typeName == "bvec3") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform3i{});
        }
    } else if (typeName == "bvec4") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, Uniform4i{});
        }
    }

    else if (typeName == "mat2")   {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformMatrix2f{});
        }
    } else if (typeName == "mat3")   {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformMatrix3f{});
        }
    } else if (typeName == "mat4")   {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformMatrix4f{});
        }
    } else if (typeName == "mat2x3") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformMatrix2x3f{});
        }
    } else if (typeName == "mat2x4") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformMatrix2x4f{});
        }
    } else if (typeName == "mat3x2") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformMatrix3x2f{});
        }
    } else if (typeName == "mat3x4") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformMatrix3x4f{});
        }
    } else if (typeName == "mat4x2") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformMatrix4x2f{});
        }
    } else if (typeName == "mat4x3") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformMatrix4x3f{});
        }
    }

    else if (typeName == "sampler1D") {
        DOA_LOG_WARNING("Uniform typename %s is still waiting implementation!", typeName.data());
    } else if (typeName == "sampler2D") {
        for (int i = 0; i < arraySize; i++) {
            uniforms.Set(location, name, UniformSampler2D{});
        }
    } else if (typeName == "sampler3D") {
        DOA_LOG_WARNING("Uniform typename %s is still waiting implementation!", typeName.data());
    }
    else {
        DOA_LOG_WARNING("Uniform typename %s is either unknown or not currently supported!", typeName.data());
    }
}

template<>
void Assets::PerformPostDeserializationAction<Material>(const UUID uuid) noexcept {
    Material& asset = database[uuid].DataAs<Material>();
    if (!asset.HasShaderProgram()) {
        asset.ClearAllUniforms();
        return;
    }
    assert(database.Contains(asset.ShaderProgram)); // Asset must exist, but GPU resource may not due to compilation-linking errors.
    //assert(bridge.GetShaderPrograms().Exists(asset.ShaderProgram));

    const GPUShaderProgram* program = bridge.GetShaderPrograms().Query(asset.ShaderProgram);
    if (program) {
        auto&& algorithm = [](Material::Uniforms& uniforms, ShaderType group, const GPUShaderProgram& program) {
            Material::Uniforms copy = std::move(uniforms);
            uniforms.Clear();

            auto& uniformList = copy.GetAll();

            for (auto& uniform : program.Uniforms) {
                if (uniform.ReferencedBy != group) { continue; }

                auto search = std::ranges::find_if(uniformList, [&uniform](auto& uniformValue) {
                    return uniformValue.Name == uniform.Name && uniformValue.Value.index() == MaterialPostDeserialization::TypeNameToVariantIndex(uniform.TypeName);
                });
                if (search != uniformList.end()) {
                    MaterialPostDeserialization::InsertUniform(uniforms, uniform.Location, *search);
                } else {
                    MaterialPostDeserialization::EmplaceUniform(uniforms, uniform.Location, uniform.TypeName, uniform.Name, uniform.ArraySize);
                }
            }
        };

        algorithm(asset.VertexUniforms, ShaderType::Vertex, *program);
        algorithm(asset.TessellationControlUniforms, ShaderType::TessellationControl, *program);
        algorithm(asset.TessellationEvaluationUniforms, ShaderType::TessellationEvaluation, *program);
        algorithm(asset.GeometryUniforms, ShaderType::Geometry, *program);
        algorithm(asset.FragmentUniforms, ShaderType::Fragment, *program);
    } else {
        std::vector<std::any>& errorMessages = database.errorLists[uuid];
        errorMessages.emplace_back(std::string("Material deserialization failed."));
        errorMessages.emplace_back(std::string("Referenced shader program failed to allocate. Check for errors in program!"));
    }
}
template<>
void Assets::PerformPostDeserializationAction<FrameBuffer>(const UUID uuid) noexcept {
    bridge.GetFrameBuffers().Deallocate(uuid);
    std::vector<FrameBufferAllocatorMessage> messages = bridge.GetFrameBuffers().Allocate(*this, uuid);

    std::vector<std::any>& errorMessages = database.errorLists[uuid];
    for (auto& message : messages) {
        errorMessages.emplace_back(std::move(message));
    }
}
template<>
void Assets::PerformPostDeserializationAction<Mesh>(const UUID uuid) noexcept {
    bridge.GetVertexBuffers().Deallocate(uuid);
    bridge.GetIndexBuffers().Deallocate(uuid);
    std::vector<BufferAllocatorMessage> vMessages = bridge.GetVertexBuffers().Allocate(*this, uuid);
    std::vector<BufferAllocatorMessage> iMessages = bridge.GetIndexBuffers().Allocate(*this, uuid);

    std::vector<std::any>& infoMessages = database.infoLists[uuid];
    std::vector<std::any>& warningMessages = database.warningLists[uuid];
    std::vector<std::any>& errorMessages = database.errorLists[uuid];
    for (auto& message : vMessages) {
        errorMessages.emplace_back(std::move(message));
    }
    for (auto& message : iMessages) {
        errorMessages.emplace_back(std::move(message));
    }
    if (errorMessages.empty()) {
        Mesh& mesh = GetDataOfAssetAs<Mesh>(uuid);

        assert(mesh.Vertices.has_value());
        Mesh::VertexList& vertices = mesh.Vertices.value();
        infoMessages.emplace_back(std::string("Mesh deserialized successfully."));
        infoMessages.emplace_back(std::format("\tVertex Count: {}", vertices.size()));
        infoMessages.emplace_back(std::format("\t\tVertex Buffer Size: {} (VRAM)", FormatBytes(static_cast<float>(std::span{ vertices }.size_bytes()))));
        if (mesh.Indices.has_value()) {
            Mesh::IndexList& indices = mesh.Indices.value();
            infoMessages.emplace_back(std::format("\tIndex Count: {}", indices.size()));
            infoMessages.emplace_back(std::format("\t\tIndex Buffer Size: {} (VRAM)", FormatBytes(static_cast<float>(std::span{ indices }.size_bytes()))));

            if (indices.size() % 3 != 0) {
                warningMessages.emplace_back(std::string("Index count not divisible by 3. Mesh faces might be ill-formed."));
            }
        } else {
            warningMessages.emplace_back(std::string("Mesh has no index table. This may affect performance."));
        }
    }
}
template<>
void Assets::PerformPostDeserializationAction<Model>(const UUID uuid) noexcept {
    AssetData& data = database.data[uuid];
    Model& model = std::get<Model>(data);

    for (const auto& mMesh : model.Meshes) {
        PerformPostDeserializationAction<Mesh>(mMesh.MeshUUID);
    }
    for (const auto& mTexture : model.Textures) {
        if (!mTexture.Embedded) { continue; }

        PerformPostDeserializationAction<Texture>(mTexture.TextureUUID);
    }
}

