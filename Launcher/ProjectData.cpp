#include <Launcher/ProjectData.hpp>

#include <tinyxml2.h>

#include <Engine/Assets.hpp>
#include <Engine/FileNode.hpp>
#include <Engine/ProjectDeserializer.hpp>

bool CheckProjectValidity(const ProjectData& data) noexcept {
    std::filesystem::path path = data.AbsolutePath;
    FNode projectFile{{
        .name = (path / data.Name).string(),
        .ext = Assets::ProjectExtension
    }};
    ProjectDeserializationResult pdr = DeserializeProject(&projectFile);
    return !pdr.erred;
}
std::string SerializeProjectDataCollection(const ProjectDataCollection& pdc) noexcept {
    tinyxml2::XMLPrinter printer;
    printer.PushComment("WARNING!! This file is not for editing! Don't!");

    printer.OpenElement("projects");
    for (const auto& project : pdc) {
        printer.OpenElement("project");
        printer.PushAttribute("favourite", project.IsFavourite);
        printer.PushAttribute("name", project.Name.c_str());
        printer.PushAttribute("path", project.AbsolutePath.c_str());
        printer.PushAttribute("lastOpened", project.LastOpened.c_str());
        printer.CloseElement();
    }
    printer.CloseElement();

    return printer.CStr();
}
ProjectDataCollectionSerializationResult DeserializeProjectDataCollection(std::string_view data) noexcept {
    ProjectDataCollectionSerializationResult result;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.data());
    tinyxml2::XMLElement* rootNode;

    if (err != tinyxml2::XML_SUCCESS) { goto error; }

    rootNode = doc.RootElement();
    if (!rootNode) { goto error; }

    for (tinyxml2::XMLElement* projectNode = rootNode->FirstChildElement(); projectNode != nullptr; projectNode = projectNode->NextSiblingElement()) {
        const tinyxml2::XMLAttribute* favouriteAttrib = projectNode->FindAttribute("favourite");
        const tinyxml2::XMLAttribute* nameAttrib = projectNode->FindAttribute("name");
        const tinyxml2::XMLAttribute* pathAttrib = projectNode->FindAttribute("path");
        const tinyxml2::XMLAttribute* lastOpenedAttrib = projectNode->FindAttribute("lastOpened");

        if (!favouriteAttrib || !nameAttrib || !pathAttrib || !lastOpenedAttrib) { goto error; }

        bool favourite = favouriteAttrib->BoolValue();
        std::string name = nameAttrib->Value();
        std::string path = pathAttrib->Value();
        std::string lastOpened = lastOpenedAttrib->Value();

        ProjectData& emplaced = result.pdc.emplace_back(true, favourite, std::move(name), std::move(path), std::move(lastOpened));
        emplaced.IsValid = CheckProjectValidity(emplaced);
    }
    return result;

error:
    result.erred = true;
    return result;
}