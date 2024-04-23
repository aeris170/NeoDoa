#include <Engine/Assets.hpp>

#include <string>
#include <utility>

#include <Engine/Core.hpp>
#include <Engine/SceneSerializer.hpp>

AssetHandle::AssetHandle() noexcept :
    _asset(nullptr) {}
AssetHandle::AssetHandle(Asset* const asset) noexcept :
    _asset(asset) {}
Asset& AssetHandle::operator*() const noexcept { return *_asset; }
Asset* AssetHandle::operator->() const noexcept { return _asset; }
AssetHandle::operator Asset* () const noexcept { return _asset; }
AssetHandle::operator bool() const noexcept { return HasValue(); }

bool AssetHandle::HasValue() const { return _asset != nullptr; }
Asset& AssetHandle::Value() const { return *_asset; }
void AssetHandle::Reset() { _asset = nullptr; }

bool Assets::IsProjectFile(const FNode& file) noexcept { return file.ext == ProjectExtension; }
bool Assets::IsSceneFile(const FNode& file) noexcept { return file.ext == SceneExtension; }
bool Assets::IsComponentDefinitionFile(const FNode& file) noexcept { return file.ext == ComponentDefinitionExtension; }
bool Assets::IsSamplerFile(const FNode& file) noexcept { return file.ext == SamplerExtension; }
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
bool Assets::IsVertexShaderFile(const FNode& file) noexcept { return file.ext == VertexShaderExtension; }
bool Assets::IsTessellationControlShaderFile(const FNode& file) noexcept { return file.ext == TessellationControlShaderExtension; }
bool Assets::IsTessellationEvaluationShaderFile(const FNode& file) noexcept { return file.ext == TessellationEvaluationShaderExtension; }
bool Assets::IsGeometryShaderFile(const FNode& file) noexcept { return file.ext == GeometryShaderExtension; }
bool Assets::IsFragmentShaderFile(const FNode& file) noexcept { return file.ext == FragmentShaderExtension; }
bool Assets::IsComputeShaderFile(const FNode & file) noexcept { return file.ext == ComputeShaderExtension; }
bool Assets::IsShaderProgramFile(const FNode& file) noexcept { return file.ext == ShaderProgramExtension; }
bool Assets::IsMaterialFile(const FNode& file) noexcept { return file.ext == MaterialExtension; }
bool Assets::IsScriptFile(const FNode& file) noexcept { return file.ext == SCRIPT_EXT; }
bool Assets::IsModelFile(const FNode& file) noexcept { return file.ext == MODEL_EXT; }

Assets::Assets(const Project& project, AssetGPUBridge& bridge) noexcept :
    _root({ &project, nullptr, "", "", "", true }),
    bridge(bridge) {
    BuildFileNodeTree(project, _root);
    ImportAllFiles(database, _root);
}

FNode& Assets::CreateFolder(FNode& parentFolder, const std::string_view folderName) {
    FNode& rv = *parentFolder.CreateChildFolder({ parentFolder.owner, &parentFolder, std::string(folderName) });
    ReimportAll(); /* folder structure is mutated, must reimport to re-build! */
    return rv;
}
void Assets::MoveFolder(FNode& folder, FNode& targetParentFolder) {
    folder.MoveUnder(targetParentFolder);
    ReimportAll(); /* folder structure is mutated, must reimport to re-build! */
}
void Assets::DeleteFolder(FNode& folder) {
    folder.Delete();
    ReimportAll(); /* folder structure is mutated, must reimport to re-build! */
}

void Assets::SaveAsset(const AssetHandle asset) {
    if (!asset.HasValue()) { return; }
    asset->Serialize();
}
void Assets::MoveAsset(const AssetHandle asset, FNode& targetParentFolder) {
    if (!asset.HasValue()) { return; }
    asset->File().MoveUnder(targetParentFolder);
    ReimportAll();
}
void Assets::DeleteAsset(const AssetHandle asset) {
    if (!asset.HasValue()) { return; }

    files.erase(&asset->File());
    asset->File().Delete();

    UUID id = asset->ID();
    database.erase(id);
    std::erase(allAssets, id);
    std::erase(sceneAssets, id);
    std::erase(componentDefinitionAssets, id);
    std::erase(shaderAssets, id);
    std::erase(shaderProgramAssets, id);
    std::erase(materialAssets, id);
    std::erase(textureAssets, id);

    dependencyGraph.RemoveVertex(id);
    //ReimportAll();
}

AssetHandle Assets::FindAsset(UUID uuid) const {
    if (!database.contains(uuid)) { return nullptr; }

    /*
    * casting away const is safe here
    * because database does not contain "const Asset*"
    * in the first place, it contains "Asset*"
    */
    return { const_cast<Asset*>(&database.at(uuid)) };
}
AssetHandle Assets::FindAssetAt(const FNode& file) const {
    for (const auto& [uuid, asset] : database) {
        if (asset.File() == file) {
            /*
            * casting away const is safe here
            * because database does not contain "const Asset*"
            * in the first place, it contains "Asset*"
            */
            return { const_cast<Asset*>(&asset) };
        }
    }
    return nullptr;
}
bool Assets::IsAssetExistsAt(const FNode& file) const { return files.contains(&file); }

FNode& Assets::Root() { return _root; }
const FNode& Assets::Root() const { return _root; }

const Assets::UUIDCollection& Assets::AllAssetsIDs() const { return allAssets; }
const Assets::UUIDCollection& Assets::SceneAssetIDs() const { return sceneAssets; }
const Assets::UUIDCollection& Assets::ScriptAssetIDs() const { return scriptAssets; }
const Assets::UUIDCollection& Assets::TextureAssetIDs() const { return textureAssets; }
const Assets::UUIDCollection& Assets::ComponentDefinitionAssetIDs() const { return componentDefinitionAssets; }
const Assets::UUIDCollection& Assets::ModelAssetIDs() const { return modelAssets; }
const Assets::UUIDCollection& Assets::ShaderAssetIDs() const { return shaderAssets; }
const Assets::UUIDCollection& Assets::ShaderProgramAssetIDs() const { return shaderProgramAssets; }
const Assets::UUIDCollection& Assets::MaterialAssetIDs() const { return materialAssets; }
const Assets::UUIDCollection& Assets::SamplerAssetIDs() const { return samplerAssets; }

const AssetGPUBridge& Assets::GPUBridge() const { return bridge; }

AssetHandle Assets::Import(const FNode& file) { return ImportFile(database, file); }
void Assets::ReimportAll() {
    database.clear();
    allAssets.clear();
    sceneAssets.clear();
    componentDefinitionAssets.clear();
    shaderAssets.clear();
    shaderProgramAssets.clear();
    materialAssets.clear();
    samplerAssets.clear();
    textureAssets.clear();

    _root.children.clear();
    dependencyGraph.Clear();
    BuildFileNodeTree(*_root.OwningProject(), _root);
    ImportAllFiles(database, _root);
    EnsureDeserialization();
}
// donkey donk
void Assets::EnsureDeserialization() {
    Deserialize(componentDefinitionAssets);
    Deserialize(sceneAssets);
    Deserialize(textureAssets);
    Deserialize(samplerAssets);
    Deserialize(shaderAssets);
    Deserialize(shaderProgramAssets);
    Deserialize(materialAssets);

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

void Assets::OnNotify(const ObserverPattern::Observable* source, ObserverPattern::Notification message) {
    if (message == "deserialized"_hs) {
        const Asset* asset = dynamic_cast<const Asset*>(source);
        assert(asset); // must be non-null
        const UUID origin = asset->ID();

        if (asset->IsScene())               { PerformPostDeserializationAction<Scene>        (origin); }
        if (asset->IsComponentDefinition()) { PerformPostDeserializationAction<Component>    (origin); }
        if (asset->IsShader())              { PerformPostDeserializationAction<Shader>       (origin); }
        if (asset->IsShaderProgram())       { PerformPostDeserializationAction<ShaderProgram>(origin); }
        if (asset->IsMaterial())            { PerformPostDeserializationAction<Material>     (origin); }
        if (asset->IsSampler())             { PerformPostDeserializationAction<Sampler>      (origin); }
        if (asset->IsTexture())             { PerformPostDeserializationAction<Texture>      (origin); }

        if (dependencyGraph.HasVertex(origin)) {
            auto edgeVertices = dependencyGraph.GetIncomingEdgesOf(origin);
            while (edgeVertices.HasNext()) {
                const UUID& dependentID = edgeVertices.Next();
                assert(database.contains(dependentID));
                database[dependentID].ForceDeserialize();
            }
        }
    }
    if (message == "data_deleted"_hs || message == "destructed"_hs) {
        const Asset* asset = dynamic_cast<const Asset*>(source);
        assert(asset); // must be non-null
        if (asset->IsScene())               {}
        if (asset->IsComponentDefinition()) {}
        if (asset->IsShader())              { bridge.GetShaders().Deallocate(asset->ID());        }
        if (asset->IsShaderProgram())       { bridge.GetShaderPrograms().Deallocate(asset->ID()); }
        if (asset->IsMaterial())            {}
        if (asset->IsSampler())             {}
        if (asset->IsTexture())             {}
    }
}

AssetHandle Assets::ImportFile(AssetDatabase& database, const FNode& file) {
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
        * Step 9: Set ownself as imported asset's Observer and return
    */
    if (IsProjectFile(file)) { return nullptr; }
    if (file.IsDirectory()) { return nullptr; }
    if (file.ext == AssetIDExtension) { return nullptr; }
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
        printer.PushText(UUID());
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
        while (database.contains(uuid)) {
            // collision detected! generate new UUID
            uuid = UUID();
            doc.RootElement()->FirstChildElement("uuid")->SetText(uuid);
            doc.SaveFile(importData.AbsolutePath().string().c_str());
        }

        // Step 6
        auto&& [itr, result] = database.emplace(uuid, Asset{ uuid, const_cast<FNode*>(&file) });
        auto&& [id, asset] = *itr;
        files.emplace(&file, id);

        // Step 7
        // asset.Deserialize();

        // Step 8
        allAssets.push_back(id);
        if (asset.IsScene()) {
            sceneAssets.push_back(id);
        }
        if (asset.IsComponentDefinition()) {
            componentDefinitionAssets.push_back(id);
        }
        if (asset.IsShader()) {
            shaderAssets.push_back(id);
        }
        if (asset.IsShaderProgram()) {
            shaderProgramAssets.push_back(id);
        }
        if (asset.IsMaterial()) {
            materialAssets.push_back(id);
        }
        if (asset.IsSampler()) {
            samplerAssets.push_back(id);
        }
        if (asset.IsTexture()) {
            textureAssets.push_back(id);
        }

        asset.AddObserver(*this);
        dependencyGraph.AddVertex(id);
        return &asset;
    } else {
        DOA_LOG_ERROR("Failed to import asset at %s do you have read/write access to the directory?", std::quoted(file.Path().c_str()));
        return nullptr;
    }
}
void Assets::ImportAllFiles(AssetDatabase& database, const FNode& root) {
    ImportFile(database, root);
    for (const auto& child : root.Children()) {
        ImportAllFiles(database, child);
    }
}
void Assets::Deserialize(const UUIDCollection& assets) {
    for (const UUID id : assets) {
        database[id].DeleteDeserializedData();
        database[id].ForceDeserialize();
    }
}

void Assets::BuildFileNodeTree(const Project& project, FNode& root) {
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
        if (asset.IsSampler()) {}
        if (asset.IsTexture()) {}
    }
}

template <>
void Assets::PerformPostDeserializationAction<Texture>(UUID id) noexcept {
    bridge.GetTextures().Deallocate(id);
    std::vector<TextureAllocatorMessage> messages = bridge.GetTextures().Allocate(*this, id);

    // Cast-away const. Assets are never created const.
    const Asset& asset{ database[id] };
    std::vector<std::any>& errorMessages = const_cast<std::vector<std::any>&>(asset.ErrorMessages());
    for (auto& message : messages) {
        errorMessages.emplace_back(std::move(message));
    }
}
template <>
void Assets::PerformPostDeserializationAction<Shader>(UUID id) noexcept {
    bridge.GetShaders().Deallocate(id);
    std::vector<ShaderCompilerMessage> messages = bridge.GetShaders().Allocate(*this, id);

    // Cast-away const. Assets are never created const.
    const Asset& asset{ database[id] };
    std::vector<std::any>& infoMessages = const_cast<std::vector<std::any>&>(asset.InfoMessages());
    std::vector<std::any>& warningMessages = const_cast<std::vector<std::any>&>(asset.WarningMessages());
    std::vector<std::any>& errorMessages = const_cast<std::vector<std::any>&>(asset.ErrorMessages());
    for (auto& message : messages) {
        switch (message.MessageType) {
        using enum ShaderCompilerMessageType;
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
template <>
void Assets::PerformPostDeserializationAction<ShaderProgram>(UUID id) noexcept {
    bridge.GetShaderPrograms().Deallocate(id);
    std::vector<ShaderLinkerMessage> messages = bridge.GetShaderPrograms().Allocate(*this, id);

    // Cast-away const. Assets are never created const.
    const Asset& asset{ database[id] };
    std::vector<std::any>& errorMessages = const_cast<std::vector<std::any>&>(asset.ErrorMessages());
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
    else { DOA_LOG_WARNING("MaterialPostDeserialization::TypeNameToVariantIndex encountered unknown typeName %s", typeName.data()); return -1uLL; } // =)
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

template <>
void Assets::PerformPostDeserializationAction<Material>(UUID id) noexcept {
    Material& asset = database[id].DataAs<Material>();
    if (!asset.HasShaderProgram()) {
        asset.ClearAllUniforms();
        return;
    }
    assert(database.contains(asset.ShaderProgram));
    assert(bridge.GetShaderPrograms().Exists(asset.ShaderProgram));

    const GPUShaderProgram& program = bridge.GetShaderPrograms().Fetch(asset.ShaderProgram);

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

    algorithm(asset.VertexUniforms, ShaderType::Vertex, program);
    algorithm(asset.TessellationControlUniforms, ShaderType::TessellationControl, program);
    algorithm(asset.TessellationEvaluationUniforms, ShaderType::TessellationEvaluation, program);
    algorithm(asset.GeometryUniforms, ShaderType::Geometry, program);
    algorithm(asset.FragmentUniforms, ShaderType::Fragment, program);
}