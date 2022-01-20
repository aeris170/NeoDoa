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

	FNode& Root();
	const FNode& Root() const;

	std::vector<FNode*>& Scenes();
	const std::vector<FNode*>& Scenes() const;

	std::vector<FNode*>& Scripts();
	const std::vector<FNode*>& Scripts() const;

	std::vector<FNode*>& Textures();
	const std::vector<FNode*>& Textures() const;

	std::vector<FNode*>& Models();
	const std::vector<FNode*>& Models() const;

	std::vector<FNode*>& Materials();
	const std::vector<FNode*>& Materials() const;

	std::vector<FNode*>& Shaders();
	const std::vector<FNode*>& Shaders() const;

	Assets(const Project* owner) noexcept;

	void ReScan();
	FNode* Find(std::string path);
	FNode* CreateNewSceneFileNode(std::string_view relativePath, std::string_view name);

	~Assets() = default;
	Assets(const Assets& other) = default;
	Assets(Assets&& other) noexcept;
	Assets& operator=(const Assets& other) = default;
	Assets& operator=(Assets&& other) noexcept;

private:
	const Project* project{ nullptr };

	FNode _root;

	std::vector<FNode*> _scenes;
	std::vector<FNode*> _scripts;
	std::vector<FNode*> _textures;
	std::vector<FNode*> _models;
	std::vector<FNode*> _materials;
	std::vector<FNode*> _shaders;

	static void ReadRecursive(FNode& root);
	static void FindFilesFor(Assets& assetManager, FNode& root);

	friend struct Project;
};