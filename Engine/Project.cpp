#include "Project.hpp"

#include <tinyxml2.h>

#include "SceneSerializer.hpp"
#include "SceneDeserializer.hpp"
#include "ProjectSerializer.hpp"

Project::Project(std::string workspace, std::string name) noexcept :
	_workspace(workspace),
	_name(name),
	_assets(this) {}

Project::Project(std::string workspace, std::string name, std::string startupLoc) noexcept :
	Project(workspace, name) {
	if (startupLoc != "NULL") {
		_startupScene = _assets.Find(startupLoc);
		_openScene = DeserializeScene(_startupScene);
		_openSceneFile = _startupScene;
	}
}

Project::Project(Project&& other) noexcept :
	_workspace(std::move(other._workspace)),
	_name(std::move(other._name)),
	_assets(std::move(other._assets)),
	_startupScene(std::exchange(other._startupScene, nullptr)),
	_openScene(std::move(other._openScene)),
	_openSceneFile(std::exchange(other._openSceneFile, nullptr)) {
	_assets.project = this;
}

Project& Project::operator=(Project&& other) noexcept {
	_workspace = std::move(other._workspace);
	_name = std::move(other._name);

	_assets = std::move(other._assets);
	_assets.project = this;

	_startupScene = std::exchange(other._startupScene, nullptr);

	_openScene = std::move(other._openScene);
	_openSceneFile = std::exchange(other._openSceneFile, nullptr);
	return *this;
}

Assets& Project::Assets() {
	return _assets;
}

void Project::OpenStartupScene() {
	OpenScene(_startupScene);
}

void Project::OpenScene(FNode* sceneFile) {
	if (sceneFile == nullptr || !Assets::IsSceneFile(*sceneFile)) { return; }

	_openScene.emplace(DeserializeScene(sceneFile));
	_openSceneFile = sceneFile;
}

void Project::SaveToDisk() {
	tinyxml2::XMLDocument doc;
	doc.Parse(SerializeProject(*this).c_str());
	doc.SaveFile((_workspace + "\\" + _name + ".doa").c_str());
}

void Project::SaveOpenSceneToDisk() {
	if (!_openScene) return;
	tinyxml2::XMLDocument doc;
	doc.Parse(SerializeScene(_openScene.value()).c_str());
	doc.SaveFile(_openSceneFile->_path.c_str());
}
