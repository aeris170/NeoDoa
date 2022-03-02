#pragma once

#include <string>
#include <optional>
#include <memory>

#include "Assets.hpp"
#include "Scene.hpp"

struct FNode;

struct Project {

	inline static const std::string DEFAULT_PATH = std::getenv("USERPROFILE");

	std::string _workspace;
	std::string _name;
	Assets _assets;
	FNode* _startupScene{ nullptr };
	std::optional<Scene> _openScene{ std::nullopt };
	FNode* _openSceneFile{ nullptr };

	Project(std::string workspace, std::string name) noexcept;
	Project(std::string workspace, std::string name, std::string startupLoc) noexcept;
	~Project() = default;
	Project(const Project& other) = delete;
	Project(Project&& other) noexcept;
	Project& operator=(const Project& other) = delete;
	Project& operator=(Project&& other) noexcept;

	Assets& Assets();
	void OpenStartupScene();
	void OpenScene(FNode* sceneFile);
	bool HasOpenScene() const;
	Scene& GetOpenScene();

	void SaveToDisk();
	void SaveOpenSceneToDisk();
};