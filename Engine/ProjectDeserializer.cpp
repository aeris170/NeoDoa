#include <Engine/ProjectDeserializer.hpp>

#include <tinyxml2.h>

#include <Engine/FileNode.hpp>

ProjectDeserializationResult DeserializeProject(const FNode* file) {
    ProjectDeserializationResult result;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile(file->Path().string().c_str());
    if (err != tinyxml2::XML_SUCCESS) { result.erred = true; return result; }

    tinyxml2::XMLElement* rootNode = doc.RootElement();
    if (!rootNode) { result.erred = true; return result; }

    tinyxml2::XMLElement* nameElement = rootNode->FirstChildElement("name");
    if (!nameElement) { result.erred = true; return result; }
    std::string name = nameElement->GetText();

    tinyxml2::XMLElement* startupElement = rootNode->FirstChildElement("startup");
    if (!startupElement) { result.erred = true; return result; }
    UUID startupID = startupElement->Unsigned64Text(UUID::Empty());

    result.project = { file->FolderPath(), name, startupID };
    return result;
}
