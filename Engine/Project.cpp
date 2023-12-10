#include "Project.hpp"

#include "Core.hpp"
#include "Scene.hpp"
#include "Assets.hpp"

Project::Project(const std::filesystem::path& workspace, const std::string& name, UUID startupSceneID) noexcept :
    workspace(workspace),
    name(name),
    startupSceneID(startupSceneID) {}

std::filesystem::path Project::Workspace() const { return workspace; }
std::string Project::WorkspaceName() const { return workspace.string(); }
const std::string& Project::Name() const { return name; }

void Project::OpenStartupScene() { OpenScene(startupSceneID); }
UUID Project::GetStartupScene() const { return startupSceneID; }
void Project::SetStartupScene(UUID sceneID) { startupSceneID = sceneID; }

void Project::OpenScene(UUID sceneID) {
    if (sceneID == UUID::Empty()) { return; }

    AssetHandle sceneAsset = Core::GetCore()->Assets()->FindAsset(sceneID);

    if (!sceneAsset.HasValue()) {
        DOA_LOG_ERROR("Cannot open scene! Asset with id %ul not found!", sceneID);
        return;
    }
    if (!sceneAsset->IsScene()) {
        DOA_LOG_ERROR("Cannot open scene! Asset with id %ul is not a scene!", sceneID);
        return;
    }

    sceneAsset->ForceDeserialize();
    openSceneID = sceneAsset->ID();
}
void Project::CloseScene() {
    UUID id = openSceneID;

    openSceneID = UUID::Empty();

    AssetHandle openScene = Core::GetCore()->Assets()->FindAsset(id);
    if (!openScene.HasValue()) { return; }
    openScene->DeleteDeserializedData();
}
bool Project::HasOpenScene() const { return openSceneID != UUID::Empty(); }
Scene& Project::GetOpenScene() { return Core::GetCore()->Assets()->FindAsset(openSceneID)->DataAs<Scene>(); }
UUID Project::GetOpenSceneID() const { return openSceneID; }

void Project::SaveOpenSceneToDisk() {
    if (!HasOpenScene()) return;

    AssetHandle openScene = Core::GetCore()->Assets()->FindAsset(openSceneID);
    openScene->Serialize();
    OpenScene(openScene->ID());
}
