#include "Assets.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <utility>

#include <stb_image.h>

#include "Project.hpp"
#include "Log.hpp"
#include "SceneSerializer.hpp"

AssetHandle::AssetHandle() noexcept :
	_asset(nullptr) {}

AssetHandle::AssetHandle(Asset* const asset) noexcept :
	_asset(asset) {}

Asset& AssetHandle::operator*() const { return *_asset; }
Asset* AssetHandle::operator->() const { return _asset; }
AssetHandle::operator Asset*() const { return _asset; }
AssetHandle::operator bool() const { return HasValue(); }

bool AssetHandle::HasValue() const { return _asset != nullptr; }
Asset& AssetHandle::Value() const { return *_asset; }
void AssetHandle::Reset() { _asset = nullptr; }

bool Assets::IsSceneFile(const FNode* file) { return file->ext == SCENE_EXT; }
bool Assets::IsScriptFile(const FNode* file) { return file->ext == SCRIPT_EXT; }
bool Assets::IsTextureFile(const FNode* file) { return file->ext == TEXTURE_EXT; }
bool Assets::IsModelFile(const FNode* file) { return file->ext == MODEL_EXT; }
bool Assets::IsMaterialFile(const FNode* file) { return file->ext == MATERIAL_EXT; }
bool Assets::IsShaderFile(const FNode* file) { return file->ext == SHADER_EXT; }

Assets::Assets(const Project* owner) noexcept :
	project(owner),
	_root({ owner, nullptr, "", "", "", true }) {
	BuildFileNodeTree(_root);
	ImportAllFiles(database, _root);
}

Assets::Assets(Assets&& other) noexcept :
	project(std::exchange(other.project, nullptr)),
	_root(std::move(other._root)),
	database(std::move(other.database)),
	files(std::move(other.files)),
	sceneAssets(std::move(other.sceneAssets)),
	scriptAssets(std::move(other.scriptAssets)),
	textureAssets(std::move(other.textureAssets)),
	modelAssets(std::move(other.modelAssets)),
	shaderAssets(std::move(other.shaderAssets)),
	shaderUniformBlockAssets(std::move(other.shaderUniformBlockAssets)) {
}

Assets& Assets::operator=(Assets&& other) noexcept {
	project = std::exchange(other.project, nullptr);
	_root = std::move(other._root);
	database = std::move(other.database);
	files = std::move(other.files);
	sceneAssets = std::move(other.sceneAssets);
	scriptAssets = std::move(other.scriptAssets);
	textureAssets = std::move(other.textureAssets);
	modelAssets = std::move(other.modelAssets);
	shaderAssets = std::move(other.shaderAssets);
	shaderUniformBlockAssets = std::move(other.shaderUniformBlockAssets);
	return *this;
}

FNode& Assets::CreateFolder(FNode& parentFolder, const std::string_view folderName) {
	FNode& rv = *parentFolder.CreateChildFolder({ parentFolder.owner, &parentFolder, std::string(folderName) });
	ReimportAll();
	return rv;
}
void Assets::MoveFolder(FNode& folder, FNode& targetParentFolder) {
	folder.MoveUnder(targetParentFolder);
	ReimportAll();
}
void Assets::DeleteFolder(FNode& folder) {
	folder.Delete();
	ReimportAll();
}

void Assets::MoveAsset(const AssetHandle asset, FNode& targetParentFolder) {
	if (!asset.HasValue()) { return; }
	asset->File()->MoveUnder(targetParentFolder);
	ReimportAll();
}
void Assets::DeleteAsset(const AssetHandle asset) {
	if (!asset.HasValue()) { return; }

	UUID id = asset->ID();

	database.erase(id);
	std::erase(sceneAssets, id);
	std::erase(scriptAssets, id);
	std::erase(textureAssets, id);
	std::erase(modelAssets, id);
	std::erase(shaderAssets, id);
	std::erase(shaderUniformBlockAssets, id);

	asset->File()->Delete();

	ReimportAll();
}

AssetHandle Assets::FindAsset(UUID uuid) {
	if (!database.contains(uuid)) { return nullptr; }
	return { &database[uuid] };
}
AssetHandle Assets::FindAssetAt(const FNode& file) const {
	for (auto& [uuid, asset] : database) {
		if (asset.File() == &file) {
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

const Assets::UUIDCollection& Assets::SceneAssetIDs() const { return sceneAssets; }
const Assets::UUIDCollection& Assets::ScriptAssetIDs() const { return scriptAssets; }
const Assets::UUIDCollection& Assets::TextureAssetIDs() const { return textureAssets; }
const Assets::UUIDCollection& Assets::ModelAssetIDs() const { return modelAssets; }
const Assets::UUIDCollection& Assets::ShaderAssetIDs() const { return shaderAssets; }
const Assets::UUIDCollection& Assets::ShaderUniformBlockAssetIDs() const { return shaderUniformBlockAssets; }

void Assets::ReimportAll() {
	_root = FNode(FNodeCreationParams{ project, nullptr, "", "", "", true });
	BuildFileNodeTree(_root);
	ImportAllFiles(database, _root);
}

AssetHandle Assets::ImportFile(AssetDatabase& database, const FNode& file) {
	/* Import a file:
	    * Step 1: Get the sibling file fileName.fileExtension.id
	    * Step 2: Check if such file exists
	    * Step 3: If not exists, create it, generate a UUID and write the UUID
	    * Step 4: Read the file for the UUID
	    * Step 5: If there is a collision, resolve it by generating new UUID's until the collision is resolved
	    * Step 6: Register the file into the database
	    * Step 7: Call the importer to import the content to the memory
		* Step 8: Separate imported asset to its own subcategory
    */
	if (file.IsDirectory()) { return nullptr; }
	if (file.ext == ID_EXT) { return nullptr; }
	// Step 1
	FNode importData = FNode::HollowCopy(file);
	importData.ext.append(ID_EXT);
	importData.fullName.append(ID_EXT);

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
		auto result = database.emplace(uuid, Asset{ uuid, const_cast<FNode*>(&file) });
		Asset& asset = std::get<Asset>(*result.first);
		files.emplace(&file, asset.ID());

	    // Step 7
		asset.Deserialize();

		// Step 8
		if (asset.IsScene()) {
			sceneAssets.push_back(asset.ID());
		}

		return &asset;
	} else {
		DOA_LOG_ERROR("Failed to import asset at %s do you have write access to the directory?", file.Path());
		return nullptr;
	}
}

void Assets::ImportAllFiles(AssetDatabase& database, const FNode& root) {
	database.clear();
	ImportFile(database, root);
	for (auto& child : root.Children()) {
		ImportAllFiles(database, *child);
	}
}

void Assets::BuildFileNodeTree(FNode& root) {
	std::filesystem::current_path(project->Workspace());
	auto it = std::filesystem::directory_iterator(project->Workspace() / root.Path());
	for (const auto& entry : it) {
		root.children.push_back(new FNode({
			project,
			&root,
			entry.path().filename().replace_extension().string(),
			entry.path().extension().string(),
			"",
			entry.is_directory()
		}));
	}
	// DON'T merge this loop with the above.
	for (auto& child : root.children) {
		if (child->IsDirectory()) {
			BuildFileNodeTree(*child);
		}
	}
}
