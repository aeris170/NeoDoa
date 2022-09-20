#include "ProjectDeserializer.hpp"

#include <tinyxml2.h>

#include "FileNode.hpp"
#include "Project.hpp"

Project DeserializeProject(const FNode* file) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile(file->Path().string().c_str());

	tinyxml2::XMLElement* rootNode = doc.RootElement();

	std::string name = rootNode->FirstChildElement("name")->GetText();
	std::string startupLoc = rootNode->FirstChildElement("startup")->GetText();

	// TODO
	return { file->FolderPath(), name, startupLoc};
}
