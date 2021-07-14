#include "Project.hpp"

#include "tinyxml2.h"

#include "SceneDeserializer.hpp"

Project::Project(std::string path) noexcept :
	_name(""),
	_assets(path.substr(0, path.find_last_of('\\'))) {
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.c_str());
	_name = doc.FirstChildElement("project")->FirstChildElement("name")->FirstChild()->ToText()->Value();
	std::string startupLoc = doc.FirstChildElement("project")->FirstChildElement("startup")->FirstChild()->ToText()->Value();
	if (startupLoc != "") {
		FNode* sceneNode = _assets.Find(startupLoc);
		_startupScene = DeserializeScene(sceneNode);
	}
}

Project::Project(std::string path, std::string name) noexcept :
	_name(name),
	_assets(path) {
}

void Project::Serialize(std::string path) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* root = doc.NewElement("project");
	root->InsertNewChildElement("name")->SetText(_name.c_str());
	root->InsertNewChildElement("startup")->SetText("");
	doc.InsertFirstChild(root);
	doc.SaveFile((path + "\\" + _name + ".doa").c_str());
	_assets.ReScan();
}

void Project::Close() {

}
