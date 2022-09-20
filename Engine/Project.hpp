#pragma once

#include <string>
#include <memory>
#include <optional>
#include <filesystem>

#include "Assets.hpp"
#include "Scene.hpp"

struct FNode;

struct Project {

	inline static const std::string DEFAULT_PATH = std::getenv("USERPROFILE");

	Project(std::filesystem::path workspace, std::string name) noexcept;
	Project(std::filesystem::path workspace, std::string name, std::filesystem::path  startupLoc) noexcept;
	~Project() = default;
	Project(const Project& other) = delete;
	Project(Project&& other) noexcept;
	Project& operator=(const Project& other) = delete;
	Project& operator=(Project&& other) noexcept;

	std::filesystem::path Workspace() const;
	const std::string& WorkspaceName() const;
	const std::string& Name() const;

	Assets& Assets();

	void OpenStartupScene();
	AssetHandle GetStartupScene() const;
	void SetStartupScene(AssetHandle sceneFile);

	void OpenScene(AssetHandle sceneFile);
	void CloseScene();
	bool HasOpenScene() const;
	Scene& GetOpenScene();
	AssetHandle GetOpenSceneAsset() const;

	void SaveToDisk();
	void SaveOpenSceneToDisk();

private:
	std::filesystem::path _workspace;
	std::string _name;
	struct Assets _assets;
	AssetHandle _startupScene{};
	Scene* _openScene{ nullptr };
	AssetHandle _openSceneAsset{};
};