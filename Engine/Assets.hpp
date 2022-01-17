#pragma once

#include <string>
#include <filesystem>
#include <cstdlib>

#include "FileNode.hpp"

struct Project;

struct Assets {

	inline static std::string SCENE_EXT = ".scn";
	static bool IsSceneFile(const FNode& node);
	inline static std::string SCRIPT_EXT = ".scrpt";
	static bool IsScriptFile(const FNode& node);
	inline static std::string TEXTURE_EXT = ".tex";
	static bool IsTextureFile(const FNode& node);
	inline static std::string MODEL_EXT = ".mdl";
	static bool IsModelFile(const FNode& node);
	inline static std::string MATERIAL_EXT = ".mat";
	static bool IsMaterialFile(const FNode& node);
	inline static std::string SHADER_EXT = ".shdr";
	static bool IsShaderFile(const FNode& node);

	const Project* project{ nullptr };

	FNode _root;

	std::vector<FNode*> _scenes;
	std::vector<FNode*> _scripts;
	std::vector<FNode*> _textures;
	std::vector<FNode*> _models;
	std::vector<FNode*> _materials;
	std::vector<FNode*> _shaders;

	Assets(const Project* owner) noexcept;

	void ReScan();
	FNode* Find(std::string path);
	FNode* CreateNewSceneFileNode(std::string_view relativePath, std::string_view name);

private:
	friend struct Project;
};