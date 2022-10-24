#include "ProjectSerializer.hpp"

#include <tinyxml2.h>

#include "Asset.hpp"
#include "Project.hpp"
#include "Scene.hpp"

std::string SerializeProject(const Project& project) {
    tinyxml2::XMLPrinter printer;

    printer.PushComment("WARNING!! This file is not for editing! Don't!");
    printer.OpenElement("project");
    {
        printer.OpenElement("name");
        {
            printer.PushText(project.Name().c_str());
        }
        printer.CloseElement(); // name close
        printer.OpenElement("startup");
        {
            const AssetHandle startup = project.Assets().FindAsset(project.GetStartupScene());
            if (startup.HasValue()) {
                printer.PushText(startup->ID());
            } else {
                printer.PushText(UUID::Empty());
            }
        }
        printer.CloseElement(); // name close
    }
    printer.CloseElement(); // project close
    return printer.CStr();
}
