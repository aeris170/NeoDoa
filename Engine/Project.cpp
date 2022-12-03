#include "Project.hpp"

#include <tinyxml2.h>

#include "Asset.hpp"
#include "SceneSerializer.hpp"
#include "SceneDeserializer.hpp"
#include "ProjectSerializer.hpp"

Project::Project(std::filesystem::path workspace, std::string name, UUID startupSceneID) noexcept :
    _workspace(workspace),
    _name(name),
    _assets(this),
    _startupSceneID(startupSceneID) {
    OpenStartupScene();
}

Project::Project(Project&& other) noexcept :
    _workspace(std::move(other._workspace)),
    _name(std::move(other._name)),
    _assets(std::move(other._assets)),
    _startupSceneID(std::exchange(other._startupSceneID, UUID::Empty())),
    _openScene(std::exchange(other._openScene, nullptr)),
    _openSceneID(std::exchange(other._openSceneID, UUID::Empty())) {
}

Project& Project::operator=(Project&& other) noexcept {
    _workspace = std::move(other._workspace);
    _name = std::move(other._name);

    _assets = std::move(other._assets);

    _startupSceneID = std::exchange(other._startupSceneID, UUID::Empty());

    _openScene = std::exchange(other._openScene, nullptr);
    _openSceneID = std::exchange(other._openSceneID, UUID::Empty());
    return *this;
}

std::filesystem::path Project::Workspace() const { return _workspace; }
std::string Project::WorkspaceName() const { return _workspace.string(); }
const std::string& Project::Name() const { return _name; }

Assets& Project::Assets() { return _assets; }
const Assets& Project::Assets() const { return _assets; }

void Project::OpenStartupScene() { OpenScene(_startupSceneID); }
UUID Project::GetStartupScene() const { return _startupSceneID; }
void Project::SetStartupScene(UUID sceneID) { _startupSceneID = sceneID; }

void Project::OpenScene(UUID sceneID) {
    if (sceneID == UUID::Empty()) { return; }

    AssetHandle sceneAsset = _assets.FindAsset(sceneID);

    if (!sceneAsset.HasValue() || !sceneAsset.Value().IsScene()) { return; }

    sceneAsset->ForceDeserialize();
    _openScene = &sceneAsset->DataAs<Scene>();
    _openSceneID = sceneAsset->ID();
}
void Project::CloseScene() {
    _openScene = nullptr;
    AssetHandle openScene = _assets.FindAsset(_openSceneID);
    openScene->DeleteDeserializedData();
}
bool Project::HasOpenScene() const { return _openScene != nullptr; }
Scene& Project::GetOpenScene() { return *_openScene; }
UUID Project::GetOpenSceneID() const { return _openSceneID; }

void Project::SaveToDisk() {
    tinyxml2::XMLDocument doc;
    doc.Parse(SerializeProject(*this).c_str());
    doc.SaveFile((_workspace / (_name + ".doa")).string().c_str());
}

void Project::SaveOpenSceneToDisk() {
    if (!_openScene) return;

    AssetHandle openScene = _assets.FindAsset(_openSceneID);
    openScene->UpdateData(std::move(*_openScene));
    openScene->Serialize();
    OpenScene(openScene->ID());
}
