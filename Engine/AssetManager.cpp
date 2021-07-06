#include "AssetManager.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include <stb_image.h>

#include "Log.hpp"

static void ReadRecursive(FNode& root);
static void FindFilesFor(AssetManager& assetManager, FNode& root);
static bool IsSceneFile(const FNode& node);
static bool IsScriptFile(const FNode& node);
static bool IsTextureFile(const FNode& node);
static bool IsModelFile(const FNode& node);
static bool IsMaterialFile(const FNode& node);
static bool IsShaderFile(const FNode& node);

AssetManager::AssetManager(std::string_view dir) noexcept :
	_dir(dir) {
	auto path = std::filesystem::path(_dir);
	_root._path = path.string();
	_root._name = path.filename().string();
	_root._name_noext = _root._name.substr(0, _root._name.find_last_of("."));
	_root._ext = path.extension().string();
	_root._parent = nullptr;
	_root._children.clear();
	_root._isFile = false;
	_root._isDir = true;
	ReScan();
}

AssetManager::AssetManager(AssetManager&& other) noexcept {
	*this = std::move(other);
}

AssetManager& AssetManager::operator=(AssetManager&& other) noexcept {
	_dir = std::move(other._dir);
	_root = std::move(other._root);
	_scenes = std::move(other._scenes);
	_scripts = std::move(other._scripts);
	_textures = std::move(other._textures);
	_models = std::move(other._models);
	_materials = std::move(other._materials);
	_shaders = std::move(other._shaders);
	return *this;
}

void AssetManager::ReScan() {
	_root._children.clear();
	ReadRecursive(_root);
	FindFilesFor(*this, _root);
}

FNode* AssetManager::Find(std::string fullpath) {
	FNode* cur = &_root;
	do {
		auto index = fullpath.find_first_of('\\');
		std::string path;
		if (index != std::string::npos) {
			path = fullpath.substr(0, index);
			fullpath = fullpath.substr(index + 1);
		} else {
			path = fullpath;
		}
		for (auto& node : cur->_children) {
			if (node._name == path) {
				cur = &node;
				break;
			}
		}
	} while (cur->_children.size() != 0);
	if (!cur || cur->_isDir) {
		return nullptr;
	}
	return cur;
}

//-----------------------------------------------------------------

static void ReadRecursive(FNode& root) {
	auto it = std::filesystem::directory_iterator(root._path);
	for (const auto& entry : it) {
		bool isFile = entry.is_regular_file();
		bool isDir = entry.is_directory();
		root._children.emplace_back(FNode{
			entry.path().string(),
			entry.path().filename().string(),
			entry.path().filename().string().substr(0, entry.path().filename().string().find_last_of(".")),
			entry.path().extension().string(),
			&root,
			{},
			isFile,
			isDir
		});
	}
	// DON'T merge this loop with the above.
	for (auto& child : root._children) {
		if (child._isDir) {
			ReadRecursive(child);
		}
	}
}

static void FindFilesFor(AssetManager& assetManager, FNode& root) {
	for (auto& node : root._children) {
		if(node._isFile) {
			if (IsSceneFile(node)) {
				assetManager._scenes.push_back(&node);
			} else if (IsScriptFile(node)) {
				assetManager._scripts.push_back(&node);
			} else if (IsTextureFile(node)) {
				assetManager._textures.push_back(&node);
			} else if (IsModelFile(node)) {
				assetManager._models.push_back(&node);
			} else if (IsMaterialFile(node)) {
				assetManager._materials.push_back(&node);
			} else if (IsShaderFile(node)) {
				assetManager._shaders.push_back(&node);
			}
		} else if (node._isDir) {
			FindFilesFor(assetManager, node);
		}
	}
}

static bool IsSceneFile(const FNode& node) {
	return node._ext == ".scn";
}

static bool IsScriptFile(const FNode& node) {
	return node._ext == ".as";
}

static bool IsTextureFile(const FNode& node) {
	return node._ext == ".png" || node._ext == ".jpg" || node._ext == ".jpeg";
}

static bool IsModelFile(const FNode& node) {
	return node._ext == ".obj" || node._ext == ".fbx" || node._ext == "3ds";
}

static bool IsMaterialFile(const FNode& node) {
	return node._ext == ".mat";
}

static bool IsShaderFile(const FNode& node) {
	return node._ext == ".sh";
}