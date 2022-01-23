#include "ProjectDeserializer.hpp"

#include <tinyxml2.h>

#include "FileNode.hpp"
#include "Project.hpp"

Project DeserializeProject(FNode* file) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile(file->_path.c_str());

	tinyxml2::XMLElement* rootNode = doc.RootElement();

	std::string name = rootNode->FirstChildElement("name")->GetText();
	std::string startupLoc = rootNode->FirstChildElement("startup")->GetText();

	return { FNode::GetFolderPath(file)._path, name, startupLoc };
}
