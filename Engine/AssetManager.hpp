#pragma once

#include <string>
#include <filesystem>
#include <cstdlib>

#include "FileNode.hpp"

struct AssetManager {

	inline static const std::string DEFAULT_PATH = std::getenv("USERPROFILE");

	std::string _dir;

	FNode _root;

	std::vector<FNode*> _scenes;
	std::vector<FNode*> _scripts;
	std::vector<FNode*> _textures;
	std::vector<FNode*> _models;
	std::vector<FNode*> _materials;
	std::vector<FNode*> _shaders;

	AssetManager(std::string_view dir = std::filesystem::current_path().string()) noexcept;
	~AssetManager() = default;
	AssetManager(const AssetManager&) = delete;
	AssetManager(AssetManager&&) noexcept;
	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager& operator=(AssetManager&&) noexcept;
	void ReScan();
	FNode* Find(std::string path);
};