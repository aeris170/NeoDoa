#include "ProjectDeserializer.hpp"

#include <tinyxml2.h>

#include "FileNode.hpp"
#include "Project.hpp"

Project DeserializeProject(const FNode* file) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile(file->Path().string().c_str());

	tinyxml2::XMLElement* rootNode = doc.RootElement();

	std::string name = rootNode->FirstChildElement("name")->GetText();
	UUID startupID = rootNode->FirstChildElement("startup")->Unsigned64Text(UUID::Empty());

	// TODO
	return { file->FolderPath(), name, startupID };
}
