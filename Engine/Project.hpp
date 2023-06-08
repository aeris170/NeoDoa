#pragma once

#include <string>
#include <memory>
#include <optional>
#include <filesystem>

#include "UUID.hpp"

struct FNode;
struct Scene;

struct Project {

    inline static const std::string DEFAULT_PATH = std::getenv("USERPROFILE");

    Project(const std::filesystem::path& workspace, const std::string& name, UUID startupSceneID = UUID::Empty()) noexcept;
    ~Project() = default;
    Project(const Project& other) = delete;
    Project(Project&& other) noexcept = default;
    Project& operator=(const Project& other) = delete;
    Project& operator=(Project&& other) noexcept = default;

    std::filesystem::path Workspace() const;
    std::string WorkspaceName() const;
    const std::string& Name() const;

    void OpenStartupScene();
    UUID GetStartupScene() const;
    void SetStartupScene(UUID sceneID);

    void OpenScene(UUID sceneID);
    void CloseScene();
    bool HasOpenScene() const;
    Scene& GetOpenScene();
    UUID GetOpenSceneID() const;

    void SaveOpenSceneToDisk();

private:
    std::filesystem::path workspace;
    std::string name;
    UUID startupSceneID{ UUID::Empty() };
    Scene* openScene{ nullptr };
    UUID openSceneID{ UUID::Empty() };
};