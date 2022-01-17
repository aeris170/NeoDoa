#include "Assets.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include <stb_image.h>

#include "Project.hpp"
#include "Log.hpp"
#include "SceneSerializer.hpp"

static void ReadRecursive(FNode& root);
static void FindFilesFor(Assets& assetManager, FNode& root);

Assets::Assets(const Project* owner) noexcept :
	project(owner) {
	auto path = std::filesystem::path(project->_workspace);
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

FNode* Assets::CreateNewSceneFileNode(std::string_view relativePath, std::string_view name) {
	tinyxml2::XMLDocument doc;
	doc.Parse(SerializeScene(Scene(name)).c_str());

	std::string path = project->_workspace;
	path.append("\\");
	path.append(relativePath);
	path.append(name);
	path.append(SCENE_EXT);
	doc.SaveFile(path.c_str());

	ReScan();
	return Find(path);
}

void Assets::ReScan() {
	_root._children.clear();
	ReadRecursive(_root);
	FindFilesFor(*this, _root);
}

FNode* Assets::Find(std::string fullpath) {
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

bool Assets::IsSceneFile(const FNode& node) {
	return node._ext == ".scn";
}

bool Assets::IsScriptFile(const FNode& node) {
	return node._ext == ".as";
}

bool Assets::IsTextureFile(const FNode& node) {
	return node._ext == ".png" || node._ext == ".jpg" || node._ext == ".jpeg";
}

bool Assets::IsModelFile(const FNode& node) {
	return node._ext == ".obj" || node._ext == ".fbx" || node._ext == "3ds";
}

bool Assets::IsMaterialFile(const FNode& node) {
	return node._ext == ".mat";
}

bool Assets::IsShaderFile(const FNode& node) {
	return node._ext == ".sh";
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

static void FindFilesFor(Assets& assetManager, FNode& root) {
	for (auto& node : root._children) {
		if(node._isFile) {
			if (Assets::IsSceneFile(node)) {
				assetManager._scenes.push_back(&node);
			} else if (Assets::IsScriptFile(node)) {
				assetManager._scripts.push_back(&node);
			} else if (Assets::IsTextureFile(node)) {
				assetManager._textures.push_back(&node);
			} else if (Assets::IsModelFile(node)) {
				assetManager._models.push_back(&node);
			} else if (Assets::IsMaterialFile(node)) {
				assetManager._materials.push_back(&node);
			} else if (Assets::IsShaderFile(node)) {
				assetManager._shaders.push_back(&node);
			}
		} else if (node._isDir) {
			FindFilesFor(assetManager, node);
		}
	}
}
