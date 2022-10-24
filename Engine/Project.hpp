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

    Project(std::filesystem::path workspace, std::string name, UUID startupSceneID = UUID::Empty()) noexcept;
    ~Project() = default;
    Project(const Project& other) = delete;
    Project(Project&& other) noexcept;
    Project& operator=(const Project& other) = delete;
    Project& operator=(Project&& other) noexcept;

    std::filesystem::path Workspace() const;
    std::string WorkspaceName() const;
    const std::string& Name() const;

    Assets& Assets();
    const struct Assets& Assets() const;

    void OpenStartupScene();
    UUID GetStartupScene() const;
    void SetStartupScene(UUID sceneID);

    void OpenScene(UUID sceneID);
    void CloseScene();
    bool HasOpenScene() const;
    Scene& GetOpenScene();
    UUID GetOpenSceneID() const;

    void SaveToDisk();
    void SaveOpenSceneToDisk();

private:
    std::filesystem::path _workspace;
    std::string _name;
    struct Assets _assets;
    UUID _startupSceneID{ UUID::Empty() };
    Scene* _openScene{ nullptr };
    UUID _openSceneID{ UUID::Empty() };
};