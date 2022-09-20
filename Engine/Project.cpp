#include "Project.hpp"

#include <tinyxml2.h>

#include "Asset.hpp"
#include "SceneSerializer.hpp"
#include "SceneDeserializer.hpp"
#include "ProjectSerializer.hpp"

Project::Project(std::filesystem::path workspace, std::string name) noexcept :
	_workspace(workspace),
	_name(name),
	_assets(this) {}

Project::Project(std::filesystem::path workspace, std::string name, std::filesystem::path startupLoc) noexcept :
	Project(workspace, name) {
	if (startupLoc != "NULL") {
		_startupScene = _assets.FindAsset(startupLoc);
		if (_startupScene.HasValue()) {
			_startupScene->ForceDeserialize();
			_openScene = &_startupScene->DataAs<Scene>();
			_openSceneAsset = _startupScene;
		}
	}
}

Project::Project(Project&& other) noexcept :
	_workspace(std::move(other._workspace)),
	_name(std::move(other._name)),
	_assets(std::move(other._assets)),
	_startupScene(std::exchange(other._startupScene, nullptr)),
	_openScene(std::exchange(other._openScene, nullptr)),
	_openSceneAsset(std::exchange(other._openSceneAsset, nullptr)) {
	_assets.project = this;
}

Project& Project::operator=(Project&& other) noexcept {
	_workspace = std::move(other._workspace);
	_name = std::move(other._name);

	_assets = std::move(other._assets);
	_assets.project = this;

	_startupScene = std::exchange(other._startupScene, nullptr);

	_openScene = std::exchange(other._openScene, nullptr);
	_openSceneAsset = std::exchange(other._openSceneAsset, nullptr);
	return *this;
}

std::filesystem::path Project::Workspace() const { return _workspace; }
const std::string& Project::WorkspaceName() const { return _workspace.string(); }
const std::string& Project::Name() const { return _name; }

Assets& Project::Assets() { return _assets; }

void Project::OpenStartupScene() { OpenScene(_startupScene); }
AssetHandle Project::GetStartupScene() const { return _startupScene; }
void Project::SetStartupScene(AssetHandle sceneFile) { _startupScene = sceneFile; }

void Project::OpenScene(AssetHandle sceneAsset) {
	if (!sceneAsset.HasValue() || !sceneAsset.Value().IsScene()) { return; }

	sceneAsset->ForceDeserialize();
	_openScene = &sceneAsset->DataAs<Scene>();
	_openSceneAsset = sceneAsset;
}
void Project::CloseScene() {
	_openScene = nullptr;
	_openSceneAsset->DeleteDeserializedData();
	_openSceneAsset.Reset();
}
bool Project::HasOpenScene() const { return _openScene != nullptr; }
Scene& Project::GetOpenScene() { return *_openScene; }
AssetHandle Project::GetOpenSceneAsset() const { return _openSceneAsset; }

void Project::SaveToDisk() {
	tinyxml2::XMLDocument doc;
	doc.Parse(SerializeProject(*this).c_str());
	doc.SaveFile((_workspace / (_name + ".doa")).string().c_str());
}

void Project::SaveOpenSceneToDisk() {
	if (!_openScene) return;

	_openSceneAsset->Serialize();
}
